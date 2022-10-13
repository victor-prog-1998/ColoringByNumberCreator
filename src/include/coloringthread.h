#ifndef COLORINGTHREAD_H
#define COLORINGTHREAD_H
#include <QThread>
#include <QImage>
#include "imagecreator.h"

/*!
 * \brief Класс - поток создания раскраски по номерам
 *        и сопутствующих изображений - готовой раскраски
 *        и легенды
 */
class ColoringThread : public QThread
{
    Q_OBJECT
public:
    ColoringThread(QObject* parent = nullptr);
    virtual ~ColoringThread() override {}
    /*!
     * \brief Задание объекта ImageCreator
     * \param[in] imageCreator - указатель на объект ImageCreator
     */
    void setImageCreator(ImageCreator* imageCreator);
    /*!
     * \brief Запуск потока
     */
    virtual void run() override;
    /*!
     * \brief Задание постеризованного изображения
     * \param[in] image - постеризованное изображение
     */
    void setPosterizedImage(const QImage& image);
    /*!
     * \brief Задание флага масштабирования
     * \param[in] scaled - значение: true, если заданное постеризованное
     *            изображение уже масштабировано; false - иначе
     */
    void setScaled(bool scaled);
    /*!
     * \brief Задание коэффициента масштабирования
     * \param[in] factor - коэффициент масштабирования
     */
    void setScalingFactor(int factor);
    /*!
     * \brief Задание флага краев
     * \param[in] edges - значение: true, если заданно изображение
     *            с границами областей; false - иначе
     */
    void setEdges(bool edges);
    /*!
     * \brief Задание изображения с границами областей
     * \param[in] image - изображение с границами областей
     */
    void setEdgesImage(const QImage& image);
    /*!
     * \brief Задание цвета контуров и цифр раскраски
     * \param[in] color - цвет
     */
    void setColoringColor(const QColor& color);
    /*!
     * \brief Установка флага упрощения
     * \param[in] simplification - значение: true, если стоит применять
     *            упрощённый алгоритм создания раскраски, false - иначе
     */
    void setSimplification(bool simplification);
    /*!
     * \brief Возврат масштабированного постеризованного изображения
     * \return Изображение
     */
    const QImage& getScaledPosterized() const {return m_posterizedImage;}
    /*!
     * \brief Возврат изображения с границами областей
     * \return
     */
    const QImage& getEdgesImage() const {return m_edgesImage;}
    /*!
     * \brief Значение флага краёв
     * \return true - если было задано изображение с границами областей
     *         false - иначе
     */
    bool getEdges() const {return m_edges;}
    /*!
     * \brief Значение флага масштабирования
     * \return true - если заданное постеризованное изображение
     *         уже было масштабировано
     *         false - иначе
     */
    bool getScaled() const {return m_scaled;}
private:
    /*!
     * \brief Масштабирование постеризованного изображения по
     *        установленному коэффициенту масштабирования
     */
    void _scalePosterizedImage();
    /*!
     * \brief Создание изображения с границами областей
     */
    void _makeEdgesImage();
private:
    //!< Флаг масштабирования: было ли поданное на вход постеризованное
    //!<  изображение масштабировано
    bool m_scaled;
    //!< Коэффициент масштабирования
    int m_scalingFactor;
    //!< Флаг краёв: было ли подано на вход изображение с границами областей
    bool m_edges;
    //!< Флаг упрощения: нужно ли применять алгоритм создания раскраски
    //!<  с упрощённой детализацией
    bool m_simplification;
    //!< Цвет контуров и цифр раскраски
    QColor m_color;
    //!< Постеризованное изображение
    QImage m_posterizedImage;
    //!< Изображение с границами областей
    QImage m_edgesImage;
    //!< Указатель на объект ImageCreator
    ImageCreator *m_imageCreator;
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
};

#endif // COLORINGTHREAD_H
