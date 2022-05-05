#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H
#include <QQuickImageProvider>


class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    void clear() {m_imagesMap.clear();}
    void add(const QString& id, const QImage& image);
    int size() const {return m_imagesMap.size();}
    QImage get(const QString& id) const {return m_imagesMap[id];}
    void remove(const QString& key) {m_imagesMap.remove(key);}
    bool contains(const QString& key) const {return  m_imagesMap.contains(key);}

private:
    QMap<QString, QImage> m_imagesMap;
};

#endif // IMAGEPROVIDER_H
