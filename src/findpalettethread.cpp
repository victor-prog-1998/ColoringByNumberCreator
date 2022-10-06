#include "include/findpalettethread.h"
#include "include/algorithms.h"

FindPaletteThread::FindPaletteThread(QObject* parent):
    QThread(parent)
{
}

void FindPaletteThread::run()
{
  auto palette = Algorithms::findOptimalPalette(mImage, mColorsCount);
  emit finished(palette);
}

void FindPaletteThread::set(const QImage &image, int colorsCount)
{
  mImage = image;
  mColorsCount = colorsCount;
}

