#ifndef FINDPALETTETHREAD_H
#define FINDPALETTETHREAD_H
#include <QThread>
#include <QImage>

/*!
 * \brief Класс - поток расчёта оптимальной палитры изображения
 */
class FindPaletteThread : public QThread
{
    Q_OBJECT
public:
    FindPaletteThread(QObject* parent = nullptr);
    virtual ~FindPaletteThread() override {}
    /*!
     * \brief Запуск потока
     */
    virtual void run() override;
    /*!
     * \brief Установка данных для расчёта
     * \param[in] image - изображение
     * \param[in] colorsCount - число цветов палитры
     */
    void set(const QImage& image, int colorsCount);
signals:
    /*!
     * \brief Сигнал завершения расчета палитры (завершения потока)
     * \param palette - палитра в виде списка строк с именами цветов
     */
    void finished(const QStringList& palette);
private:
    //!< Изображение
    QImage m_image;
    //!< Число цветов палитры
    int m_colorsCount;
};

#endif // FINDPALETTETHREAD_H
