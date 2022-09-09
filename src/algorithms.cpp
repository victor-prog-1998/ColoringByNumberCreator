#include "include/algorithms.h"
#include <cmath>
#include <omp.h>
#include <QDebug>
#include <QQueue>
#include <QDateTime>

namespace Algorithms
{

double colorDistance(const QColor &color1, const QColor &color2)
{
    int dr = color1.red() - color2.red();
    int dg = color1.green() - color2.green();
    int db = color1.blue() - color2.blue();
    return 0.2126 * dr * dr + 0.7152 * dg * dg + 0.0722 * db * db;
}

int findNearestColorIndex(const QColor &color, const QList<QColor> &colors)
{
    int minDistanceIndex = 0;
    double minDistance = colorDistance(color, colors[0]);
    for(int i = 1; i < colors.size(); ++i)
    {
        double distance = colorDistance(color, colors[i]);
        if(distance < minDistance)
        {
            minDistance = distance;
            minDistanceIndex = i;
        }
    }
    return minDistanceIndex;
}

void posterize(const QImage &sourceImage, QImage &result,
               const QList<QColor> &colors)
{
#pragma omp parallel
    {
#pragma omp for
        for(int y = 0; y < sourceImage.height(); ++y)
          for(int x = 0; x < sourceImage.width(); ++x)
          {
            QColor srcPixColor = sourceImage.pixelColor(x, y);
            QColor posterizedPixColor =
                    colors[findNearestColorIndex(srcPixColor, colors)];
            //#pragma omp critical
            {
            result.setPixelColor(x, y, posterizedPixColor);
            }
          }
    }
}

void rgb2xyz(const QColor &rgbColor, double &x, double &y, double &z)
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

double fLab(double t)
{
    constexpr double t0 = (6.0 / 29) * (6.0 / 29) * (6.0 / 29);
    constexpr double a = (29.0 / 6) * (29.0 / 6) / 3;
    constexpr double b = 4.0 / 29;
    constexpr double _1_3 = 1.0 / 3;
    if(t > t0) return pow(t, _1_3);
    return a * t + b;
}

void xyz2Lab(double x, double y, double z, double &l, double &a, double &b)
{
    // в d65: 0.9504 1.0000 1.0888
    const double xN = 0.9642;
    const double yN = 1.0;
    const double zN = 0.8249;

    double f_x = fLab(x / xN);
    double f_y = fLab(y / yN);
    double f_z = fLab(z / zN);

    l = 116 * f_y - 16;
    a = 500 * (f_x - f_y);
    b = 200 * (f_y - f_z);
}

void rgb2Lab(const QColor &rgbColor, double &l, double &a, double &b)
{
    double x, y, z;
    rgb2xyz(rgbColor, x, y, z);
    xyz2Lab(x, y, z, l, a, b);
}

double colorDistanceLab(const QColor &color1, const QColor &color2)
{
    double l1, l2, a1, a2, b1, b2;
    rgb2Lab(color1, l1, a1, b1);
    rgb2Lab(color2, l2, a2, b2);
    double dL = l1 - l2;
    double dA = a1 - a2;
    double dB = b1 - b2;
    return dL * dL + dA * dA + dB * dB;
}

void medianFilter(const QImage &sourceImage, QImage &result,
                  int maskSize, int iterations)
{
  if(maskSize % 2 == 0)
  {
    qDebug() << "Некорректный размер маски медианного фильтра: " << maskSize;
    return;
  }
  int maskRadius = maskSize / 2;
  int medIndex = maskSize * maskSize / 2;

  QImage inputImage{sourceImage}; // на каждой итерации применяется фильтр
  for(int it = 0; it < iterations; ++it)
  {
    #pragma omp parallel
    {
    #pragma omp for
    for(int y = 0; y < inputImage.height(); ++y)
      for(int x = 0; x < inputImage.width(); ++x)
      {
        QList<QPair<double,QColor>> maskValues;
        int xFrom = x - maskRadius;
        int xTo = x + maskRadius;
        int yFrom = y - maskRadius;
        int yTo = y + maskRadius;
        {
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
                value = 0.2126 * color.red() + 0.7152 * color.green() +
                        0.0722 * color.blue();
              }
              maskValues << QPair<double,QColor>{value, color};
            }
        }

        std::sort(maskValues.begin(), maskValues.end(),
                  [](const QPair<double,QColor>& a,
                  const QPair<double,QColor>& b)
        {
            return b.first > a.first;
        });
        QColor medPixColor = maskValues[medIndex].second;
        #pragma omp critical
        {
          result.setPixelColor(x, y, medPixColor);
        }
      }
    }
    inputImage = result;
  }
}

