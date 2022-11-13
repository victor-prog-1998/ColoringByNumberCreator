#ifndef ALGORITHMS_H
#define ALGORITHMS_H
#include <QImage>
#include "datatypes.h"

namespace Algorithms
{
    /*!
     * \brief Постеризация изображения
     * \param[in] sourceImage - исходное изображение
     * \param[in/out] result - постеризованное изображение
     * \param[in] colors - палитра постеризации
     */
    void posterize(const QImage& sourceImage, QImage& result,
                   const QList<QColor>& colors);

    /*!
     * \brief Создание изображение с границами областей исходного изображения
     * \details Области - группы объединенных пикселей, имеющих одинаковый цвет
     * \param[in] posterized - постеризованное изображение
     * \return изображение с границами областей
     */
    QImage makeEdgesImage(const QImage& posterized);
    /*!
     * \brief Замена цвета на изображении с цвета указанного пикселя
     *        на новый цвет
     * \param[in/out] image - изображение
     * \param[in] x - координата Х пикселя
     * \param[in] y - координата У пикселя
     * \param[in] color - новый цвет
     */
    void changeColor(QImage& image, int x, int y, const QColor& color);
    /*!
     * \brief Нахождение оптимальной палитры постеризации
     *        по указанному числу цветов
     * \param[in] image - изображение
     * \param[in] colorsCount - число цветов
     * \return
     */
    QStringList findOptimalPalette(const QImage& image, int colorsCount);
    /*!
     * \brief Нахождение индекса ближайшего цвета среди цветов в наборе
     * \param[in] color - цвет, для которого ищется индекс
     * \param[in] colors - набор цветов
     * \return - индекс в наборе
     */
    int findNearestColorIndex(const QColor &color, const QList<QColor>& colors);
    /*!
     * \brief Нахождение расстояния между цветами
     * \details Несет смысл визуальной разницы между цветами и полезно при
     *          нахождении ближайшего похожего цвета перебором.
     * \param color1[in] - первый цвет
     * \param color2[in] - второй цвет
     * \return - расстояние между цветами
     */
    double colorDistance(const QColor& color1, const QColor& color2);
    /*!
     * \brief Экспериментальная версия функции colorDistance
     * \details Находит расстояние по формулам с использованием пространства Lab
     * \param color1[in] - первый цвет
     * \param color2[in] - второй цвет
     * \return - расстояние между цветами
     */
    double colorDistanceLab(const QColor& color1, const QColor& color2);

