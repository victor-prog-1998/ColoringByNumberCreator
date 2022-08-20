#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QJsonObject>

/*!
 * \brief Класс для работы с настройками приложения
 */
class ConfigManager : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Конструктор, в котором вызывается инициализация настроек (_init())
     * \param[in] parent - указатель на предка объекта
     */
    explicit ConfigManager(QObject *parent = nullptr);
    /*!
     * \brief Установка настроек и запись в конфигурационный файл
     * \param[in] simplify - флаг упрощения
     * \param[in] coloringColor - цвет контуров и цифр
     * \param[in] scalingFactor - коэффициент масштабирования
     */
    Q_INVOKABLE void setConfigs(bool simplify, const QString& coloringColor,
                                int scalingFactor = 1);
    /*!
     * \brief Получение установленного флага упрощения
     * \return Флаг упрощения
     */
    Q_INVOKABLE bool simplify() const
        {return m_configsJson["Simplify"].toBool();}
    /*!
     * \brief Получение коэффициента масштабирования
     * \return Коэффициент масштабирования
     */
    Q_INVOKABLE int scalingFactor() const
        {return m_configsJson["ScalingFactor"].toInt();}
    /*!
     * \brief Получение установленного цвета контуров и цифр
     * \return Цвет
     */
    Q_INVOKABLE QString coloringColor() const
        {return  m_configsJson["ColoringColor"].toString();}
private:
    /*!
     * \brief Инициализация настроек
     * \details Если файл настроек имеется, то настройки читаются из него
     *          Иначе - создается файл с настройками по умолчанию
     */
    void _init();
    /*!
     * \brief Записать настройки в конфигурационный файл
     */
    void _writeConfigsToFile();
    /*!
     * \brief Прочитать настройки из конфигурационного файла
     */
    void _readConfigsFromFile();

    //!< Имя файла настроек
    const QString m_fileName {"Configs.json"};
    //!< Флаг упрощения (false при высокой детализации раскраски)
    bool m_simplify {false};
    //!< Коэффициент масштабирования
    int m_scalingFactor = 1;
    //!< Цвета контуров и цифр раскраски
    QString m_coloringColor {"black"};
    //!< Json - объект с настройками
    QJsonObject m_configsJson;
signals:
};

#endif // CONFIGMANAGER_H
