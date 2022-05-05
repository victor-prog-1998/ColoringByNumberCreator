#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include "imageprovider.h"

class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = nullptr);
    void setImageProvider(ImageProvider* provider) {m_imageProvider = provider;}
    Q_INVOKABLE void posterize();
    Q_INVOKABLE bool setCurrentImage(const QString& source);
    Q_INVOKABLE void setColors(const QStringList& colors);
    Q_INVOKABLE QStringList findOptimalPalette(int colorsCount);
    Q_INVOKABLE void changeColor(int x, int y, const QColor& color);
    Q_INVOKABLE void setPixelColor(int x, int y, const QColor& color);
    Q_INVOKABLE void fill(int x, int y, const QColor& fillColor);
    Q_INVOKABLE void edges();
    Q_INVOKABLE void removeImageFromProvider(const QString& id);
private:
    QImage m_currentImage;
    ImageProvider *m_imageProvider;
    QList<QColor> m_colors;
    void _posterize(const QImage& sourceImage, QImage& result);
    void _medianFilter(const QImage& sourceImage, QImage& result, int maskSize = 3, int iterations = 1);
    void _averagingFilter(const QImage& sourceImage, QImage& result, int maskSize = 3, int iterations = 1);
    QRect _fill(int x, int y, QImage& image, const QColor& fillColor);
    int _findNearestColorIndex(const QColor& color);
    double _colorDistanceLab(const QColor& color, int index);
    double _colorDistance(const QColor& color, int index);
    void _rgb2xyz(const QColor& rgbColor, double &x, double& y, double& z);
    void _xyz2Lab(double x, double y, double z, double& l, double& a, double& b);
    void _rgb2Lab(const QColor& rgbColor, double &l, double &a, double &b);
    double _f(double t);
    QList<QColor> _getUniqueColors();

    QPair<QList<QColor>,QList<QColor>> _splitPixels(const QList<QColor>& pixels);
    QList<QColor> _getImagePixels();
    QColor _averageColor(const QList<QColor>& colors);

signals:

};

#endif // IMAGEPROCESSOR_H
