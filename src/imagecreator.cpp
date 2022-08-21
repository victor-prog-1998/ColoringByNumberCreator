#include "include/imagecreator.h"
#include <QPainter>
#include <QDebug>
#include <math.h>

ImageCreator::ImageCreator()
{
    _initDigits();
}

void ImageCreator::createSimplifiedImages(const QImage &posterized,
                                          const QList<DataTypes::Area>& areas,
                                          const QMap<QString, int> &colorsMap)
{
    // Цвет, которого нет в наборе, которым предварительно будет залито готовое изображение
    QColor tmpColor = Algorithms::generateNonExistingColor(colorsMap.keys());
    _createLegend(colorsMap);
    m_coloringImage = QImage(posterized.size(), QImage::Format_RGB888);
    m_coloringImage.fill(Qt::white);
    m_paintedImage = m_coloringImage;
    m_paintedImage.fill(tmpColor);

    QPainter painter(&m_coloringImage);

    painter.setPen(m_coloringColor); // цвет контуров

    // Первый проход (подготовка)
    for(const auto& area: areas)
    {
        int colorId = area.colorId;
        // Получаем матрицу внутренних точек контура
        auto interiorMatrix = Algorithms::makePointsMatrix(area.interiorPoints);
         //! Пытаемся вписать метку, начиная с наибольшего размера шрифта
        for(int size = 2; size >= 0; --size)
        {
            int w = _getNumberTextWidth(colorId, size);
            int h = m_digits[size][0].height();
            // Находим позицию метки цвета
            QPoint textPos = Algorithms::findTextPosition(interiorMatrix, w, h);
            // Если удалось вписать метку цвета в контур
            if(textPos.x() != -1)
            {
                // Рисуем контур на раскраске
                _drawContour(m_coloringImage, area.contourPoints);
                // Рисуем контур на готовом изображении
                _drawContour(m_paintedImage, area.contourPoints);
                // Закрашиваем пиксели готового изображения текущим цветом
                for(const auto& point: area.interiorPoints)
                    m_paintedImage.setPixelColor(point.x(), point.y(),
                                                 QColor(colorsMap.key(colorId)));
                // Рисуем метку цвета на раскраске
                _drawNumber(textPos.x(), textPos.y(), colorId, size, painter);
                break;
            }
        }
    }

    /* Работа с неразмеченными контурами, а также с контурами, которые образовались
     * в ходе создания раскраски, но изначально в наборе их не было
     */
    for(int y = 0; y < m_paintedImage.height(); ++y)
      for(int x = 0; x < m_paintedImage.width(); ++x)
      {
        if(m_paintedImage.pixelColor(x, y) == tmpColor)
        {
          QColor color = posterized.pixelColor(x, y);
          QList<QPoint> filledPixels;
          Algorithms::fill(x, y, m_paintedImage, color, &filledPixels);
          int colorId = colorsMap[color.name(QColor::HexRgb)];
          auto matrix = Algorithms::makePointsMatrix(filledPixels);
          //! Пытаемся вписать метку, начиная с наибольшего размера шрифта
          for(int size = 2; size >= 0; --size)
          {
              int w = _getNumberTextWidth(colorId, size);
              int h = m_digits[size][0].height();
              auto textPos = Algorithms::findTextPosition(matrix, w, h);
              if(textPos.x() != -1)
              {
                  _drawNumber(textPos.x(), textPos.y(), colorId, size, painter);
                  break;
              }
          }
        }
      }

    painter.setPen(Qt::black); // цвет рамки
    painter.drawRect(0, 0, m_coloringImage.width() - 1,
                     m_coloringImage.height() - 1);
}

