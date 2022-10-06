#include "include/imageprocessor.h"
#include <QDebug>
#include <algorithm>
#include <math.h>
#include <QQueue>
#include <QDate>
#include <QDir>
#include "include/algorithms.h"

ImageProvider* ImageProcessor::m_imageProvider;
ConfigManager* ImageProcessor::m_configManager;

ImageProcessor::ImageProcessor(QObject *parent) : QObject(parent)
{
}

void ImageProcessor::posterize()
{
    if(m_imageProvider->contains("scaled_posterized"))
        m_imageProvider->remove("scaled_posterized");
    if(m_imageProvider->contains("edges"))
        removeEdgesFromProvider();
    if(m_imageProvider->contains("coloring"))
        removeColoringFromProvider();
    QImage filtered(m_currentImage.size(), QImage::Format_RGB888);
    QImage posterized(m_currentImage.size(), QImage::Format_RGB888);



    if(m_imageProvider->contains("filtered"))
        filtered = m_imageProvider->get("filtered");
    else
    {
        Algorithms::medianFilter(m_currentImage, filtered, 3, 2);
        Algorithms::averagingFilter(filtered, filtered, 3, 2);
        m_imageProvider->add("filtered", filtered);
    }
    Algorithms::posterize(filtered, posterized, m_colors);
    Algorithms::medianFilter(posterized, posterized, 3, 3);

    m_imageProvider->add("posterized", posterized);
}

bool ImageProcessor::setCurrentImage(const QString &source)
{
    // file:///C:/....
    int offset = 8;
#ifdef  __linux__
    offset = 7;
#endif
    QString src = source.right(source.size() - offset);
    if(!m_currentImage.load(src))
    {
        qDebug() << "ImageProcessor: Не удалось загрузить изображение";
        return false;
    }
    if(m_imageProvider->contains("edges"))
        m_imageProvider->remove("edges");
    if(m_imageProvider->contains("filtered"))
        m_imageProvider->remove("filtered");
    return true;
}

void ImageProcessor::setColors(const QStringList &colors)
{
    if(!m_colors.isEmpty())
        m_colors.clear();
    for(auto &color: colors)
        m_colors.push_back(QColor(color));
}

QStringList ImageProcessor::findOptimalPalette(int colorsCount)
{
    if(m_currentImage.isNull())
    {
        qDebug() << "Изображение не задано";
        return QStringList();
    }
    auto palette = Algorithms::findOptimalPalette(m_currentImage, colorsCount);
    return palette;
}

void ImageProcessor::changeColor(int x, int y, const QColor &color)
{
    QImage img = m_imageProvider->get("posterized");
    Algorithms::changeColor(img, x, y, color);
    m_imageProvider->add("posterized", img);
}

void ImageProcessor::setPixelColor(int x, int y, const QColor &color)
{
    QImage img = m_imageProvider->get("posterized");
    img.setPixelColor(x, y, color);
    m_imageProvider->add("posterized", img);
}

void ImageProcessor::fill(int x, int y, const QColor &fillColor)
{
    QImage img = m_imageProvider->get("posterized");
    Algorithms::fill(x, y, img, fillColor);
    m_imageProvider->add("posterized", img);
}

void ImageProcessor::edges()
{
    if(m_imageProvider->contains("edges"))
        return;
    QImage posterized = m_imageProvider->get("scaled_posterized");

    QImage edgesImage = Algorithms::makeEdgesImage(posterized);
    m_imageProvider->add("edges", edgesImage);
}

void ImageProcessor::coloring()
{
    if(m_imageProvider->contains("coloring"))
        return;
    scalePosterizedImage();
    edges();
    QImage posterizedImage{m_imageProvider->get("scaled_posterized")};
    QImage edgesImage{m_imageProvider->get("edges")};

    QMap<QString, int> colorsMap; // color -> id
    QList<DataTypes::Area> areas;
    Algorithms::findAreas(posterizedImage, edgesImage, areas, colorsMap);

    QColor coloringColor{m_configManager->coloringColor()};
    m_imageCreator.setColoringColor(coloringColor);

    if(m_configManager->simplify())
        m_imageCreator.createSimplifiedImages(posterizedImage, areas,
                                              colorsMap);
    else
        m_imageCreator.createImages(posterizedImage, areas, colorsMap);
    QImage coloringImage = m_imageCreator.getColoringImage();
    QImage paintedImage = m_imageCreator.getPaintedImage();
    QImage legend = m_imageCreator.getLegendImage();

    m_imageProvider->add("coloring", coloringImage);
    m_imageProvider->add("painted", paintedImage);
    m_imageProvider->add("legend", legend);
}

void ImageProcessor::removeEdgesFromProvider()
{
    m_imageProvider->remove("edges");
}

void ImageProcessor::removeColoringFromProvider()
{
    m_imageProvider->remove("scaled_posterized");
    m_imageProvider->remove("edges");
    m_imageProvider->remove("coloring");
    m_imageProvider->remove("painted");
    m_imageProvider->remove("legend");
}

void ImageProcessor::saveResults(const QString& folderPath, int tileRows,
                                 int tileColumns)
{
    QString path;
#ifdef __linux__
    path += '/';
#endif
    path += folderPath + '/';
    if(!m_imageProvider->contains("coloring"))
        return;
    QImage coloring = m_imageProvider->get("coloring");
    QImage painted = m_imageProvider->get("painted");
    QImage legend = m_imageProvider->get("legend");
    QString folderName = "Coloring_";
    folderName += QDateTime::currentDateTime().toString("dd-MMMM-yy_hh-mm-ss");
    QDir dir(path);
    dir.mkdir(folderName);
    path += folderName + '/';
    QString coloringPath = path + "coloring.png";
    QString paintedPath = path + "painted.png";
    QString legendPath = path + "legend.png";
    coloring.save(coloringPath, "PNG");
    painted.save(paintedPath, "PNG");
    legend.save(legendPath, "PNG");
    if(tileRows > 0)
    {
        dir.cd(folderName);
        dir.mkdir("tiled");
        path += "tiled/";
        auto tiles = m_imageCreator.tileColoringImage(tileRows, tileColumns);
        for(const auto& tile: tiles)
            tile.first.save(path + tile.second + ".png", "PNG");
    }
}

void ImageProcessor::scalePosterizedImage()
{
    if(m_imageProvider->contains("scaled_posterized"))
        return;
    QImage posterized = m_imageProvider->get("posterized");
    int scalingFactor = m_configManager->scalingFactor();
    QImage scaledPosterized;
    switch(scalingFactor)
    {
    case 2:
        scaledPosterized = Algorithms::scaleImage2x(posterized);
        break;
    case 3:
        scaledPosterized = Algorithms::scaleImage3x(posterized);
        break;
    case 4:
        scaledPosterized = Algorithms::scaleImage2x(posterized, 2);
        break;
    case 6:
        scaledPosterized = Algorithms::scaleImage2x(posterized);
        scaledPosterized = Algorithms::scaleImage3x(scaledPosterized);
        break;
    case 8:
        scaledPosterized = Algorithms::scaleImage2x(posterized, 3);
        break;
    case 9:
        scaledPosterized = Algorithms::scaleImage3x(posterized, 2);
        break;
    case 12:
        scaledPosterized = Algorithms::scaleImage3x(posterized);
        scaledPosterized = Algorithms::scaleImage2x(scaledPosterized, 2);
        break;
    default:
        scaledPosterized = posterized;
        break;
    }
    m_imageProvider->add("scaled_posterized", scaledPosterized);
}