void averagingFilter(const QImage &sourceImage, QImage &result,
                     int maskSize, int iterations)
{
    if(maskSize % 2 == 0)
    {
      qDebug() << "Некорректный размер маски "
                  "усредняющего фильтра: " << maskSize;
      return;
    }
    QImage inputImage{sourceImage}; // на каждой итерации применяется фильтр
    int radius = maskSize / 2; // удаленность границ от центра
    int maskCount = maskSize * maskSize; // количество пикселей в маске
    for(int it = 0; it < iterations; ++it)
    {
      // с OpenMP работает медленнее
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
}

void fill(int x, int y, QImage &image, const QColor &fillColor,
          QList<QPoint> *pixels)
{
    QColor pixColor = image.pixelColor(x, y);
    if(pixColor == fillColor)
        return;

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
           while(west.x() >= 0 &&
                 image.pixelColor(west.x(), west.y()) == pixColor)
               west.setX(west.x() - 1);
           // смещение вправо, пока цвет east равен pixColor и пока east не вышел за границы
           while(east.x() < image.width() &&
                 image.pixelColor(east.x(), east.y()) == pixColor)
               east.setX(east.x() + 1);

           // проход по пикселям между west и east (не включая их)
           for(int i = west.x() + 1; i < east.x(); ++i)
           {
               // установка цвета пикселя fillColor
               image.setPixelColor(i, pix.y(), fillColor);
               if(pixels) pixels->push_back(QPoint{i, pix.y()});
               // проверка пикселя сверху: если цвет == pixColor, то добавляем в очередь
               if(pix.y() > 0)
               {
                   QPoint north{i, pix.y() - 1};
                   if(image.pixelColor(north.x(), north.y()) == pixColor)
                       queue.enqueue(north);
               }
               // проверка пикселя снизу: если цвет == pixColor, то добавляем в очередь
               if(pix.y() < image.height() - 1)
               {
                   QPoint south{i, pix.y() + 1};
                   if(image.pixelColor(south.x(), south.y()) == pixColor)
                       queue.enqueue(south);
               }
           }
       }
    }
}

QList<QColor> getUniqueColors(const QImage &image)
{
    QList<QColor> uniqueColors;
    for(int y = 0; y < image.height(); ++y)
      for(int x = 0; x < image.width(); ++x)
      {
        QColor color = image.pixelColor(x, y);
        if(!uniqueColors.contains(color))
          uniqueColors << color;
      }
    return uniqueColors;
}

