#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QQmlContext>
#include "include/imageprovider.h"
#include "include/imageprocessor.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    ImageProvider imageProvider;
    ImageProcessor imageProcessor;
    imageProcessor.setImageProvider(&imageProvider);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("imageProcessor", &imageProcessor);

    const QUrl url(QStringLiteral("qrc:/QML/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);


    engine.addImageProvider("provider", &imageProvider);
    engine.load(url);

    return app.exec();
}
