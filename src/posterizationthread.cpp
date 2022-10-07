#include "include/posterizationthread.h"
#include "include/algorithms.h"
#include <QDebug>

PosterizationThread::PosterizationThread(QObject *parent):
    QThread(parent)
{
}

void PosterizationThread::run()
{
    if(!m_filtered)
    {
        m_filteredImage = QImage(m_image.size(), m_image.format());
        emit progress("Подготовка: медианная фильтрация");
        Algorithms::medianFilter(m_image, m_filteredImage, 3, 2);
        emit progress("Подготовка: усредняюшая фильтрация");
        Algorithms::averagingFilter(m_filteredImage, m_filteredImage, 3, 2);
    }
    emit progress("Идёт постеризация");

    m_posterizedImage = QImage(m_filteredImage.size(), m_filteredImage.format());
    Algorithms::posterize(m_filteredImage, m_posterizedImage, m_palette);

    emit progress("Завершение: медианная фильтрация");
    Algorithms::medianFilter(m_posterizedImage, m_posterizedImage, 3, 3);

    emit finished();
    //    if(m_imageProvider->contains("filtered"))
    //        filtered = m_imageProvider->get("filtered");
    //    else
    //    {
    //        Algorithms::medianFilter(m_currentImage, filtered, 3, 2);
    //        Algorithms::averagingFilter(filtered, filtered, 3, 2);
    //        m_imageProvider->add("filtered", filtered);
    //    }
    //    Algorithms::posterize(filtered, posterized, m_colors);
    //    Algorithms::medianFilter(posterized, posterized, 3, 3);

    //    m_imageProvider->add("posterized", posterized);
}

void PosterizationThread::set(bool filtered, const QImage &image,
                              const QList<QColor> &palette)
{
    m_filtered = filtered;
    if(filtered)
        m_filteredImage = image;
    else
        m_image = image;
    m_palette = palette;
}
