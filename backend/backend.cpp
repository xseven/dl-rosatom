#include "backend.h"

#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QCoreApplication>
#include <QDir>

Backend::Backend(qint32 port, QObject* parent)
	: QObject(parent)
{
	websocketServer = std::make_unique<QWebSocketServer>(QStringLiteral("backend"), QWebSocketServer::NonSecureMode, this);

    if (websocketServer->listen(QHostAddress::Any, port)) {
        qDebug() << QStringLiteral("Listening on port: %1").arg(QString::number(port));

        connect(websocketServer.get(), &QWebSocketServer::newConnection,
            this, &Backend::onNewConnection);        
    } else {
        qCritical() << QStringLiteral("Unable to listen on port: %1").arg(QString::number(port));
    }
}

Backend::~Backend()
{
    std::for_each(std::cbegin(connections), std::cend(connections)
        , [](const auto& connection) { connection->deleteLater(); });
    connections.clear();
}

void Backend::onNewConnection() noexcept
{
    auto connection = websocketServer->nextPendingConnection();

    connections.push_back(connection);

    qDebug() << QStringLiteral("Client connected [peer name: %1] [origin: %2]").arg(connection->peerName()).arg(connection->origin());

    connect(connection, &QWebSocket::textMessageReceived, this, &Backend::processTextMessage);
    connect(connection, &QWebSocket::binaryMessageReceived, this, &Backend::processBinaryMessage);
    connect(connection, &QWebSocket::disconnected, this, &Backend::disconnected);
}

void Backend::processTextMessage(const QString& message) noexcept
{
    qDebug() << QStringLiteral("Text message: %1").arg(message);
}

void Backend::processBinaryMessage(const QByteArray& message) noexcept
{
    qDebug() << QStringLiteral("Binary message: %1").arg(QString::fromUtf8(message));

    auto connection = qobject_cast<QWebSocket*>(sender());

    auto object = QJsonDocument::fromJson(message).object();

    auto cmd = object.value(QStringLiteral("cmd")).toString();

    if (cmd == QStringLiteral("getdata")) {
        QProcess process;
        QString scriptFile = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/Rosatom_docs.py");

        QStringList pythonCommandArguments = QStringList() << scriptFile
            << object.value(QStringLiteral("position")).toString() << object.value(QStringLiteral("department")).toString() << object.value(QStringLiteral("keywords")).toString();
        
        QFile file("./docs/results_table.json");
        file.remove();

        QObject::connect(&process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
            qDebug() << "process error: " << error;
        });

        process.start("python", pythonCommandArguments);
        process.waitForFinished(-1);
        auto conStdOut = process.readAllStandardOutput();
        auto conStdErr = process.readAllStandardError();
        qDebug() << "exit code: " << process.exitCode() << " error: " << process.error();
        qDebug() << "Process stdout: " << conStdOut;
        qDebug() << "Process stderr: " << conStdErr;
                
        process.close();
                
        file.setFileName("./docs/results_table.json");
        
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            auto data = file.readAll();
            file.close();

            connection->sendBinaryMessage(data);
        }
    }

#if 0
    QProcess process;
    QString scriptFile = QCoreApplication::applicationDirPath() + "../../scriptPath/script.py";

    QStringList pythonCommandArguments = QStringList() << scriptFile
        << "-f " << parameter1 << "-t" << parameter2 << "-v"
        << parameter3 << "-e" << parameter4;

    printf("PyCommand: %s\n", qStringToStdString(pythonCommandArguments.join(' ')).c_str());

    process.start("python", pythonCommandArguments);

    {
        QFile file;
        file.setFileName("result_table.json");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        auto data = file.readAll();
        file.close();

        connection->sendBinaryMessage(data);

        /*
        auto doc = QJsonDocument::fromJson(data);

        const auto& entries = doc.array();

        for (const auto& entry : entries) {
            auto entryItem = entry.toObject();

            int id = entryItem.value("id").toInt();
            QString position = entryItem.value("Position").toString();
            QString path = entryItem.value("path").toString();

            qDebug() << id << " " << position << " " << path;
        }
        */
    }
#endif
}

void Backend::disconnected() noexcept
{
    auto connectionToRemove = qobject_cast<QWebSocket*>(sender());
    qDebug() << QStringLiteral("Disconnected [code: %1] [reason: %2]")
        .arg(QString::number(connectionToRemove->closeCode()))
        .arg(connectionToRemove->closeReason());

    auto connectionIt = std::find(std::cbegin(connections), std::cend(connections), connectionToRemove);
    
    if (connectionIt != std::cend(connections)) {
        auto connectionPos = std::distance(std::cbegin(connections), connectionIt);

        connections.at(connectionPos)->deleteLater();
        connections.erase(connectionIt);        
    } else {
        qCritical() << QStringLiteral("Attempting to remove nonexisting connection");
    }
}