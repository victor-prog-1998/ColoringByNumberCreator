# Приложение для создания раскрасок по номерам
Позволяет создавать раскраски из фотографий и изображений
## Создание раскраски
Процесс состоит из следующих этапов:
- **Загрузка изображения**
- **Задание палитры изображения**
  >*Палитра* представляет собой набор цветов, на основе которого создается постеризованное изображение. 
  Палитра может быть задана вручную, либо быть рассчитана автоматически.
  - ***Ручное задание палитры***. Пользователь самостоятельно составляет палитру из произвольных цветов.
  - ***Автоматический расчёт***. Пользователь задает необходимое число цветов в палитре, после чего алгоритм рассчитывает оптимальную палитру для данного изображения и указанного числа цветов.

- **Постеризация изображения**
  >*Постеризованное изображение* представляет собой исходное изображение, цветовая палитра которого ограничена указанной палитрой.
-  **Коррекция постеризованного изображения** *(при необходимости)*
   - ***Замена цвета***. Заменяются цвета всех пикселей, имеющих указанный цвет, на новый цвет.
   - ***Заливка областей***. Пользователь заливает выбранную область выбранным цветом.
   - ***Рисование пикселей***. Пользователь имеет возможность рисования пикселей произвольного цвета на постеризованном изображении.
- **Получение результатов**
  >Для получения результатов используется постеризованное изображение. Если требуется масштабирование (см. раздел "Настройки"), то постеризованное изображение масштабируется перед созданием раскраски.
   - ***Раскраска по номерам***. Главное изображение. Представляет собой белое изображение с нанесенными контурами. Во все контура, за исключением слишком мелких, вписаны числовые метки цветов. 
   - ***Готовая раскраска***. Вспомогательное изображение, представляющее собой раскраску по номерам после раскрашивания. В первую очередь, полезно при раскрашивании очень мелких контуров, в которые по причине их размера не удалось вписать числовую метку цвета. По их цветам на готовом изображении можно понять, какой цвет необходимо использовать. Для удобства восприятия на данное изображение числовые метки не наносятся.
   - ***Легенда рскраски***. Представляет собой таблицу с набором цветов, которые необходимы для раскраски. Каждому цвету поставлена в соответствие числовая метка.
## Сохранение результата
При сохранении результата пользователь указывает путь. По указанному пути создаётся папка с результатами:
- Раскраска по номерам
- Готовая раскраска
- Легенда раскраски

При сохранении имеется возможность нарезать раскраску по номерам на фрагменты. Это удобно для печати раскраски по частям и дальнейшего склеивания. Пользователь задаёт степень нарезки по вертикали и горизонтали. 
В папке с результатами создаётся папка "tiled", внутри которой лежат изображения с фрагментами раскраски. 
Каждое изображение имеет имя "Буква-Цифра.png", например, "А-3.png", "D-9.png". Буквы начитаются с "А" и соответсвуют строке фрагмента. Цифры начинаются с "1" и соответствуют столбцу. Например, "C-4.png" — фрагмент на пересечении третьей строки и четвертого столбца. 
>Все фрагменты имеют одинаковое разрешение. Даже если раскраску не удаётся разделить на равные части, крайние фрагменты с меньшим размером будут дополнены до размеров остальных фрагментов. Это сделано для того, чтобы при печати всё фрагменты гарантированно имели одинаковый масштаб. 

## Настройки
Сохраняются при выходе из программы. Хранятся в файле Configs.json.
В настройках есть возможность задать следующие параметры:
- **Цвет контуров и цифр раскраски**
- **Детализация** (высокая либо упрощённая)

  Для упрощённой детализации при создании раскраски отбрасывается большее количество мелких контуров, чем при высокой. 
- **Масштабирование**

  Применяется к постеризованному изображению, на основе которого создаётся раскраска. Есть возможность задать коэффициент как 2х, 3х, 4х, 6х, 8х, 9х или 12х. Позволяет избавиться от очень мелких контуров за счёт увеличения размеров изображения. 
  >Особенность масштабирования в том, что изображение не просто растягивается в несколько раз. К нему применяются алгоритмы масштабирования пиксельной графики. Благодаря этому не возникает ступенчатости при растяжении диагональных линий. Палитра при этом сохраняется, так как размытия не происходит.При включённом масштабировании рекомендуется отключать высокую детализацию, так как алгоритмы масштабирования могут вызывать появление побочных тонких линий. Кроме того, за счёт масштабирования в раскраске сохраняется максимальное число контуров даже при упрощенной детализации. 
