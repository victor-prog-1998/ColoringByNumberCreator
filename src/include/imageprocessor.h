#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include "imageprovider.h"
#include "configmanager.h"
#include "imagecreator.h"

/*!
 * \brief Класс высокого уровня, связвающий графический интерфейс на QML
 *        с вычислениями на C++
 */
class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = nullptr);
    /*!
     * \brief Установка провайдера изображений
     * \param[in] provider - провайдер
     */
    void setImageProvider(ImageProvider* provider) {m_imageProvider = provider;}
    /*!
     * \brief Установка менеджера настроек
     * \param[in] configManager - менеджер настроек
     */
    void setConfigManager(ConfigManager* configManager)
                         {m_configManager = configManager;}
    /*!
     * \brief Постеризация текущего изображения
     * \details Результат добавляется в провайдер
     */
    Q_INVOKABLE void posterize();
    /*!
     * \brief Установка текущего изображения
     * \details Изображение добавляется в провайдер
     * \param[in] source - путь к изображению
     */
    Q_INVOKABLE bool setCurrentImage(const QString& source);
    /*!
     * \brief Установка набора цветов раскраски (палитры)
     * \param[in] colors - набор цветов, заданных строками
     */
    Q_INVOKABLE void setColors(const QStringList& colors);
    /*!
     * \brief Нахождение оптимальной палитры постеризации
     *        по указанному числу цветов
     * \param[in] colorsCount - число цветов
     * \return
     */
    Q_INVOKABLE QStringList findOptimalPalette(int colorsCount);
    /*!
     * \brief Замена цвета на постеризованном изображении с цвета
     *        указанного пикселя на новый цвет
     * \param[in] x - координата Х пикселя
     * \param[in] y - координата У пикселя
     * \param[in] color - новый цвет
     */
    Q_INVOKABLE void changeColor(int x, int y, const QColor& color);
    /*!
     * \brief Установка цвета пикселя на постеризованном изображении
     * \param[in] x - координата Х пикселя
     * \param[in] y - координата У пикселя
     * \param[in] color - цвет
     */
    Q_INVOKABLE void setPixelColor(int x, int y, const QColor& color);
    /*!
     * \brief Заливка области постеризованноо изображения
     * \param[in] x - координата Х пикселя, принадлежащего области
     * \param[in] y - координата У пикселя, принадлежащего области
     * \param[in] fillColor - цвет заливки
     */
    Q_INVOKABLE void fill(int x, int y, const QColor& fillColor);
    /*!
     * \brief Создание изображения с границами областей
     *        постеризованного изображения
     * \details Результат добавляется в провайдер
     */
    Q_INVOKABLE void edges();
    /*!
     * \brief Создание изображений относящихся к раскраске
     * \details Создает раскраску по номерам, готовое изображение
     *          и легенду раскраски. Добавляет данные изображения
     *          в провайдер.
     */
    Q_INVOKABLE void coloring();
    /*!
     * \brief Удаление изображения с границами областей из
     *        провайдера изображений
     */
    Q_INVOKABLE void removeEdgesFromProvider();
    /*!
     * \brief Удаление из провайдера изображений, относящихся к раскраске
     * \details Удаляет раскраску по номерам, готовое изображение
     *          и легенду раскраски
     */
    Q_INVOKABLE void removeColoringFromProvider();
    /*!
     * \brief Сохранение раскраски в указанную папку
     * \details В указанной папке создается папка с результатами -
     *          раскраской по номерам, готовой раскраской и легендой.
     *          Если параметры нарезки ненулевые, то в созданной папке
     *          создается вложенная папка с нарезанной раскраской по номерам.
     *          Файлы в ней имеют вид "Буква-Цифра".
     *          Буквы начинаются с "А" и возрастают по вертикали для фрагментов,
     *          от верхних к нижним.
     *          Цифры начинаются с "1" и возрастают по горизонтали для
     *          фрагментов, от левым к правым.
     * \param[in] folderPath - папка сохранения результатов
     * \param[in] tileRows - нарезка по вертикали
     * \param[in] tileColumns - нарезка по горизонтали
     */
    Q_INVOKABLE void saveResults(const QString& folderPath,
                                 int tileRows = 0, int tileColumns = 0);
private:
    //!< Текущее изображение
    QImage m_currentImage;
    //!< Провайдер изображений
    ImageProvider *m_imageProvider;
    //!< Менеджер настроек
    ConfigManager *m_configManager;
    //!< Объект класса для создания результатов (раскраски)
    ImageCreator m_imageCreator;
    //!< Набор цветов раскраски
    QList<QColor> m_colors;
signals:

};

#endif // IMAGEPROCESSOR_H
