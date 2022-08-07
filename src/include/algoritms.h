#ifndef ALGORITMS_H
#define ALGORITMS_H
#include <QImage>
#include "datatypes.h"

namespace Algorithms
{
    void posterize(const QImage& sourceImage, QImage& result,
                   const QList<QColor>& colors);
    int findNearestColorIndex(const QColor &color, const QList<QColor>& colors);
    double colorDistance(const QColor& color1, const QColor& color2);
    double colorDistanceLab(const QColor& color1, const QColor& color2);
    void rgb2xyz(const QColor& rgbColor, double &x, double& y, double& z);
    void xyz2Lab(double x, double y, double z, double& l, double& a, double& b);
    void rgb2Lab(const QColor& rgbColor, double &l, double &a, double &b);
    double fLab(double t);
    void medianFilter(const QImage& sourceImage, QImage& result,
                      int maskSize = 3, int iterations = 1);
    void averagingFilter(const QImage& sourceImage, QImage& result,
                         int maskSize = 3, int iterations = 1);
    void fill(int x, int y, QImage& image, const QColor& fillColor,
              QList<QPoint> *pixels = nullptr);
    QList<QColor> getUniqueColors(const QImage& image);
    QList<QPoint> findContour(const QImage& image, const QList<QPoint>& pixels);
    QPair<QList<QColor>,QList<QColor>> splitPixels(const QList<QColor>& pixels);
    QList<QColor> getImagePixels(const QImage& image);
    QColor averageColor(const QList<QColor>& colors);
    DataTypes::PointsMatrix makePointsMatrix(const QList<QPoint>& points);
    DataTypes::PointsMatrix makeScaledPointsMatrix(const QList<QPoint>& points,
                                                   uint8_t scale);
    QPoint findTextPosition(const DataTypes::PointsMatrix& matrix, int textWidth,
                            int textHeight);
};

#endif // ALGORITMS_H
