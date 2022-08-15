#include "include/imageprocessor.h"
#include <QDebug>
#include <algorithm>
#include <math.h>
#include <QQueue>
#include <QDate>
#include <QDir>
#include "include/algorithms.h"

ImageProcessor::ImageProcessor(QObject *parent) : QObject(parent)
{
}

void ImageProcessor::posterize()
{
    if(m_imageProvider->contains("edges"))
        removeEdgesFromProvider();
    if(m_imageProvider->contains("coloring"))
        removeColoringFromProvider();
    QImage filtered(m_currentImage.size(), QImage::Format_RGB888);
    QImage posterized(m_currentImage.size(), QImage::Format_RGB888);

    Algorithms::medianFilter(m_currentImage, filtered, 3, 2);

    Algorithms::averagingFilter(filtered, filtered, 3, 2);
    Algorithms::posterize(filtered, posterized, m_colors);
    Algorithms::medianFilter(posterized, posterized, 3, 3);

    m_imageProvider->add("posterized", posterized);
}

bool ImageProcessor::setCurrentImage(const QString &source)
{
    // file:///C:/....
    QString src = source.right(source.size() - 8);
    if(!m_currentImage.load(src))
    {
        qDebug() << "ImageProcessor: Не удалось загрузить изображение";
        return false;
    }
    if(m_imageProvider->contains("edges"))
        m_imageProvider->remove("edges");
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
    QImage posterized = m_imageProvider->get("posterized");
    QImage edgesImage = Algorithms::makeEdgesImage(posterized);
    m_imageProvider->add("edges", edgesImage);
}

void ImageProcessor::coloring()
{
    if(m_imageProvider->contains("coloring"))
        return;
    edges();
    QImage posterizedImage{m_imageProvider->get("posterized")};
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
    m_imageProvider->remove("coloring");
    m_imageProvider->remove("painted");
    m_imageProvider->remove("legend");
}

void ImageProcessor::saveResults(const QString& folderPath, int tileRows,
                                 int tileColumns)
{
    QString path = folderPath + '/';
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

