# BrickGame Тетрис
В данном проекте реализована игра «Тетрис» на языке программирования С с использованием структурного подхода.

## Введение

Проект состоит из двух частей: библиотеки, реализующей логику работы игры, которую можно подключать к различным GUI в будущем, и терминального интерфейса. Логика работы библиотеки реализована с использованием конечных автоматов.

- Программа разработана на языке Си стандарта C11 с использованием компилятора gcc.
- Программа состоит из двух частей: библиотеки, реализующей логику игры тетрис, и терминального интерфейса с использованием библиотеки `ncurses`.
- Для формализации логики игры использован конечный автомат.
- Код библиотеки программы находится в папке `src/brick_game/tetris`.
- Код с интерфейсом программы находится в папке `src/gui/cli`.
- Сборка программы настроена с помощью Makefile со стандартным набором целей для GNU-программ: all, install, uninstall, clean, dvi, dist, test, gcov_report. 
- При написании кода придерживался Google Style.
- Обеспечено покрытие библиотеки unit-тестами, с помощью библиотеки `check`. Покрытие библиотеки с логикой игры тестами составляет более 90 процентов.
- В игре присутствовуют следующие механики:
  - Вращение фигур;
  - Перемещение фигуры по горизонтали;
  - Ускорение падения фигуры;
  - Показ следующей фигуры;
  - Уничтожение заполненных линий;
  - Завершение игры при достижении верхней границы игрового поля;
  - Подсчет очков;
  - Хранение максимального количества очков.
  - Каждый раз, когда игрок набирает 600 очков, уровень увеличивается на 1. Повышение уровня увеличивает скорость движения фигур. Максимальное количество уровней — 10
- Игровое поле соответствовует размерам игрового поля консоли — десять «пикселей» в ширину и двадцать «пикселей» в высоту.
- Подготовлена  диаграмма, описывающая используемый КА (его состояния и все возможные переходы).

Начисление очков происходит следующим образом:

- 1 линия — 100 очков;
- 2 линии — 300 очков;
- 3 линии — 700 очков;
- 4 линии — 1500 очков.
