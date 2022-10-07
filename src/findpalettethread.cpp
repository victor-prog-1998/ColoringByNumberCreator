#include "include/findpalettethread.h"
#include "include/algorithms.h"

FindPaletteThread::FindPaletteThread(QObject* parent):
    QThread(parent)
{
}

void FindPaletteThread::run()
{
  auto palette = Algorithms::findOptimalPalette(m_image, m_colorsCount);
  emit finished(palette);
}

void FindPaletteThread::set(const QImage &image, int colorsCount)
{
  m_image = image;
  m_colorsCount = colorsCount;
}

