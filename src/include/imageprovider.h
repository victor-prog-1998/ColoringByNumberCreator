#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H
#include <QQuickImageProvider>

/*!
 * \brief Класс-посредник для передечи изображений из C++ в QML
 */
class ImageProvider : public QQuickImageProvider
{
public:
    //!< Перечисление для удобства работы с ключами изображений
    enum ImageKey
    {
        Filtered ,        //!< Фильтрованное исходное изображение
        Posterized,       //!< Постеризованное изображение
        ScaledPosterized, //!< Масштабированное постеризованное изображение
        Edges,            //!< Изображение с границами областей
        Coloring,         //!< Изображение с раскраской по номерам
        Painted,          //!< Изображение с раскрашенной раскраской
        Legend            //!< Изображение с легендой раскраски
    };
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
    void clear();
    /*!
     * \brief Добавление изображения в провайдер
     * \param[in] id - ключ изображения
     * \param[in] image - изображение
     */
    void add(const QString& id, const QImage& image);
    /*!
     * \brief Добавление изображения в провайдер
     * \param[in] key - ключ изображения
     * \param[in] image - изображение
     */
    void add(ImageKey key, const QImage& image);
    /*!
     * \brief size - получение числа изображений в провайдере
     * \return число изображений
     */
    int size() const;
    /*!
     * \brief Получение изображения по указанному ключу
     * \param[in] id - ключ изображения
     * \return изображений
     */
    QImage get(const QString& id) const;
    /*!
     * \brief Получение изображения по указанному ключу
     * \param[in] key - ключ изображения
     * \return изображений
     */
    QImage get(ImageKey key) const;
    /*!
     * \brief Удаление из провайдера изображения по указанному ключу
     * \param[in] key - ключ изображения
     */
    void remove(const QString& key);
    /*!
     * \brief Удаление из провайдера изображения по указанному ключу
     * \param[in] key - ключ изображения
     */
    void remove(ImageKey key);
    /*!
     * \brief Проверка наличия в провайдере изображения с указанным ключом
     * \param[in] key - ключ изображения
     * \return true - если изображение имеется
     *         false - иначе
     */
    bool contains(const QString& key) const;
    /*!
     * \brief Проверка наличия в провайдере изображения с указанным ключом
     * \param[in] key - ключ изображения
     * \return true - если изображение имеется
     *         false - иначе
     */
    bool contains(ImageKey key) const;

private:
    //!< Набор пар "ключ - изображение"
    QMap<QString, QImage> m_imagesMap;
    //!< Набор пар, "числовой код изображения (ImageKey) - ключ изображения"
    QMap<uint8_t, QString> m_imageKeySources;
};

#endif // IMAGEPROVIDER_H
