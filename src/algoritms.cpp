#include "include/algoritms.h"
#include "math.h"
#include <QDebug>
#include <QQueue>


void Algoritms::posterize(const QImage &sourceImage, QImage &result, const QList<QColor> &colors)
{
    for(int y = 0; y < sourceImage.height(); ++y)
      for(int x = 0; x < sourceImage.width(); ++x)
      {
        QColor srcPixColor = sourceImage.pixelColor(x, y);
        QColor posterizedPixColor = colors[Algoritms::findNearestColorIndex(srcPixColor, colors)];
        result.setPixelColor(x, y, posterizedPixColor);
      }
}

int Algoritms::findNearestColorIndex(const QColor &color, const QList<QColor> &colors)
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

double Algoritms::colorDistance(const QColor &color1, const QColor &color2)
{
    int dr = color1.red() - color2.red();
    int dg = color1.green() - color2.green();
    int db = color1.blue() - color2.blue();
    return 0.2126 * dr * dr + 0.7152 * dg * dg + 0.0722 * db * db;
    //return 30 * dr * dr + 59 * dg * dg + 11 * db * db;
}

double Algoritms::colorDistanceLab(const QColor &color1, const QColor &color2)
{
    double l1, l2, a1, a2, b1, b2;
    rgb2Lab(color1, l1, a1, b1);
    rgb2Lab(color2, l2, a2, b2);
    double dL = l1 - l2;
    double dA = a1 - a2;
    double dB = b1 - b2;
    return dL * dL + dA * dA + dB * dB;
}

void Algoritms::rgb2xyz(const QColor &rgbColor, double &x, double &y, double &z)
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

void Algoritms::xyz2Lab(double x, double y, double z, double &l, double &a, double &b)
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

void Algoritms::rgb2Lab(const QColor &rgbColor, double &l, double &a, double &b)
{
    double x, y, z;
    rgb2xyz(rgbColor, x, y, z);
    xyz2Lab(x, y, z, l, a, b);
}

double Algoritms::fLab(double t)
{
    constexpr double t0 = (6.0 / 29) * (6.0 / 29) * (6.0 / 29);
    constexpr double a = (29.0 / 6) * (29.0 / 6) / 3;
    constexpr double b = 4.0 / 29;
    constexpr double _1_3 = 1.0 / 3;
    if(t > t0) return pow(t, _1_3);
    return a * t + b;
}

void Algoritms::medianFilter(const QImage &sourceImage, QImage &result, int maskSize, int iterations)
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

void Algoritms::averagingFilter(const QImage &sourceImage, QImage &result, int maskSize, int iterations)
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

void Algoritms::fill(int x, int y, QImage &image, const QColor &fillColor, QList<QPoint> *pixels)
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

QList<QColor> Algoritms::getUniqueColors(const QImage &image)
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

QList<QPoint> Algoritms::findContour(const QImage &image, const QList<QPoint> &pixels)
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



QPair<QList<QColor>, QList<QColor> > Algoritms::splitPixels(const QList<QColor> &pixels)
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

QList<QColor> Algoritms::getImagePixels(const QImage& image)
{
    QList<QColor> pixels;
    for(int y = 0; y < image.height(); ++y)
      for(int x = 0; x < image.width(); ++x)
        pixels << image.pixelColor(x, y);
    return pixels;
}

QColor Algoritms::averageColor(const QList<QColor> &colors)
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

PointsMatrix Algoritms::makePointsMatrix(const QList<QPoint> &points)
{
    int minX, maxX, minY, maxY;
    minX = maxX = points[0].x();
    minY = maxY = points[0].y();
    for(int i = 1; i < points.size(); ++i)
    {
      int x = points[i].x();
      int y = points[i].y();
      if(x < minX) minX = x;
      if(x > maxX) maxX = x;
      if(y < minY) minY = y;
      if(y > maxY) maxY = y;
    }

    PointsMatrix matrix;
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

PointsMatrix Algoritms::makeScaledPointsMatrix(const QList<QPoint> &points, uint8_t scale)
{
    int minX, maxX, minY, maxY;
    minX = maxX = points[0].x();
    for(int i = 1; i < points.size(); ++i)
    {
      int x = points[i].x();
      int y = points[i].y();
      if(x < minX) minX = x;
      if(x > maxX) maxX = x;
      if(y < minY) minY = y;
      if(y > maxY) maxY = y;
    }
    PointsMatrix matrix;
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

QPoint Algoritms::findTextPosition(const PointsMatrix &matrix, int textWidth, int textHeight)
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