void ImageCreator::createImages(const QImage &posterized,
                                const QList<DataTypes::Area> &areas,
                                const QMap<QString, int> &colorsMap)
{
    // Цвет, которого нет в наборе, которым предварительно будет залито готовое изображение
    QColor tmpColor = Algorithms::generateNonExistingColor(colorsMap.keys());
    _createLegend(colorsMap);
    m_coloringImage = QImage(posterized.size(), QImage::Format_RGB888);
    m_coloringImage.fill(Qt::white);
    m_paintedImage = m_coloringImage;
    m_paintedImage.fill(tmpColor);
    QPainter painter(&m_coloringImage);

    painter.setPen(m_coloringColor); // цвет контуров

    // Первый проход (подготовка)
    for(const auto& area: areas)
    {
        int colorId = area.colorId;
        // Получаем матрицу внутренних точек контура
        auto interiorMatrix = Algorithms::makePointsMatrix(area.interiorPoints);
        // Рисуем контур на раскраске и на готовом изображении
        _drawContour(m_coloringImage, area.contourPoints);
        _drawContour(m_paintedImage, area.contourPoints);
        // Закрашиваем пиксели готового изображения текущим цветом
        for(const auto& point: area.interiorPoints)
            m_paintedImage.setPixelColor(point.x(), point.y(),
                                         QColor(colorsMap.key(colorId)));

        //! Пытаемся вписать метку, начиная с наибольшего размера шрифта
        for(int size = 2; size >= 0; --size)
        {
            int w = _getNumberTextWidth(colorId, size);
            int h = m_digits[size][0].height();
            // Находим позицию метки цвета
            QPoint textPos = Algorithms::findTextPosition(interiorMatrix, w, h);
            // Если удалось вписать метку цвета в контур, то рисуем её
            if(textPos.x() != -1)
            {
                _drawNumber(textPos.x(), textPos.y(), colorId, size, painter);
                break;
            }
        }
    }

    // Работа с пустыми областями (которые образовались при отрисовке раскраски)
    for(int y = 0; y < m_paintedImage.height(); ++y)
      for(int x = 0; x < m_paintedImage.width(); ++x)
      {
        if(m_paintedImage.pixelColor(x, y) == tmpColor)
        {
          QColor color = posterized.pixelColor(x, y);
          QList<QPoint> filledPixels;
          Algorithms::fill(x, y, m_paintedImage, color, &filledPixels);
        }
      }

    painter.setPen(Qt::black); // цвет рамки
    painter.drawRect(0, 0, m_coloringImage.width() - 1,
                     m_coloringImage.height() - 1);
}

QList<QPair<QImage, QString>> ImageCreator::tileColoringImage(int rows,
                                                              int columns) const
{
    QList<QPair<QImage,QString>> tiles;
    int tileWidth = std::ceil(static_cast<double>
                              (m_coloringImage.width()) / columns);
    int tileHeight = std::ceil(static_cast<double>
                               (m_coloringImage.height()) / rows);
    int lastTileWidth = m_coloringImage.width() % tileWidth;
    if(lastTileWidth == 0) lastTileWidth = tileWidth;
    int lastTileHeight = m_coloringImage.height() % tileHeight;
    if(lastTileHeight == 0) lastTileHeight = tileHeight;
    for(int row = 0; row < rows; ++row)
    {
      QChar rowChar(static_cast<char>('A' + row));
      for(int column = 0; column < columns; ++column)
      {
        int x = column * tileWidth;
        int y = row * tileHeight;
        int width =  column == columns - 1 ? lastTileWidth : tileWidth;
        int height = row == rows - 1 ? lastTileHeight : tileHeight;

        QImage fragment = m_coloringImage.copy(x, y, width, height);

        QImage tile(tileWidth + 2, tileHeight + 2, QImage::Format_RGB888);
        tile.fill(Qt::white);
        QPainter painter(&tile);
        painter.drawImage(1, 1, fragment);

        painter.setPen(Qt::gray);
        // top
        if(row > 0)
            painter.drawLine(1, 0, fragment.width(), 0);
        // bottom
        if(row < rows - 1)
            painter.drawLine(1, fragment.height() + 1, fragment.width(),
                             fragment.height() + 1);
        // left
        if(column > 0)
            painter.drawLine(0, 1, 0, fragment.height());
        // right
        if(column < columns - 1)
            painter.drawLine(fragment.width() + 1, 1, fragment.width() + 1,
                             fragment.height());
        QString name = rowChar + '-' + QString::number(column + 1);
        tiles.push_back({tile, name});
      }
    }
    return tiles;
}