QList<QPoint> findContour(const QImage &image, const QList<QPoint> &pixels)
{
    // все предыдущие найденные области залиты черным, а текущая - зеленым
    // проверка выхода за пределы не нужна, так как в edges рисуется рамка
    // нужно проверять дубликаты найденных пикселей
    QList<QPoint> contourPixels;
    for(const auto& pix: pixels)
    {
        QPoint left  {pix.x() - 1, pix.y()    };
        QPoint right {pix.x() + 1, pix.y()    };
        QPoint top   {pix.x(),     pix.y() - 1};
        QPoint bottom{pix.x(),     pix.y() + 1};
        QColor leftColor  {image.pixelColor(left.x(),   left.y())};
        QColor rightColor {image.pixelColor(right.x(),  right.y())};
        QColor topColor   {image.pixelColor(top.x(),    top.y())};
        QColor bottomColor{image.pixelColor(bottom.x(), bottom.y())};

        if(leftColor == Qt::black && !contourPixels.contains(left))
            contourPixels << left;
        if(rightColor == Qt::black && !contourPixels.contains(right))
            contourPixels << right;
        if(topColor == Qt::black && !contourPixels.contains(top))
            contourPixels << top;
        if(bottomColor == Qt::black && !contourPixels.contains(bottom))
            contourPixels << bottom;
    }
    return contourPixels;
}



QPair<QList<QColor>, QList<QColor> > splitPixels(const QList<QColor> &pixels)
{
    if(pixels.size() < 2)
      return {QList<QColor>(), QList<QColor>()};

    uint64_t redSum {0};
    uint64_t greenSum {0};
    uint64_t blueSum {0};

#pragma omp parallel reduction (+: redSum) reduction (+: greenSum) \
                     reduction (+: blueSum)
    {
#pragma omp for
    for(int i = 0; i < pixels.size(); ++i)
    {
        redSum += pixels[i].red();
        greenSum += pixels[i].green();
        blueSum += pixels[i].blue();
    }
    }

    int redAvg = qRound(static_cast<double>(redSum) / pixels.size());
    int greenAvg = qRound(static_cast<double>(greenSum) / pixels.size());
    int blueAvg = qRound(static_cast<double>(blueSum) / pixels.size());

    int redMax, redMin, greenMax, greenMin, blueMax, blueMin;
    uint64_t redDisp, greenDisp, blueDisp;
    redMax = greenMax = blueMax = 0;
    redMin = greenMin = blueMin = 255;
    redDisp = greenDisp = blueDisp = 0;

    for(int i = 0; i < pixels.size(); ++i)
    {
        redDisp += std::abs(pixels[i].red() - redAvg);
        greenDisp += std::abs(pixels[i].green() - greenAvg);
        blueDisp += std::abs(pixels[i].blue() - blueAvg);

        redMin = std::min(redMin, pixels[i].red());
        greenMin = std::min(greenMin, pixels[i].green());
        blueMin = std::min(blueMin, pixels[i].blue());
        redMax = std::max(redMax, pixels[i].red());
        greenMax = std::max(greenMax, pixels[i].green());
        blueMax = std::max(blueMax, pixels[i].blue());
    }

    uint64_t maxDisp;
    maxDisp = std::max(redDisp, std::max(greenDisp, blueDisp));

    QList<QColor> leftPixels;
    QList<QColor> rightPixels;

    for(const auto& pix: pixels)
    {
       if(maxDisp == redDisp)
       {
           if(pix.red() < redAvg)
               leftPixels << pix;
           else
               rightPixels << pix;
       }
       else if(maxDisp == greenDisp)
       {
           if(pix.green() < greenAvg)
               leftPixels << pix;
           else
               rightPixels << pix;
       }
       else
       {
           if(pix.blue() < blueAvg)
               leftPixels << pix;
           else
               rightPixels << pix;
       }
    }

    return {leftPixels, rightPixels};
}

QList<QColor> getImagePixels(const QImage& image)
{
    QList<QColor> pixels;
    for(int y = 0; y < image.height(); ++y)
      for(int x = 0; x < image.width(); ++x)
        pixels << image.pixelColor(x, y);
    return pixels;
}

QColor averageColor(const QList<QColor> &colors)
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

