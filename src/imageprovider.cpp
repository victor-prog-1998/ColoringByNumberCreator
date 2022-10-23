#include "include/imageprovider.h"
#include <QDebug>

ImageProvider::ImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{
  m_imageKeySources =
  {
    {Filtered, "filtered"},
    {Posterized, "posterized"},
    {ScaledPosterized, "scaled_posterized"},
    {Edges, "edges"},
    {Coloring, "coloring"},
    {Painted, "painted"},
    {Legend, "legend"}
  };
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

void ImageProvider::clear()
{
  m_imagesMap.clear();
}

void ImageProvider::add(const QString &id, const QImage &image)
{
  m_imagesMap[id] = image;
}

void ImageProvider::add(ImageProvider::ImageKey key, const QImage &image)
{
  m_imagesMap[m_imageKeySources[key]] = image;
}

int ImageProvider::size() const
{
  return m_imagesMap.size();
}

QImage ImageProvider::get(const QString &id) const
{
  return m_imagesMap[id];
}

QImage ImageProvider::get(ImageProvider::ImageKey key) const
{
  return m_imagesMap[m_imageKeySources[key]];
}

void ImageProvider::remove(const QString &key)
{
  m_imagesMap.remove(key);
}

void ImageProvider::remove(ImageProvider::ImageKey key)
{
  m_imagesMap.remove(m_imageKeySources[key]);
}

bool ImageProvider::contains(const QString &key) const
{
  return m_imagesMap.contains(key);
}

bool ImageProvider::contains(ImageProvider::ImageKey key) const
{
  return m_imagesMap.contains(m_imageKeySources[key]);
}
