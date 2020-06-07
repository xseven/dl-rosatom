#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QTextCodec>

#include "datamanipulator.h"

int main(int argc, char *argv[])
{    
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app,
                     [url](QObject *obj, const QUrl &objUrl) {
                        if (!obj && url == objUrl)
                            QCoreApplication::exit(-1);
                     }
    , Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