    /*!
     * \brief Перевод цвета из пространства RGB в XYZ
     * \param rgbColor[in] - цвет
     * \param x[in/out] - составляющая Х пространства XYZ
     * \param y[in/out] - составляющая Y пространства XYZ
     * \param z[in/out] - составляющая Z пространства XYZ
     */
    void rgb2xyz(const QColor& rgbColor, double &x, double& y, double& z);
    /*!
     * \brief Перевод цвета из пространства XYZ в Lab
     * \param x[in] - составляющая Х пространства XYZ
     * \param y[in] - составляющая Y пространства XYZ
     * \param z[in] - составляющая Z пространства XYZ
     * \param l[in/out] - составляющая L пространства Lab
     * \param a[in/out] - составляющая a пространства Lab
     * \param b[in/out] - составляющая b пространства Lab
     */
    void xyz2Lab(double x, double y, double z, double& l, double& a, double& b);
    /*!
     * \brief Перевод цвета из пространства RGB в Lab
     * \param rgbColor[in] - цвет
     * \param l[in/out] - составляющая L пространства Lab
     * \param a[in/out] - составляющая a пространства Lab
     * \param b[in/out] - составляющая b пространства Lab
     */
    void rgb2Lab(const QColor& rgbColor, double &l, double &a, double &b);
    /*!
     * \brief Функция, используемая для перевода из XYZ в Lab
     * \param t - параметр функции
     * \return значение функции для указанного параметра
     */
    double fLab(double t);
    /*!
     * \brief Медианный фильтр для изображения
     * \param[in] sourceImage - исходное изображение
     * \param[in/out] result - изображение после фильтрации
     * \param[in] maskSize - размер маски фильтра
     * \param[in] iterations - число итераций фильтрации (нечетное число)
     * \details Если для размера маски указано четное число,
     *          фильтрация не отработает
     */
    void medianFilter(const QImage& sourceImage, QImage& result,
                      int maskSize = 3, int iterations = 1);
    /*!
     * \brief Усредняющий фильтр для изображения
     * \param[in] sourceImage - исходное изображение
     * \param[in/out] result - изображение после фильтрации
     * \param[in] maskSize - размер маски фильтра
     * \param[in] iterations - число итераций фильтрации (нечетное число)
     * \details Если для размера маски указано четное число,
     *          фильтрация не отработает
     */
    void averagingFilter(const QImage& sourceImage, QImage& result,
                         int maskSize = 3, int iterations = 1);
    /*!
     * \brief Заливка области изображения
     * \param x[in] - координата Х точки, из которой начинается заливка
     * \param у[in] - координата У точки, из которой начинается заливка
     * \param[in/out] image - изображение
     * \param[in] fillColor - цвет заливки
     * \param[in/out] pixels - пиксели, цвет которых был изменен
     *                в процессе заливки
     */
    void fill(int x, int y, QImage& image, const QColor& fillColor,
              QList<QPoint> *pixels = nullptr);
    /*!
     * \brief Заливка указанных пикселей указанным цветом на изображении
     * \param[in/out] image - изображение
     * \param[in] pixels - набор заливаемых пикселей
     * \param[in] color - цвет заливки
     */
    void fill(QImage& image, const QList<QPoint>& pixels, const QColor& color);
    /*!
     * \brief Получение набора уникальных цветов на изображении
     * \param[in] image - изображение
     * \return набор цветов
     */
    QList<QColor> getUniqueColors(const QImage& image);
    /*!
     * \brief Нахождение контура, описывающего заданную область
     * \details Если в области имеются внутренние области другого цвета,
     *          то пиксели границы между такими областями и текущей областью
     *          также включаются в выходной набор пикселей
     * \param[in] image - изображение
     * \param[in] pixels - пиксели области
     * \return набор пикселей, составляющих контур
     */
    QList<QPoint> findContour(const QImage& image, const QList<QPoint>& pixels);
    /*!
     * \brief Разбиение набора цветов на две части по критерию максимального
     *        визуального цветового отличия между получаемыми частями
     * \param[in] pixels - набор цветов
     * \return поделенный на две части исходный набор цветов
     */
    QPair<QList<QColor>,QList<QColor>> splitPixels(const QList<QColor>& pixels);
    /*!
     * \brief Получение набора цветов всех пикселей изображения
     * \param[in] image - изображение
     * \return набор цветов пикселей
     */
    QList<QColor> getImagePixels(const QImage& image);
    /*!
     * \brief Получение усредненного цвета для всех цветов в наборе
     * \param[in] colors - набор цветов
     * \return усредненный цвет
     */
    QColor averageColor(const QList<QColor>& colors);
    /*!
     * \brief Конвертация набора точек в объект PointsMatrix (матрица)
     * \details Функция строит матрицу размерами с прямоугольник, описывающий
     *          переданный набор точек. Элементы матрицы, соответствующие
     *          точкам, имеют значение "1", остальные - "0"
     * \param[in] points - набор точек
     * \return объект PointsMatrix
     */
    DataTypes::PointsMatrix makePointsMatrix(const QList<QPoint>& points);
    /*!
     * \brief Конвертация набора точек в объект PointsMatrix с масштабированием
     * \details Матрица на выходе имеет размер в scale раз больший размера
     *          прямоугольника, описывающего переданный набор точек.
     *          Для каждой точки из набора в матрицу записывается значение "1"
     *          для (scale * scale) соответствующих точке элементов.
     * \param[in] points - набор точек
     * \param[in] scale - масштаб
     * \return объект PointsMatrix
     */
    DataTypes::PointsMatrix makeScaledPointsMatrix(const QList<QPoint>& points,
                                                   uint8_t scale);
    /*!
     * \brief Нахождение областей на постеризованном изображении
     * \param[in] posterized - постеризованное изображение
     * \param[in] edges - изображение с границами областей
     *                постеризованного изображения
     * \param[in/out] areas - области
     * \param[in/out] colorsMap - набор "имя цвета - id цвета"
     */
    void findAreas(const QImage& posterized, const QImage& edges,
                   QList<DataTypes::Area>& areas, QMap<QString,int>& colorsMap);

    /*!
     * \brief Нахождение позиции цветовой метки (числа) на матрице точек
     * \details При поиске функция ищет такие координаты на матрице,
     *          в котором удастся вписать метку так, чтобы она не перекрывала
     *          ни одну точку, которая есть в матрицы (== "1").
     *          Если область слишком маленькая или узкая, такая позиция может
     *          не найтись.
     * \param[in] matrix - матрица точек
     * \param[in] textWidth - ширина цветовой метки
     * \param[in] textHeight - высота цветовой метки
     * \return Координаты цветовой метки относительно верхнего
     *         левого угла изображения, если удалось найти позицию.
     *         Иначе - точка с координатами (-1, -1)
     */
    QPoint findTextPosition(const DataTypes::PointsMatrix& matrix, int textWidth,
                            int textHeight);

    /*!
     * \brief Генерация цвета, которого нет в наборе
     * \details Цвет всегда генерируется так, чтобы быть не в градациях серого
     * \param[in] existingColorsNames - Набор, в котором цвета представлены
     *            в виде строковых представлений
     * \return
     */
    QColor generateNonExistingColor(const QList<QString>& existingColorsNames);

    /*!
     * \brief Получение числа цифр, из которых состоит число
     * \param[in] number - число
     * \return число цифр
     */
    int getCountOfDigits(int number);
    /*!
     * \brief Масштабирование изображения в 2 раза
     * \details При масштабировании происходит сглаживание без размытия.
     *          Размер итогового изображения == 2 в степени iterations.
     * \param[in] image - изображение
     * \param[in] iterations - число итераций
     * \return Масштабированное изображение
     */
    QImage scaleImage2x(const QImage& image, int iterations = 1);
    /*!
     * \brief Масштабирование изображения в 3 раза
     * \details При масштабировании происходит сглаживание без размытия.
     *          Размер итогового изображения == 3 в степени iterations.
     * \param[in] image - изображение
     * \param[in] iterations - число итераций
     * \return Масштабированное изображение
     */
    QImage scaleImage3x(const QImage& image, int iterations = 1);
};

#endif // ALGORITHMS_H
