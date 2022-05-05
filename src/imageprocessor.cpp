#include "include/imageprocessor.h"
#include <QDebug>
#include <algorithm>
#include <math.h>
#include <QQueue>
ImageProcessor::ImageProcessor(QObject *parent) : QObject(parent)
{

}

void ImageProcessor::posterize()
{
    if(m_imageProvider->contains("edges"))
        m_imageProvider->remove("edges");
    QImage filtered(m_currentImage.size(), QImage::Format_RGB888);
    QImage posterized(m_currentImage.size(), QImage::Format_RGB888);

    _medianFilter(m_currentImage, filtered, 3, 2);

    _averagingFilter(filtered, filtered, 3, 2);
    _posterize(filtered, posterized);
    _medianFilter(posterized, posterized, 3, 3);

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
    pixelsQueue.enqueue(_getImagePixels());

    QList<QList<QColor>> colorsList;
    int count = colorsCount;
    while(pixelsQueue.size() < count)
    {
      int queueSize = pixelsQueue.size();
      for(int i = 0; i < queueSize; ++i)
      {
        auto forSplit = pixelsQueue.dequeue();  // извлекаем из очереди
        auto splitted = _splitPixels(forSplit); // разбиваем
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
        QColor paletteColor = _averageColor(colors);
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
    _fill(x, y, img, fillColor);
    m_imageProvider->add("posterized", img);
}

void ImageProcessor::edges()
{
    if(m_imageProvider->contains("edges"))
        return;
    QImage posterized = m_imageProvider->get("posterized");

    // удаление тонких линий и одиночных пикселей

//    for(int y = 1; y < posterized.height() - 1; ++y)
//      for(int x = 1; x < posterized.width() - 1; ++x)
//      {
//        QColor current = posterized.pixelColor(x, y);
//        QColor top = posterized.pixelColor(x, y - 1);
//        QColor bottom = posterized.pixelColor(x, y + 1);
//        QColor left = posterized.pixelColor(x - 1, y);
//        QColor right = posterized.pixelColor(x + 1, y);

//        if(current != left && current != right)
//          posterized.setPixelColor(x, y, left);
//        else if(current != top && current != bottom)
//          posterized.setPixelColor(x, y, top);
//      }

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

void ImageProcessor::removeImageFromProvider(const QString &id)
{
    m_imageProvider->remove(id);
}

void ImageProcessor::_posterize(const QImage& sourceImage, QImage& result)
{
    for(int y = 0; y < sourceImage.height(); ++y)
        for(int x = 0; x < sourceImage.width(); ++x)
        {
            QColor srcPixColor = sourceImage.pixelColor(x, y);
            QColor posterizedPixColor = m_colors[_findNearestColorIndex(srcPixColor)];
            result.setPixelColor(x, y, posterizedPixColor);
        }
}

void ImageProcessor::_medianFilter(const QImage &sourceImage, QImage &result, int maskSize, int iterations)
{
  if(maskSize % 2 == 0)
  {
    qDebug() << "Некорректный размер маски медианного фильтра: " << maskSize;
    return;
  }
  QImage inputImage{sourceImage}; // на каждой итерации применяется фильтр
  for(int it = 0; it < iterations; ++it)
  {
    for(int y = 0; y < inputImage.height(); ++y)
      for(int x = 0; x < inputImage.width(); ++x)
      {
        QList<QPair<double,QColor>> maskValues;
        int xFrom = x - maskSize / 2;
        int xTo = x + maskSize / 2;
        int yFrom = y - maskSize / 2;
        int yTo = y + maskSize / 2;
        for(int yMask = yFrom; yMask <= yTo; ++yMask)
          for(int xMask = xFrom; xMask <= xTo; ++xMask)
          {
            double value = 0.0;
            QColor color;
            if(yMask < 0 || xMask < 0 ||
               yMask >= inputImage.height() || xMask >= inputImage.width())
            {
              color = Qt::black;
            }
            else
            {
              color = inputImage.pixelColor(xMask, yMask);
              value = 0.2126 * color.red() + 0.7152 * color.green() + 0.0722 * color.blue();
              //value = color.lightness();
              // return 0.2126 * dr * dr + 0.7152 * dg * dg + 0.0722 * db * db;
            }
            maskValues << QPair<double,QColor>{value, color};
          }
        std::sort(maskValues.begin(), maskValues.end(),
                  [](const QPair<double,QColor>& a, const QPair<double,QColor>& b)
        {
            return b.first > a.first;
        });
        int medIndex = maskValues.size() / 2;
        QColor medPixColor = maskValues[medIndex].second;
        result.setPixelColor(x, y, medPixColor);
      }
    inputImage = result;
  }
}

void ImageProcessor::_averagingFilter(const QImage &sourceImage, QImage &result, int maskSize, int iterations)
{
    if(maskSize % 2 == 0)
    {
      qDebug() << "Некорректный размер маски усредняющего фильтра: " << maskSize;
      return;
    }
    QImage inputImage{sourceImage}; // на каждой итерации применяется фильтр
    int radius = maskSize / 2; // удаленность границ от центра
    int maskCount = maskSize * maskSize; // количество пикселей в маске
    for(int it = 0; it < iterations; ++it)
    {
      for(int y = 0; y < inputImage.height(); ++y)
        for(int x = 0; x < inputImage.width(); ++x)
        {
          int xFrom = x - radius;
          int xTo = x + radius;
          int yFrom = y - radius;
          int yTo = y + radius;
          int rSum = 0;
          int gSum = 0;
          int bSum = 0;
          for(int yMask = yFrom; yMask <= yTo; ++yMask)
            for(int xMask = xFrom; xMask <= xTo; ++xMask)
            {
              QColor pixColor;
              if(yMask < 0 || xMask < 0 ||
                 yMask >= inputImage.height() || xMask >= inputImage.width())
              {
                pixColor = inputImage.pixelColor(x, y); // центр
              }
              else
                pixColor = inputImage.pixelColor(xMask, yMask);

              rSum += pixColor.red();
              gSum += pixColor.green();
              bSum += pixColor.blue();
            }
          int r = qRound(static_cast<float>(rSum) / maskCount);
          int g = qRound(static_cast<float>(gSum) / maskCount);
          int b = qRound(static_cast<float>(bSum) / maskCount);

          result.setPixelColor(x, y, QColor(r, g, b));
        }
      inputImage = result;
    }
}

QRect ImageProcessor::_fill(int x, int y, QImage &image, const QColor &fillColor)
{
    QColor pixColor = image.pixelColor(x, y);
    if(pixColor == fillColor)
        return QRect();
    int minX, maxX, minY, maxY;
    minX = maxX = x;
    minY = maxY = y;

    QQueue<QPoint> queue;
    queue.enqueue(QPoint{x, y});
    while(!queue.isEmpty())
    {
       auto pix = queue.dequeue();
       if(image.pixelColor(pix.x(), pix.y()) == pixColor)
       {
           auto west{pix};
           auto east{pix};
           // смещение влево, пока цвет west равен pixColor и пока west не вышел за границы
           while(west.x() >= 0 && image.pixelColor(west.x(), west.y()) == pixColor)
               west.setX(west.x() - 1);
           // смещение вправо, пока цвет east равен pixColor и пока east не вышел за границы
           while(east.x() < image.width() && image.pixelColor(east.x(), east.y()) == pixColor)
               east.setX(east.x() + 1);

           // проход по пикселям между west и east (не включая их)
           for(int i = west.x() + 1; i < east.x(); ++i)
           {
               // установка цвета пикселя fillColor
               image.setPixelColor(i, pix.y(), fillColor);
               // проверка пикселя сверху: если цвет == pixColor, то добавляем в очередь
               if(pix.y() > 0)
               {
                   QPoint north{i, pix.y() - 1};
                   if(image.pixelColor(north.x(), north.y()) == pixColor)
                       queue.enqueue(north);
                   minY = std::min(minY, north.y());
               }
               // проверка пикселя снизу: если цвет == pixColor, то добавляем в очередь
               if(pix.y() < image.height() - 1)
               {
                   QPoint south{i, pix.y() + 1};
                   if(image.pixelColor(south.x(), south.y()) == pixColor)
                       queue.enqueue(south);
                   maxY = std::max(maxY, south.y());
               }
           }
           minX = std::min(minX, west.x() + 1);
           maxX = std::max(maxX, east.x() - 1);
       }
    }
    return QRect{QPoint{minX, minY}, QPoint{maxX, maxY}};
}



int ImageProcessor::_findNearestColorIndex(const QColor &color)
{
    int minDistanceIndex = 0;
    double minDistance = _colorDistance(color, 0);
    for(int i = 1; i < m_colors.size(); ++i)
    {
        double distance = _colorDistance(color, i);
        if(distance < minDistance)
        {
            minDistance = distance;
            minDistanceIndex = i;
        }
    }
    return minDistanceIndex;
}

double ImageProcessor::_colorDistanceLab(const QColor &color, int index)
{
    double l1, l2, a1, a2, b1, b2;
    _rgb2Lab(color, l1, a1, b1);
    _rgb2Lab(m_colors[index], l2, a2, b2);
    double dL = l1 - l2;
    double dA = a1 - a2;
    double dB = b1 - b2;
    return dL * dL + dA * dA + dB * dB;
}

double ImageProcessor::_colorDistance(const QColor &color, int index)
{
    int dr = color.red() - m_colors[index].red();
    int dg = color.green() - m_colors[index].green();
    int db = color.blue() - m_colors[index].blue();
    return 0.2126 * dr * dr + 0.7152 * dg * dg + 0.0722 * db * db;
    //return 30 * dr * dr + 59 * dg * dg + 11 * db * db;
}

void ImageProcessor::_rgb2xyz(const QColor &rgbColor, double &x, double &y, double &z)
{
    /*
     * |X| = |0.49    0.31   0.2    |   |R|
     * |Y| = |0.17697 0.8124 0.01063| x |G|
     * |Z| = |0.0     0.01   0.99   |   |B|
     */

    double r = rgbColor.red();
    double g = rgbColor.green();
    double b = rgbColor.blue();

    x = r * 0.49 + g * 0.31 + b * 0.2;
    y = r * 0.17697 + g * 0.8124 + b * 0.01063;
    z = g * 0.01 + b * 0.99;
}

void ImageProcessor::_xyz2Lab(double x, double y, double z, double &l, double &a, double &b)
{
    // в d65: 0.9504 1.0000 1.0888
    const double xN = 0.9642;
    const double yN = 1.0;
    const double zN = 0.8249;

    double f_x = _f(x / xN);
    double f_y = _f(y / yN);
    double f_z = _f(z / zN);

    l = 116 * f_y - 16;
    a = 500 * (f_x - f_y);
    b = 200 * (f_y - f_z);
}

void ImageProcessor::_rgb2Lab(const QColor &rgbColor, double &l, double &a, double &b)
{
    double x, y, z;
    _rgb2xyz(rgbColor, x, y, z);
    _xyz2Lab(x, y, z, l, a, b);
}

double ImageProcessor::_f(double t)
{
    constexpr double t0 = (6.0 / 29) * (6.0 / 29) * (6.0 / 29);
    constexpr double a = (29.0 / 6) * (29.0 / 6) / 3;
    constexpr double b = 4.0 / 29;
    constexpr double _1_3 = 1.0 / 3;
    if(t > t0) return pow(t, _1_3);
    return a * t + b;
}

QList<QColor> ImageProcessor::_getUniqueColors()
{
    QList<QColor> uniqueColors;
    for(int y = 0; y < m_currentImage.height(); ++y)
      for(int x = 0; x < m_currentImage.width(); ++x)
      {
        QColor color = m_currentImage.pixelColor(x, y);
        if(!uniqueColors.contains(color))
          uniqueColors << color;
      }
    qDebug() << "Число уникальных цветов: " << uniqueColors.size();
    return uniqueColors;
}



QPair<QList<QColor>, QList<QColor> > ImageProcessor::_splitPixels(const QList<QColor> &pixels)
{
    if(pixels.size() < 2)
      return {QList<QColor>(), QList<QColor>()};


    int rMax, rMin, gMax, gMin, bMax, bMin, dr, dg, db, maxDiff;
    QList<QColor> leftPixels;
    QList<QColor> rightPixels;
    rMax = rMin = pixels[0].red();
    gMax = gMin = pixels[0].green();
    bMax = bMin = pixels[0].blue();
    for(int i = 1; i < pixels.size(); ++i)
    {
        int r = pixels[i].red();
        int g = pixels[i].green();
        int b = pixels[i].blue();
        if(r < rMin) rMin = r;
        else if(r > rMax) rMax = r;
        if(g < gMin) gMin = g;
        else if(g > gMax) gMax = g;
        if(b < bMin) bMin = b;
        else if(b > bMax) bMax = b;
    }
    dr = rMax - rMin;
    dg = gMax - gMin;
    db = bMax - bMin;
    maxDiff = dr;
    if(dg > maxDiff) maxDiff = dg;
    if(db > maxDiff) maxDiff = db;
    if(maxDiff == dr)
    {
        int rAvg = rMin + dr / 2;
        for(int i = 0; i < pixels.size(); ++i)
        {
            int r = pixels[i].red();
            if(r < rAvg)
                leftPixels << pixels[i];
            else
                rightPixels << pixels[i];
        }
    }
    else if(maxDiff == dg)
    {
        int gAvg = gMin + dg / 2;
        for(int i = 0; i < pixels.size(); ++i)
        {
            int g = pixels[i].green();
            if(g < gAvg)
                leftPixels << pixels[i];
            else
                rightPixels << pixels[i];
        }
    }
    else if(maxDiff == db)
    {
        int bAvg = bMin + db / 2;
        for(int i = 0; i < pixels.size(); ++i)
        {
            int b = pixels[i].blue();
            if(b < bAvg)
                leftPixels << pixels[i];
            else
                rightPixels << pixels[i];
        }
    }
    return {leftPixels, rightPixels};
}

QList<QColor> ImageProcessor::_getImagePixels()
{
    QList<QColor> pixels;
    for(int y = 0; y < m_currentImage.height(); ++y)
      for(int x = 0; x < m_currentImage.width(); ++x)
        pixels << m_currentImage.pixelColor(x, y);
    return pixels;
}

QColor ImageProcessor::_averageColor(const QList<QColor> &colors)
{
    if(colors.isEmpty())
    {
        qDebug() << "_averageColor: пустой список цветов";
        return Qt::black;
    }
    uint64_t rSum = 0;
    uint64_t gSum = 0;
    uint64_t bSum = 0;
    for(const auto &col: colors)
    {
        rSum += col.red();
        gSum += col.green();
        bSum += col.blue();
    }
    int r = qRound(static_cast<double>(rSum) / colors.size());
    int g = qRound(static_cast<double>(gSum) / colors.size());
    int b = qRound(static_cast<double>(bSum) / colors.size());

    return QColor(r, g, b);
}