void ImageCreator::setColoringColor(const QColor &color)
{
    m_coloringColor = color;
    // + меняем цвета пикселей для изображений с цифрами
    if(!m_digits[0][0].isNull())
    {
      for(int size = 0; size < 3; ++size)
        for(int i = 0; i < 10; ++i)
          for(int y = 0; y < m_digits[size][i].height(); ++y)
            for(int x = 0; x < m_digits[size][i].width(); ++x)
            {
              if(m_digits[size][i].pixelColor(x, y) != Qt::white)
                m_digits[size][i].setPixelColor(x, y, color);
            }
    }
}

int ImageCreator::_getNumberTextWidth(int number, int size)
{
    int count = Algorithms::getCountOfDigits(number);
    return count * (m_digits[size][0].width() +
           m_numberTextSpacing) - m_numberTextSpacing;
}

void ImageCreator::_drawContour(QImage &image, const QList<QPoint> &points)
{
    for(const auto& point: points)
        image.setPixelColor(point.x(), point.y(), m_coloringColor);
}

void ImageCreator::_createLegend(const QMap<QString, int> &colorsMap)
{
    const int margins = 10;
    const int spacing = 5;
    const int itemSize = 64;
    const int labelItemSize = 24;
    const int columnsCount = 4;
    int rowsCount = (colorsMap.size() - 1) / columnsCount + 1;
    int legendWidth = margins * 2 + columnsCount *
                      (itemSize + spacing) - spacing;
    int legendHeight = margins * 2 + rowsCount * (itemSize + spacing) - spacing;
    m_legend = QImage(legendWidth, legendHeight, QImage::Format_BGR888);
    m_legend.fill(Qt::white);
    QPainter painter(&m_legend);
    painter.setPen(Qt::black);
    QFont font("Arial");
    font.setPixelSize(14);
    painter.setFont(font);
    for(const auto& colorName: colorsMap.keys())
    {
        int id = colorsMap[colorName];
        int index = id - 1;
        int row = index / columnsCount;
        int column = index % columnsCount;
        int x = margins + column * (itemSize + spacing);
        int y = margins + row * (itemSize + spacing);
        painter.setBrush(QBrush(QColor(colorName)));
        painter.drawRect(x, y, itemSize - 1, itemSize - 1);
        painter.setBrush(QBrush(Qt::white));
        painter.drawRect(x, y, labelItemSize - 1, labelItemSize - 1);
        painter.drawText(QRect(x, y, labelItemSize - 1, labelItemSize - 1),
                         Qt::AlignCenter, QString::number(id));
    }
}

void ImageCreator::_drawNumber(int x, int y, uint8_t number, int size, QPainter& painter)
{
    int count = Algorithms::getCountOfDigits(number);
    int width = m_digits[size][0].width();
    x += (width + m_numberTextSpacing) * (count - 1);
    for(int i = 0; i < count; ++i)
    {
        int digitIndex = number % 10;
        painter.drawImage(x, y, m_digits[size][digitIndex]);
        number /= 10;
        x -= (width + m_numberTextSpacing);
    }
}



void ImageCreator::_initDigits()
{
    //! Small
    QString pathPrefix = ":/digits/small/%1.png";
    for(int i = 0; i < 10; ++i)
        m_digits[0][i].load(pathPrefix.arg(i));
    //! Medium
    pathPrefix = ":/digits/medium/%1.png";
    for(int i = 0; i < 10; ++i)
        m_digits[1][i].load(pathPrefix.arg(i));
    //! Large
    pathPrefix = ":/digits/large/%1.png";
    for(int i = 0; i < 10; ++i)
        m_digits[2][i].load(pathPrefix.arg(i));
}
