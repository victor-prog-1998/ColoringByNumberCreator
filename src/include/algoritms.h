#ifndef ALGORITMS_H
#define ALGORITMS_H
#include <QImage>

struct Area
{
    Area(const QList<QPoint>& contour, const QList<QPoint>& interior, int id):
        contourPoints(contour), interiorPoints(interior), colorId(id) {}
    QList<QPoint> contourPoints;
    QList<QPoint> interiorPoints;
    int colorId;
};

struct PointsMatrix
{
    QVector<QVector<uint8_t>> values;
    uint16_t x;
    uint16_t y;
    uint16_t height() const {return values.size();}
    uint16_t width() const {return values.isEmpty() ? 0 : values[0].size();};
};

class Algoritms
{
public:
    static void posterize(const QImage& sourceImage, QImage& result, const QList<QColor>& colors);
    static int findNearestColorIndex(const QColor &color, const QList<QColor>& colors);
    static double colorDistance(const QColor& color1, const QColor& color2);
    static double colorDistanceLab(const QColor& color1, const QColor& color2);
    static void rgb2xyz(const QColor& rgbColor, double &x, double& y, double& z);
    static void xyz2Lab(double x, double y, double z, double& l, double& a, double& b);
    static void rgb2Lab(const QColor& rgbColor, double &l, double &a, double &b);
    static double fLab(double t);
    static void medianFilter(const QImage& sourceImage, QImage& result, int maskSize = 3, int iterations = 1);
    static void averagingFilter(const QImage& sourceImage, QImage& result, int maskSize = 3, int iterations = 1);
    static void fill(int x, int y, QImage& image, const QColor& fillColor, QList<QPoint> *pixels = nullptr);
    static QList<QColor> getUniqueColors(const QImage& image);
    static QList<QPoint> findContour(const QImage& image, const QList<QPoint>& pixels);
    static QPair<QList<QColor>,QList<QColor>> splitPixels(const QList<QColor>& pixels);
    static QList<QColor> getImagePixels(const QImage& image);
    static QColor averageColor(const QList<QColor>& colors);
    static PointsMatrix makePointsMatrix(const QList<QPoint>& points);
    static PointsMatrix makeScaledPointsMatrix(const QList<QPoint>& points, uint8_t scale);
    static QPoint findTextPosition(const PointsMatrix& matrix, int textWidth, int textHeight);
};

#endif // ALGORITMS_H
