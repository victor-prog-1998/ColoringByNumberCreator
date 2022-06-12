#ifndef IMAGECREATOR_H
#define IMAGECREATOR_H
#include <QImage>
#include "algoritms.h"




class ImageCreator
{
public:
    ImageCreator();
    void createImages(const QImage& posterized, const QList<Area>& areas, const QMap<QString, int>& colorsMap);
    void createSimplifiedImages(const QImage& posterized, const QList<Area>& areas, const QMap<QString, int>& colorsMap);
    const QImage& getColoringImage() const {return m_coloringImage;}
    const QImage& getPaintedImage() const {return m_paintedImage;}
    const QImage& getLegendImage() const {return m_legend;}
    QList<QPair<QImage, QString>> tileColoringImage(int rows, int columns) const;
    void setColoringColor(const QColor& color);

private:
    int m_scaleFactor = 2;
    const int m_digitHeight = 9;
    const int m_digitWidth = 6;
    const int m_numberTextSpacing = -1;
    int _getNumberTextWidth(int number);
    int _getCountOfDigits(int number);
    void _drawContour(QImage& image, const QList<QPoint>& points);
    void _createLegend(const QMap<QString, int>& colorsMap);
    void _drawNumber(int x, int y, uint8_t number, QPainter &painter);
    QColor _generateNonExistingColor(const QList<QString>& existingColorsNames);
    void _makeDigits();

    QImage m_coloringImage;
    QImage m_paintedImage;
    QImage m_legend;
    QImage m_digits[10];
    QColor m_coloringColor{Qt::black};
};

#endif // IMAGECREATOR_H
