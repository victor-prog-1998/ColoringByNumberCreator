#ifndef POSTERIZATIONTHREAD_H
#define POSTERIZATIONTHREAD_H
#include <QThread>
#include <QImage>

/*!
 * \brief Класс - поток постеризации изображения
 */
class PosterizationThread : public QThread
{
    Q_OBJECT
public:
    PosterizationThread(QObject* parent = nullptr);
    virtual ~PosterizationThread() override {}
    /*!
     * \brief Запуск потока
     */
    virtual void run() override;
    /*!
     * \brief Установка данных для обработки изображения
     * \param[in] filtered - флаг: отфильтровано ли входное изображение или нет
     * \param[in] image - входное изображение
     * \param[in] palette - палитра постеризации
     */
    void set(bool filtered, const QImage& image, const QList<QColor>& palette);
    /*!
     * \brief Возвращает отфильтрованное изображение
     * \return Изображение
     */
    const QImage& getFilteredImage() const {return m_filteredImage;}
    /*!
     * \brief Возвращает постеризованное изображение
     * \return Изображение
     */
    const QImage& getPosterizedImage() const {return m_posterizedImage;}
    /*!
     * \brief Возвращает флаг фильтрации
     * \return true - если входное изображение являлос отфильтрованным
     *         false - иначе
     */
    bool filtered() const {return m_filtered;}
signals:
    /*!
     * \brief Сигнал завершения работы потока
     */
    void finished();
    /*!
     * \brief Сигнал прогресса работы потока
     * \param message - сообщение о текущем прогрессе
     */
    void progress(const QString& message);
private:
    //!< Флаг фильтрации: отфильтровано ли входное изображение или нет
    bool m_filtered;
    //!< Изображение в исходном виде (без фильтрации)
    QImage m_image;
    //!< Палитра постеризации
    QList<QColor> m_palette;
    //!< Отфильтрованное изображение
    QImage m_filteredImage;
    //!< Постеризованное изображение
    QImage m_posterizedImage;
};

#endif // POSTERIZATIONTHREAD_H
