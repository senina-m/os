# Символьный драйвер

## Задание
Разработать комплекс программ на пользовательском уровне и уровне ярда, который собирает информацию на стороне ядра и передает информацию на уровень пользователя, и выводит ее в удобном для чтения человеком виде. Программа на уровне пользователя получает на вход аргумент(ы) командной строки (не адрес!), позволяющие идентифицировать из системных таблиц необходимый путь до целевой структуры, осуществляет передачу на уровень ядра, получает информацию из данной структуры и распечатывает структуру в стандартный вывод. Загружаемый модуль ядра принимает запрос через указанный в задании интерфейс, определяет путь до целевой структуры по переданному запросу и возвращает результат на уровень пользователя.

Интерфейс передачи между программой пользователя и ядром и целевая структура задается преподавателем.

Интерфейс передачи может быть один из следующих:

* syscall - интерфейс системных вызовов.
* ioctl - передача параметров через управляющий вызов к файлу/устройству.
* procfs - файловая система /proc, передача параметров через запись в файл.
* debugfs - отладочная файловая система /sys/kernel/debug, передача параметров через запись в файл.

### Целевая структура может быть задана двумя способами:

1. Именем структуры в заголовочных файлах Linux
2. Файлом в каталоге /proc. В этом случае необходимо определить целевую структуру по пути файла в /proc и выводимым данным.


# Решение
## Мой вариант: ioctl: thread_group_cputimer, dentry

В файле [driver.c](./src/driver.c) лежит код драйвера

В файле [user.c](./src/user.c) лежит код программы запускаемой на юзерспейсе

В файле [heder.h](./src/heder.h) лежат общие для них структуры. 


## Как запустить программы?

Нужно командой ```make``` собрать его, 
командой ```sudo insmod driver.ko``` загрузить его в ядро. 

Далее лог программы можно читать из кольцевого буфера ядра с помощью команды ```dmesg```.

На юзерспейсе программа собирается командой ```gcc user.c -o u```

Запустить её можно так ```./u pid path```, где ```pid``` это число-pid процесса, по которому мы будем искать, структуру ```thread_group_cputimer```, а ```path``` -- путь до файла для которого мы будем искать структуру ```dentry```.

Не удивляйтесь, что значения полей структуры ```thread_group_cputimer``` почти всегда нулевые. Они подсчитываются на краткое время и поймать момент, когда они отличны от нуля довольно сложно. 

# Выводы

В этой лабораторной работе я познакомилась с тем, как разрабатывают символьные драйверы под линукс. Вернее даже узнала, что это такое и как оно работает).
Также узнала как пользоваться системным вызовом ioctl чтобы передавать данные из пользовательской программы на kernel space.
