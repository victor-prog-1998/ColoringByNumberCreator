#ifndef IMAGECREATOR_H
#define IMAGECREATOR_H
#include <QImage>
#include "algorithms.h"

/*!
 * \brief Класс для создания выходных изображений,
 *        относящихся к раскраске
 */
class ImageCreator
{
public:
    ImageCreator();
    /*!
     * \brief Создание изображений, относящихся к раскраске,
     *        с высокой детализацией
     * \param[in] posterized - постеризованное изображение
     * \param[in] areas - набор областей
     * \param[in] colorsMap - набор пар "цвет (строка) - id цвета"
     */
    void createImages(const QImage& posterized,
                      const QList<DataTypes::Area>& areas,
                      const QMap<QString, int>& colorsMap);
    /*!
     * \brief Создание изображений, относящихся к раскраске,
     *        с упрощенной детализацией
     * \param[in] posterized - постеризованное изображение
     * \param[in] areas - набор областей
     * \param[in] colorsMap - набор пар "цвет (строка) - id цвета"
     */
    void createSimplifiedImages(const QImage& posterized,
                                const QList<DataTypes::Area>& areas,
                                const QMap<QString, int>& colorsMap);
    /*!
     * \brief Возвращает изображение с раскраской по номерам
     * \return изображение
     */
    const QImage& getColoringImage() const {return m_coloringImage;}
    /*!
     * \brief Возвращает изображение с готовой раскраской
     * \return изображение
     */
    const QImage& getPaintedImage() const {return m_paintedImage;}
    /*!
     * \brief Возвращает изображение с легендой раскраски
     * \return изображение
     */
    const QImage& getLegendImage() const {return m_legend;}
    /*!
     * \brief Нарезка изображения с раскраской по номерам на фрагменты
     * \details Каждому фрагменту присваивается строка с меткой
     *          вида "Буква-Цифра".
     *          Буквы начинаются с "А" и возрастают по вертикали сверху вниз.
     *          Цифры начинаются с "1" и возрастают по горизонтали слева направо
     * \param[in] rows - нарезка по вертикали
     * \param[in] columns - нарезка по горизонатали
     * \return набор пар "фрагмент - метка фрагмента"
     */
    QList<QPair<QImage, QString>> tileColoringImage(int rows,
                                                    int columns) const;
    /*!
     * \brief setColoringColor - Установка цвета контуров и цифр на раскраске
     * \param[in] color - цвет
     */
    void setColoringColor(const QColor& color);

private:
    /*!
     * \brief Получение ширины числа [пикс]
     * \param[in] number - число
     * \return ширина
     */
    int _getNumberTextWidth(int number);
    /*!
     * \brief Рисование контура на изображении
     * \param[in/out] image - изображение
     * \param[in] points - точки контура
     */
    void _drawContour(QImage& image, const QList<QPoint>& points);
    /*!
     * \brief Создание изображения с легендой раскраски
     * \param[in] colorsMap - набор, в котором каждому цвету поставлена
     *            в соответствие числовая метка
     */
    void _createLegend(const QMap<QString, int>& colorsMap);
    /*!
     * \brief Отрисовка числовой метки
     * \param[in] x - координата Х метки
     * \param[in] y - координата У метки
     * \param[in] number - число
     * \param[in/out] painter - объект QPainter c заданным контекстом рисования
     */
    void _drawNumber(int x, int y, uint8_t number, QPainter &painter);
    /*!
     * \brief Создание изображений с цифрами (инициализация массива m_digits)
     */
    void _makeDigits();

    //!< Высота цифры [пикс]
    const int m_digitHeight = 9;
    //!< Ширина цифры [пикс]
    const int m_digitWidth = 6;
    //!< Расстояние между цифрами
    const int m_numberTextSpacing = -1;
    //!< Изображение с раскраской по номерам
    QImage m_coloringImage;
    //!< Изображение с готовой раскраской
    QImage m_paintedImage;
    //!< Изображение с легендой раскраски
    QImage m_legend;
    //!< Набор изображений с цифрами
    QImage m_digits[10];
    //!< Цвет контуров и цифр на раскраске
    QColor m_coloringColor{Qt::black};
};

#endif // IMAGECREATOR_H
