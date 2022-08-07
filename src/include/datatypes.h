#ifndef DATATYPES_H
#define DATATYPES_H
#include <QList>
#include <QPoint>

namespace DataTypes
{
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
}

#endif // DATATYPES_H
