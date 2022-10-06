#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QQmlContext>
#include <QIcon>
#include "include/imageprovider.h"
#include "include/imageprocessor.h"
#include "include/configmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    app.setApplicationName("Создание раскрасок по номерам");
    app.setWindowIcon(QIcon(":/icons/app-icon.png"));

    ImageProvider imageProvider;
    ConfigManager configManager;

    ImageProcessor::setImageProvider(&imageProvider);
    ImageProcessor::setConfigManager(&configManager);

    QQmlApplicationEngine engine;

    qmlRegisterType<ImageProcessor>("com.MyTypes.ImageProcessor", 1, 0,
                                    "ImageProcessor");
    engine.rootContext()->setContextProperty("configManager", &configManager);

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
