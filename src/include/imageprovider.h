#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H
#include <QQuickImageProvider>

/*!
 * \brief Класс-посредник для передечи изображений из C++ в QML
 */
class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider();
    /*!
     * \brief Получение изображения
     * \param[in] id - ключ изображения в провайдере
     * \param[in] size - размер мзображения
     * \param[in] requestedSize - требуемый размер изображения
     * \return изображение
     */
    QImage requestImage(const QString &id, QSize *size,
                        const QSize &requestedSize) override;
    /*!
     * \brief Удаление всех изображений из провайдера
     */
    void clear() {m_imagesMap.clear();}
    /*!
     * \brief Добавление изображения в провайдер
     * \param[in] id - ключ изображения
     * \param[in] image - изображение
     */
    void add(const QString& id, const QImage& image);
    /*!
     * \brief size - получение числа изображений в провайдере
     * \return число изображений
     */
    int size() const {return m_imagesMap.size();}
    /*!
     * \brief Получение изображения по указанному ключу
     * \param[in] id - ключ изображения
     * \return изображений
     */
    QImage get(const QString& id) const {return m_imagesMap[id];}
    /*!
     * \brief Удаление из провайдера изображения по указанному ключу
     * \param[in] key - ключ изображения
     */
    void remove(const QString& key) {m_imagesMap.remove(key);}
    /*!
     * \brief Проверка наличия в провайдере изображения с указанным ключом
     * \param[in] key - ключ изображения
     * \return true - если изображение имеется
     *         false - иначе
     */
    bool contains(const QString& key) const {return  m_imagesMap.contains(key);}

private:
    //!< Набор пар "ключ-изображение"
    QMap<QString, QImage> m_imagesMap;
};

#endif // IMAGEPROVIDER_H
