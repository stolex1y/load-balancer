# Load balancer

Сетевой балансировщик нагрузки.

## Описание задачи

Балансировщик нагрузки принимает UDP-датаграммы с определенного порта и перенаправляет их на один из нескольких
серверов, таким образом, чтобы нагрузка на узлы распределялась равномерно.
Кроме того, необходимо ограничивать нагрузку так, чтобы общее количество датаграмм проходящих через балансировщик не
превышало N в
секунду, лишние датаграммы отклоняются.

## Описание реализации

Для работы с сетью используются сокеты POSIX API, причем для более удобной работы
реализована [обертка](src/socket_wrapper) в стиле ООП.

Сам же балансировщик нагрузки реализован с использованием простейшего алгоритма `Round-robin`, то есть запросы
распределяются по серверам последовательно друг за другом. Кроме того, есть возможность настройки некоторых параметров
через конфигурационный файл.
Для этого [специальный класс](src/load_balancer/configuration/configuration.h) считывает из файла config.properties
свойства. Пример этого файла можно посмотреть [здесь](config.properties), в нем указаны все возможные конфигурируемые
параметры.

| Параметр        | Значение по умолчанию | Описание                                                                              |
|-----------------|-----------------------|---------------------------------------------------------------------------------------|
| `max_rps`       | 1000                  | Максимальное количество запросов в секунду.                                           |
| `servers`       | -                     | Конечные точки серверов через запятую. Например: 192.168.0.10:1001,192.168.0.11:1001. |
| `receiver_port` | 10000                 | Порт балансировщика, на который принимаются входящие запросы.                         |
| `sender_port`   | 10001                 | Порт балансировщика, с которого перенаправляются принятые запросы.                    |

В проекте используется `Google Test` для написания модульных тестов.

## Сборка и запуск

Можно воспользоваться предварительно собранными [файлами](https://github.com/stolex1y/load-balancer/releases/latest).
Там также можно найти документацию.

Для сборки в ручную можно использовать следующие команды:

```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j`nproc --all` -t load-balancer-runnable
./build/bin/load_balancer/load-balancer-runnable 
```

Как было описано выше, можно задать конфигурационный файл `config.properties`, который должен лежать в одной
директории с исполняемым файлом.

Для запуска тестов можно использовать следующие команды:

```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cd build
cmake --build . -j`nproc --all`
ctest --timeout 10 --output-on-failure --schedule-random # unit tests
cmake --build . -t load-balancer-test-runnable-memcheck # valgrind
```
