#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include "imageprovider.h"
#include "configmanager.h"
#include "imagecreator.h"

class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = nullptr);
    void setImageProvider(ImageProvider* provider) {m_imageProvider = provider;}
    void setConfigManager(ConfigManager* configManager)
                         {m_configManager = configManager;}
    Q_INVOKABLE void posterize();
    Q_INVOKABLE bool setCurrentImage(const QString& source);
    Q_INVOKABLE void setColors(const QStringList& colors);
    Q_INVOKABLE QStringList findOptimalPalette(int colorsCount);
    Q_INVOKABLE void changeColor(int x, int y, const QColor& color);
    Q_INVOKABLE void setPixelColor(int x, int y, const QColor& color);
    Q_INVOKABLE void fill(int x, int y, const QColor& fillColor);
    Q_INVOKABLE void edges();
    Q_INVOKABLE void coloring();
    Q_INVOKABLE void removeEdgesFromProvider();
    Q_INVOKABLE void removeColoringFromProvider();
    Q_INVOKABLE void saveResults(const QString& folderPath,
                                 int tileRows = 0, int tileColumns = 0);
private:
    QImage m_currentImage;
    ImageProvider *m_imageProvider;
    ConfigManager *m_configManager;
    ImageCreator m_imageCreator;
    QList<QColor> m_colors;
    QString _generateSaveFolderName();
signals:

};

#endif // IMAGEPROCESSOR_H
