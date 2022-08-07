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
    {
        m_colors.push_back(QColor(color));
    }
}

QStringList ImageProcessor::findOptimalPalette(int colorsCount)
{
    if(m_currentImage.isNull())
    {
        qDebug() << "Изображение не задано";
        return QStringList();
    }

//    auto uniqueColors = _getUniqueColors();
//    if(uniqueColors.size() <= colorsCount)
//    {
//      QStringList result;
//      for(const auto& col: uniqueColors)
//        result << col.name(QColor::HexRgb);
//      return result;
//    }


    QQueue<QList<QColor>> pixelsQueue;
    pixelsQueue.enqueue(Algorithms::getImagePixels(m_currentImage));

    QList<QList<QColor>> colorsList;
    int count = colorsCount;
    while(pixelsQueue.size() < count)
    {
      int queueSize = pixelsQueue.size();
      for(int i = 0; i < queueSize; ++i)
      {
        auto forSplit = pixelsQueue.dequeue();  // извлекаем из очереди
        auto splitted = Algorithms::splitPixels(forSplit); // разбиваем
        if(splitted.first.isEmpty() || splitted.second.isEmpty()) // не разбивается => записываем в список и больше не рассматриваем
        {
          colorsList << forSplit;
          --count; // так как в очереди будет меньше элементов
        }
        else
        {
          pixelsQueue.enqueue(splitted.first); // добавляем в конец очереди оба => стало на 1 больше, чем до извлечения
          pixelsQueue.enqueue(splitted.second);
          if(pixelsQueue.size() == count)
            break;
        }
      }
    }

    while(!pixelsQueue.isEmpty())
      colorsList << pixelsQueue.dequeue();

//    auto it = pixelsQueue.begin();
//    while(it != pixelsQueue.end())
//    {
//        if(it->size() == 0)
//            it = pixelsQueue.erase(it);
//        else ++it;
//    }

    QStringList result;

    for(const auto& colors: colorsList)
    {
        QColor paletteColor = Algorithms::averageColor(colors);
        result << paletteColor.name(QColor::HexRgb);
    }

    return result;
}

void ImageProcessor::changeColor(int x, int y, const QColor &color)
{
    QImage img = m_imageProvider->get("posterized");
    QColor srcColor = img.pixelColor(x, y);
    for(int y = 0; y < img.height(); ++y)
      for(int x = 0; x < img.width(); ++x)
        if(img.pixelColor(x, y) == srcColor)
          img.setPixelColor(x, y, color);
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

    // нахождение краёв
    QImage edgesImage(posterized.size(), QImage::Format_RGB888);
    edgesImage.fill(Qt::white);
    for(int y = 0; y < posterized.height() - 1; ++y)
      for(int x = 0; x < posterized.width() - 1; ++x)
      {
        QColor current = posterized.pixelColor(x, y);
        QColor right = posterized.pixelColor(x + 1, y);
        QColor bottom = posterized.pixelColor(x, y + 1);
        if(current != right || current != bottom)
        {
            edgesImage.setPixelColor(x, y, Qt::black);
        }
      }


    // рамка
    for(int x = 0; x < edgesImage.width(); ++x)
    {
      edgesImage.setPixelColor(x, 0, Qt::black);
      edgesImage.setPixelColor(x, edgesImage.height() - 1, Qt::black);
    }
    for(int y = 1; y < edgesImage.height() - 1; ++y)
    {
      edgesImage.setPixelColor(0, y, Qt::black);
      edgesImage.setPixelColor(edgesImage.width() - 1, y, Qt::black);
    }

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

    for(int y = 0; y < edgesImage.height(); ++y)
      for(int x = 0; x < edgesImage.width(); ++x)
      {
        QColor pixColor = edgesImage.pixelColor(x, y);
        if(pixColor == Qt::white)
        {
          QColor posterizedColor = posterizedImage.pixelColor(x, y);
          QString colorName = posterizedColor.name(QColor::HexRgb);
          if(!colorsMap.contains(colorName))
          {
            int id = colorsMap.size();
            colorsMap[colorName] = id;
          }
          QList<QPoint> filledPixels;
          Algorithms::fill(x, y, edgesImage, Qt::green, &filledPixels);
          auto contourPixels{Algorithms::findContour(edgesImage, filledPixels)};
          DataTypes::Area area{contourPixels, filledPixels,
                               colorsMap[colorName]};
          areas << area;
          QString str;
        }
      }

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
    QString folderName = _generateSaveFolderName();
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

QString ImageProcessor::_generateSaveFolderName()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString suffix = dateTime.toString("dd-MMMM-yy_hh-mm-ss");
    return "Coloring_" + suffix;
}
