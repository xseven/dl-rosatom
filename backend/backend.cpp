#include "backend.h"

#include <QWebSocket>
#include <QTextCodec>
#include <QDebug>
#include <QFile>

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
    qDebug() << QStringLiteral("Binary message: %1").arg(QString::fromUtf8(message.toHex()));
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