DataTypes::PointsMatrix makePointsMatrix(const QList<QPoint> &points)
{
    int minX, maxX, minY, maxY;
    minX = maxX = points[0].x();
    minY = maxY = points[0].y();
    for(int i = 1; i < points.size(); ++i)
    {
      int x = points[i].x();
      int y = points[i].y();
      minX = std::min(minX, x);
      minY = std::min(minY, y);
      maxX = std::max(maxX, x);
      maxY = std::max(maxY, y);
    }

    DataTypes::PointsMatrix matrix;
    matrix.x = minX;
    matrix.y = minY;

    int width = maxX - minX + 1;
    int height = maxY - minY + 1;

    matrix.values.resize(height);
    for(auto &row: matrix.values)
    {
      row.resize(width);
      for(auto &p: row)
        p = 0;
    }

    for(const auto& point: points)
    {
      int x = point.x() - minX;
      int y = point.y() - minY;
      matrix.values[y][x] = 1;
    }

    return matrix;
}

DataTypes::PointsMatrix makeScaledPointsMatrix(const QList<QPoint> &points,
                                               uint8_t scale)
{
    int minX, maxX, minY, maxY;
    minX = maxX = points[0].x();
    for(int i = 1; i < points.size(); ++i)
    {
      int x = points[i].x();
      int y = points[i].y();
      minX = std::min(minX, x);
      maxX = std::max(maxX, x);
      minY = std::min(minY, y);
      maxY = std::max(maxY, y);
    }
    DataTypes::PointsMatrix matrix;
    matrix.x = minX * scale;
    matrix.y = minY * scale;

    int width = scale * (maxX - minX + 1);
    int height = scale * (maxY - minY + 1);
    matrix.values.resize(height);
    for(auto &row: matrix.values)
    {
      row.resize(width);
      for(auto &p: row)
        p = 0;
    }

    for(const auto& point: points)
    {
      int x = (point.x() - minX) * scale;
      int y = (point.y() - minY) * scale;
      for(int i = 0; i < scale; ++i)
        for(int j = 0; j < scale; ++j)
        {
          matrix.values[y + i][x + j] = 1;
        }
    }

    return matrix;
}

QPoint findTextPosition(const DataTypes::PointsMatrix &matrix,
                        int textWidth, int textHeight)
{
    if(matrix.width() < textWidth || matrix.height() < textHeight)
        return QPoint{-1, -1};
    int textArea = textWidth * textHeight;
    for(int y = 0; y <= matrix.height() - textHeight; ++y)
      for(int x = 0; x <= matrix.width() - textWidth; ++x)
      {
        int counter = 0;
        for(int i = 0; i < textArea; ++i)
        {
          int px = x + i % textWidth;
          int py = y + i / textWidth;
          if(!matrix.values[py][px])
            break;
          ++counter;
        }
        if(counter == textArea)
            return QPoint{matrix.x + x, matrix.y + y};
      }
    return QPoint{-1, -1};
}

QColor generateNonExistingColor(const QList<QString>& existingColorsNames)
{
    QColor newColor;
    while(true)
    {
        // Генерируем цвет, не являющийся черно-белым
        int r = qrand() % 85;         // [0   -  84]
        int g = 85 + qrand() % 85;    // [85  - 169]
        int b = 170 + qrand() % 85;   // [170 - 254]
        newColor.setRgb(r, g, b);

        bool exists = false;
        for(const auto& colorName: existingColorsNames)
        {
            QColor color(colorName);
            if(newColor == color)
            {
                exists = true;
                break;
            }
        }
        if(!exists)
            break;
    }
    return newColor;
}

int getCountOfDigits(int number)
{
    int count = 1;
    while(number >= 10)
    {
        number /= 10;
        ++count;
    }
    return count;
}

