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
        emit progress("Подготовка: усредняющая фильтрация");
        Algorithms::averagingFilter(m_filteredImage, m_filteredImage, 3, 2);
    }
    emit progress("Идёт постеризация");

    m_posterizedImage = QImage(m_filteredImage.size(), m_filteredImage.format());
    Algorithms::posterize(m_filteredImage, m_posterizedImage, m_palette);

    emit progress("Завершение: медианная фильтрация");
    Algorithms::medianFilter(m_posterizedImage, m_posterizedImage, 3, 3);

    emit finished();
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
