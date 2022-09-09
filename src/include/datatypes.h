#ifndef DATATYPES_H
#define DATATYPES_H
#include <QList>
#include <QPoint>
#include <chrono>

namespace DataTypes
{
/*!
 * \brief Структура, хранящая внешние и внутренние точки области,
 *        а также id цвета
 */
struct Area
{
    /*!
     * \brief Конструктор
     * \param[in] contour - набор внешних точек (контур)
     * \param[in] interior - набор внутренних точек
     * \param[in] id - идентификатор (числовая метка)
     */
    Area(const QList<QPoint>& contour, const QList<QPoint>& interior, int id):
        contourPoints(contour), interiorPoints(interior), colorId(id) {}

    //!< Внешние точки (контур)
    QList<QPoint> contourPoints;
    //!< Внутренние точки
    QList<QPoint> interiorPoints;
    //! Идентификатор цвета (числовая метка)
    int colorId;
};

/*!
 * \brief Структура для хранения точек области
 * \details Размеры матрицы соответсвтуют размерам прямогульника (в пикселях),
 *          в который вписаны точки области.
 *          Элементы имеют следующие значения:
 *          "1" - если на данном месте есть точка области;
 *          "0" - если точек нет.
 *          Таким образом в конкретной позиции матрицы элемент хранит факт
 *          присутствия точки интересующей области на изображении.
 *          Для матрицы задается смещение относительно верхнего
 *          левого угла изображения
 */
struct PointsMatrix
{
    //!< Двухмерный набор значений (матрица)
    QVector<QVector<uint8_t>> values;
    //!< Смещение описывающего прямоугольника по оси Х [пикс]
    uint16_t x;
    //!< Смещение описывающего прямоугольника по оси У [пикс]
    uint16_t y;
    //!< Высота описывающего прямоугольника (число строк матрицы) [пикс]
    uint16_t height() const {return values.size();}
    //!< Ширина описывающего прямоугольника (число столбцов матрицы) [пикс]
    uint16_t width() const {return values.isEmpty() ? 0 : values[0].size();};
};

/*!
 * \brief Класс для измерения времени выполнения кода
 */
class StopWatch
{
public:
    /*!
     * \brief Начать измерение
     */
    void start()  {mStart = std::chrono::steady_clock::now();}
    /*!
     * \brief Закончить измерение
     */
    void finish() {mFinish = std::chrono::steady_clock::now();}
    /*!
     * \brief Получить замеренное время (по умолчанию - в микросекундах)
     */
    template<class T = std::chrono::microseconds>
    size_t duration() const
    {
        return std::chrono::duration_cast<T>(mFinish - mStart).count();
    }
private:
    //!< Начальная точка измерения
    std::chrono::steady_clock::time_point mStart;
    //!< Конечная точка измерения
    std::chrono::steady_clock::time_point mFinish;
};
}

#endif // DATATYPES_H
