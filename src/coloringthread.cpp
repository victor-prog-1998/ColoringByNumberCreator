#include "include/coloringthread.h"
#include "include/algorithms.h"

ColoringThread::ColoringThread(QObject* parent): QThread(parent)
{

}

void ColoringThread::setImageCreator(ImageCreator *imageCreator)
{
    m_imageCreator = imageCreator;
}

void ColoringThread::run()
{
    //! Если нет изображения с границами областей, создаем его
    if(!m_edges)
    {
        //! Если помимо этого нет масштабированного постеризованного
        //! изображения, то перед созданием изображения с границами
        //! создаем масштабированное изображение
        if(!m_scaled)
        {
            emit progress("Масштабирование изображения");
            _scalePosterizedImage();
        }
        emit progress("Распознавание границ областей");
        _makeEdgesImage();
    }

    QMap<QString, int> colorsMap; // color -> id
    QList<DataTypes::Area> areas;
    emit progress("Нахождение областей");
    Algorithms::findAreas(m_posterizedImage, m_edgesImage, areas, colorsMap);
    m_imageCreator->setColoringColor(m_color);
    emit progress("Создание раскраски");
    if(m_simplification)
        m_imageCreator->createSimplifiedImages(m_posterizedImage, areas,
                                              colorsMap);
    else
        m_imageCreator->createImages(m_posterizedImage, areas, colorsMap);

    emit finished();
}

void ColoringThread::setPosterizedImage(const QImage &image)
{
    m_posterizedImage = image;
}

void ColoringThread::setScaled(bool scaled)
{
    m_scaled = scaled;
}

void ColoringThread::setScalingFactor(int factor)
{
    m_scalingFactor = factor;
}

void ColoringThread::setEdges(bool edges)
{
    m_edges = edges;
}

void ColoringThread::setEdgesImage(const QImage &image)
{
    m_edgesImage = image;
}

void ColoringThread::setColoringColor(const QColor &color)
{
    m_color = color;
}

void ColoringThread::setSimplification(bool simplification)
{
    m_simplification = simplification;
}

void ColoringThread::_scalePosterizedImage()
{
    switch(m_scalingFactor)
    {
    case 2:
        m_posterizedImage = Algorithms::scaleImage2x(m_posterizedImage);
        break;
    case 3:
        m_posterizedImage = Algorithms::scaleImage3x(m_posterizedImage);
        break;
    case 4:
        m_posterizedImage = Algorithms::scaleImage2x(m_posterizedImage, 2);
        break;
    case 6:
        m_posterizedImage = Algorithms::scaleImage2x(m_posterizedImage);
        m_posterizedImage = Algorithms::scaleImage3x(m_posterizedImage);
        break;
    case 8:
        m_posterizedImage = Algorithms::scaleImage2x(m_posterizedImage, 3);
        break;
    case 9:
        m_posterizedImage = Algorithms::scaleImage3x(m_posterizedImage, 2);
        break;
    case 12:
        m_posterizedImage = Algorithms::scaleImage3x(m_posterizedImage);
        m_posterizedImage = Algorithms::scaleImage2x(m_posterizedImage, 2);
        break;
    }
}

void ColoringThread::_makeEdgesImage()
{
    m_edgesImage = Algorithms::makeEdgesImage(m_posterizedImage);
}
