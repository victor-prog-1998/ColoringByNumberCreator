#include "include/imageprovider.h"
#include <QDebug>

ImageProvider::ImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage ImageProvider::requestImage(const QString &id, QSize *size,
                                   const QSize &requestedSize) /*override*/
{
    Q_UNUSED(size)
    Q_UNUSED(requestedSize)
    if(m_imagesMap.find(id) == m_imagesMap.end())
    {
        qDebug() << "ImageProviver: Нет изображения с id = " << id;
        return QImage();
    }
    return m_imagesMap[id];
}

void ImageProvider::add(const QString &id, const QImage &image)
{
    m_imagesMap[id] = image;
}