QImage makeEdgesImage(const QImage &posterized)
{
    // нахождение краёв
    QImage edgesImage(posterized.size(), QImage::Format_RGB888);
    edgesImage.fill(Qt::white);
#pragma omp parallel
    {
#pragma omp for
    for(int y = 0; y < posterized.height() - 1; ++y)
      for(int x = 0; x < posterized.width() - 1; ++x)
      {
        QColor current = posterized.pixelColor(x, y);
        QColor right = posterized.pixelColor(x + 1, y);
        QColor bottom = posterized.pixelColor(x, y + 1);
        if(current != right || current != bottom)
        {
          #pragma omp critical
          {
          edgesImage.setPixelColor(x, y, Qt::black);
          }
        }
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
    return edgesImage;
}

void changeColor(QImage &image, int x, int y, const QColor &color)
{
    QColor srcColor = image.pixelColor(x, y);
    for(int y = 0; y < image.height(); ++y)
      for(int x = 0; x < image.width(); ++x)
        if(image.pixelColor(x, y) == srcColor)
            image.setPixelColor(x, y, color);
}

QStringList findOptimalPalette(const QImage &image, int colorsCount)
{
    QStringList palette;

    QQueue<QList<QColor>> pixelsQueue;
    pixelsQueue.enqueue(Algorithms::getImagePixels(image));

    QList<QList<QColor>> colorsList;
    int count = colorsCount;
    while(pixelsQueue.size() < count)
    {
      int queueSize = pixelsQueue.size();
      for(int i = 0; i < queueSize; ++i)
      {
        auto forSplit = pixelsQueue.dequeue();  // извлекаем из очереди
        auto splitted = Algorithms::splitPixels(forSplit); // разбиваем
        // не разбивается => записываем в список и больше не рассматриваем
        if(splitted.first.isEmpty() || splitted.second.isEmpty())
        {
          colorsList << forSplit;
          --count; // так как в очереди будет меньше элементов
        }
        else
        {
          // добавляем в конец очереди оба => стало на 1 больше, чем до извлечения
          pixelsQueue.enqueue(splitted.first);
          pixelsQueue.enqueue(splitted.second);
          if(pixelsQueue.size() == count)
            break;
        }
      }
    }

    while(!pixelsQueue.isEmpty())
      colorsList << pixelsQueue.dequeue();

    #pragma omp parallel
    {
    #pragma omp for
    for(int i = 0; i < colorsList.size(); ++i)
    {
        QColor paletteColor = Algorithms::averageColor(colorsList[i]);
        #pragma omp critical
        {
        palette << paletteColor.name(QColor::HexRgb);
        }
    }
    }

    return palette;
}

void findAreas(const QImage &posterized, const QImage &edges,
               QList<DataTypes::Area> &areas, QMap<QString, int> &colorsMap)
{
    QImage edgesCopy(edges);
    for(int y = 0; y < edgesCopy.height(); ++y)
      for(int x = 0; x < edgesCopy.width(); ++x)
      {
        QColor pixColor = edgesCopy.pixelColor(x, y);
        if(pixColor == Qt::white)
        {
          QColor posterizedColor = posterized.pixelColor(x, y);
          QString colorName = posterizedColor.name(QColor::HexRgb);
          if(!colorsMap.contains(colorName))
          {
            int id = colorsMap.size() + 1;
            colorsMap[colorName] = id;
          }
          QList<QPoint> filledPixels;
          Algorithms::fill(x, y, edgesCopy, Qt::green, &filledPixels);
          auto contourPixels{Algorithms::findContour(edgesCopy, filledPixels)};
          DataTypes::Area area{contourPixels, filledPixels,
                               colorsMap[colorName]};
          areas << area;
        }
      }
}

QImage scaleImage2x(const QImage &image, int iterations)
{
    QImage srcImage(image);
    for(int it = 0; it < iterations; ++it)
    {
        QImage tmpImage = srcImage.scaled(srcImage.width() * 2,
                                          srcImage.height() * 2);

        int bottomY = srcImage.height() - 1;
        int rightX = srcImage.width() - 1;

        // с OpenMP работает медленнее
        for(int x = 1; x < rightX; ++x)
        {
          int pixX = x * 2;
          for(int y = 1; y < bottomY; ++y)
          {
            QColor c1, c2, c3, c4, a, b, c, d;
            c1 = c2 = c3 = c4 = srcImage.pixelColor(x, y);
            a = srcImage.pixelColor(x, y - 1);
            b = srcImage.pixelColor(x + 1, y);
            c = srcImage.pixelColor(x - 1, y);
            d = srcImage.pixelColor(x, y + 1);

            if(c==a && c!=d && a!=b) c1 = a;
            if(a==b && a!=c && b!=d) c2 = b;
            if(d==c && d!=b && c!=a) c3 = c;
            if(b==d && b!=a && d!=c) c4 = d;

            int pixY = y * 2;

            tmpImage.setPixelColor(pixX,     pixY,     c1);
            tmpImage.setPixelColor(pixX + 1, pixY,     c2);
            tmpImage.setPixelColor(pixX,     pixY + 1, c3);
            tmpImage.setPixelColor(pixX + 1, pixY + 1, c4);
          }
        }
        srcImage = tmpImage;
    }

    return srcImage;
}

QImage scaleImage3x(const QImage &image, int iterations)
{
    QImage srcImage(image);
    for(int it = 0; it < iterations; ++it)
    {
        QImage tmpImage = srcImage.scaled(srcImage.width() * 3,
                                          srcImage.height() * 3);

        int bottomY = srcImage.height() - 1;
        int rightX = srcImage.width() - 1;

        // с OpenMP работает медленнее
        for(int x = 1; x < rightX; ++x)
        {
          int pixX = x * 3;
          for(int y = 1; y < bottomY; ++y)
          {
            QColor c1, c2, c3, c4, c5, c6, c7, c8, c9;
            QColor a, b, c, d, e, f, g, h, i;
            c1 = c2 = c3 = c4 = c5 = c6 = c7 = c8 = c9 = e =
                    srcImage.pixelColor(x, y);

            a = srcImage.pixelColor(x - 1, y - 1);
            b = srcImage.pixelColor(x   ,  y - 1);
            c = srcImage.pixelColor(x + 1, y - 1);
            d = srcImage.pixelColor(x - 1, y);
            f = srcImage.pixelColor(x + 1, y);
            g = srcImage.pixelColor(x - 1, y + 1);
            h = srcImage.pixelColor(x   ,  y + 1);
            i = srcImage.pixelColor(x + 1, y + 1);

            if(d==b && d!=h && b!=f) c1=d;
            if((d==b && d!=h && b!=f && e!=c) ||
                    (b==f && b!=d && f!=h && e!=a)) c2=b;
            if(b==f && b!=d && f!=h) c3=f;
            if((h==d && h!=f && d!=b && e!=a) ||
                    (d==b && d!=h && b!=f && e!=g)) c4=d;
            if((b==f && b!=d && f!=h && e!=i) ||
                    (f==h && f!=b && h!=d && e!=c)) c6=f;
            if(h==d && h!=f && d!=b) c7=d;
            if((f==h && f!=b && h!=d && e!=g) ||
                    (h==d && h!=f && d!=b && e!=i)) c8=h;
            if(f==h && f!=b && h!=d) c9=f;

            int pixY = y * 3;

            tmpImage.setPixelColor(pixX,     pixY,     c1);
            tmpImage.setPixelColor(pixX + 1, pixY,     c2);
            tmpImage.setPixelColor(pixX + 2, pixY,     c3);
            tmpImage.setPixelColor(pixX,     pixY + 1, c4);
            tmpImage.setPixelColor(pixX + 1, pixY + 1, c5);
            tmpImage.setPixelColor(pixX + 2, pixY + 1, c6);
            tmpImage.setPixelColor(pixX,     pixY + 2, c7);
            tmpImage.setPixelColor(pixX + 1, pixY + 2, c8);
            tmpImage.setPixelColor(pixX + 2, pixY + 2, c9);
          }
        }

        srcImage = tmpImage;
    }

    return srcImage;
}
}
