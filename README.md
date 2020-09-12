# Типы в MPI: доклад

Данный доклад был подготовлен в рамках курса архитектуры компьютеров на
факультете прикладной математики и информатики БГУ в 2020 году.

Предполагается, что текст доклада ниже изучается совместно с запуском
соответствующих примеров. Все примеры написаны на языке C с использованием
стандарта C11.

[Ссылка на стандарт MPI 3.1](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report.pdf).

*Замечание*: в системном программировании обычно принято проверять коды возврата
вызываемых функций, чтобы узнавать об ошибках. Поскольку стандартный обработчик
ошибок MPI завершает работу программы прежде, чем происходит возврат из
функции, во всех примерах соответствующие коды возврата не обрабатываются.
В случае, если вы захотите переопределить коды ошибок, очевидно, вам нужно
будет позаботиться и об обработке кодов возврата.

# Среда для компиляции и выполнения примеров

В качестве среды для компиляции и выполнения представленных примеров мной
использовался дистрибутив Ubuntu 20.04, система сборки CMake, компилятор
[gcc-10](https://packages.ubuntu.com/focal/gcc-10), открытая реализация
протокола MPI ([OpenMPI](https://www.open-mpi.org/)) и среда разработки CLion.

Для запуска примеров на своем компьютере в таком же либо похожем окружении
достаточно выполнить следующие действия:
* Установить пакеты OpenMPI с помощью используемого вами пакетного менеджера:
`sudo apt install libopenmpi-dev`
* В настройках CLion (`Settings - Build, execution, deployment - Toolchains`)
добавить компилятор MPI. Путь к компилятору C - `/usr/bin/mpicc`, к компилятору
C++ - `/usr/bin/mpic++`.
* Склонировать данный репозиторий и открыть проект в CLion (необходимые
настройки будут загружены из предоставленного CMake-скрипта).
* Выбрать добавленный ранее компилятор в качестве основного для проекта,
переместив его в списке доступных в самый верх.

Для запуска скомпилированных исполняемых файлов необходимо использовать утилиту
`mpirun` (символическая ссылка на нее доступна по пути `/usr/bin/mpirun`). Это
можно делать вручную в терминале, но значительно удобнее будет настроить
автоматический запуск с нужными аргументами в CLion. Для этого достаточно
раскрыть список конфигураций запуска справа вверху, кликнуть `Edit
configurations`, в качестве `Executable` указать путь к `mpirun`, в качестве
`Program arguments` - нужные аргументы. Например, для запуска исполняемого
файла `test`, который доступен по пути
`/home/user/project/cmake-build-release/test`, на четырех процессах, можно
использовать аргументы:

`-n 4 /home/user/project/cmake-build-release/test`

При желании данные примеры можно запустить и под Windows, но в этом случае вам
придется провести настройку окружения самостоятельно.

# Выполнение примеров в GitHub Actions

[![Actions Status](https://github.com/anevero/mpi_types_report/workflows/mpi_types_report_ci/badge.svg)](https://github.com/anevero/mpi_types_report/actions)

Если вы не хотите настраивать окружение локально, вы можете просматривать
результаты выполнения примеров в консоли GitHub Actions. Для этого пройдите
по [данной ссылке](https://github.com/anevero/mpi_types_report/actions?query=workflow:mpi_types_report_ci),
выберите первый в списке запуск, далее в меню слева переключитесь на стадию
`build_and_run`, в терминале раскройте шаги вида `Run example X`.

При желании вы, конечно же, можете создавать собственные скрипты GitHub
Actions, которые будут выполнять какие-либо дополнительные действия.
 
# Доклад

Примеры отсортированы в порядке их рассмотрения. Сначала мы рассмотрим, как
можно пересылать между процессами объекты примитивных встроенных типов, затем
перейдем к массивам из таких объектов. После этого мы изучим способы
пересылки собственных структур и закончим рассмотрением возможности пересылки
объектов классов C++.

## 1. Отправка объектов примитивных типов

Просто для того, чтобы вспомнить синтаксис основных операций MPI, рассмотрим
простейший пример отправки объектов примитивных типов.

В приведенном примере нулевой процесс отправляет во все доступные процессы
(в том числе в себя самого) одно целое число, соответствующее номеру процесса.
Каждый процесс получает свое число и выводит его на экран.

В данном докладе фактически мы будем работать с тремя первыми аргументами
функций `MPI_Send` и `MPI_Recv`. Первым аргументов идет указатель на буфер,
откуда надо взять (либо куда надо записать) данные, далее идет количество
элементов, которые участвуют в передаче, после чего с помощью макросов MPI
указывается тип этих элементов.

Стоит обратить внимание на то, что второй аргумент - это именно количество
элементов, а не их суммарный размер в байтах (как это принято в C).

Если посмотреть, во что раскрывается встроенный макрос `MPI_INT`, можно
заметить, что он раскрывается в некоторую структуру типа `MPI_Datatype`.
Логично предположить, что при работе со своими типами нам нужно будет
производить какие-либо действия с подобной структурой. Так и есть.

Подобным образом можно передавать объекты многих примитивных типов. Приведем
таблицу соответствий макросов MPI и типов C:

<table>
    <tr>
        <td>MPI_CHAR</td>
        <td>char</td>
        <td>MPI_UNSIGNED_CHAR</td>
        <td>unsigned char</td>
    </tr>
    <tr>
        <td>MPI_SHORT</td>
        <td>short</td>
        <td>MPI_UNSIGNED_SHORT</td>
        <td>unsigned short</td>
    </tr>
    <tr>
        <td>MPI_INT</td>
        <td>int</td>
        <td>MPI_UNSIGNED</td>
        <td>unsigned int</td>
    </tr>
    <tr>
        <td>MPI_LONG</td>
        <td>long</td>
        <td>MPI_UNSIGNED_LONG</td>
        <td>unsigned long</td>
    </tr>
    <tr>
        <td>MPI_LONG_LONG</td>
        <td>long long</td>
        <td>MPI_UNSIGNED_LONG_LONG</td>
        <td>unsigned long long</td>
    </tr>
</table>

<table>
    <tr>
        <td>MPI_INT8_T</td>
        <td>int8_t</td>
        <td>MPI_UINT8_T</td>
        <td>uint8_t</td>
    </tr>
    <tr>
        <td>MPI_INT16_T</td>
        <td>int16_t</td>
        <td>MPI_UINT16_T</td>
        <td>uint16_t</td>
    </tr>
    <tr>
        <td>MPI_INT32_T</td>
        <td>int32_t</td>
        <td>MPI_UINT32_T</td>
        <td>uint32_t</td>
    </tr>
    <tr>
        <td>MPI_INT64_T</td>
        <td>int64_t</td>
        <td>MPI_UINT64_T</td>
        <td>uint64_t</td>
    </tr>
</table>

<table>
    <tr>
        <td>MPI_FLOAT</td>
        <td>float</td>
    </tr>
    <tr>
        <td>MPI_DOUBLE</td>
        <td>double</td>
    </tr>
    <tr>
        <td>MPI_LONG_DOUBLE</td>
        <td>long double</td>
    </tr>
</table>

<table>
    <tr>
        <td>MPI_LONG_INT</td>
        <td>struct { long, int }</td>
    </tr>
    <tr>
        <td>MPI_DOUBLE_INT</td>
        <td>struct { double, int }</td>
    </tr>
    <tr>
        <td>MPI_SHORT_INT</td>
        <td>struct { short, int }</td>
    </tr>
    <tr>
        <td>MPI_2INT</td>
        <td>struct { int, int }</td>
    </tr>
</table>

<table>
    <tr>
        <td>MPI_BYTE</td>
        <td>...</td>
    </tr>
</table>

Отдельно выделим тип `MPI_BYTE`. Он предназначен для работы с сырыми
нетипизированными бинарными данными. Его явного аналога в C нет, обычно вместо
него используются `char` или `unsigned char`. Но по соображениям совместимости
между различными архитектурами при пересылке сырых бинарных данных лучше
использовать не `MPI_CHAR`, а именно `MPI_BYTE`.

## 2. Отправка массивов объектов примитивных типов

Отправлять массивы объектов примитивных типов можно почти так же, как и
единичные объекты. Для этого нужно использовать второй аргумент функций
`MPI_Send` / `MPI_Recv`, указывая с его помощью размер массива. Первым
аргументом, соответственно, должен идти указатель на этот массив.

Логика абсолютно такая же, как и в предыдущем примере.

## 3. Отправка структур в виде сырых байтов

Очевидно, что отправкой лишь примитивных типов в реальной программе мы многого
не достигнем. Рассмотрим способы отправки собственных структур с полями
произвольных примитивных типов.

Структуры, которые мы будем использовать далее в примерах, определены в
заголовочном файле `structs.h`.

Первое, что приходит в голову, - мы можем отправлять структуры просто в виде
сырых байт, а на принимающей стороне конвертировать байты обратно в объекты.
Сделаем это. Вы можете запустить третий пример на своем компьютере и
убедиться, что все работает.

Тем не менее, делая так, мы забыли об одной важной вещи. Фактически мы
предполагаем, что во всех процессах размещение структур в памяти будет
одинаковым. Это верно, когда мы запускаем нашу программу на локальном
компьютере или даже в сети однородных компьютеров. Но механизмы MPI
предполагают работу в том числе в неоднородных сетях, то есть на компьютерах
с различной архитектурой. Будет ли наше допущение насчет размещения структур
в памяти верным в этом случае?

## 4. Исследование размещения структур в памяти

В четвертом примере у нас есть макрос, в который можно передать название
нашей структуры, и который выведет ее размер в байтах, а также размеры и
отступы от начала адреса объекта всех ее полей. Воспользуемся им для нашей
структуры `Struct`.

Теоретически размер структуры должен быть равен 10 байтам (два 32-битных целых
числа и 2 8-битных символа). На практике же в нашем окружении размер будет
равен 16 байтам. Откуда взялись лишние 6 байт?

Присмотревшись к смещениям, мы можем заметить, что между первым символом и
вторым числом у нас есть незанятое пространство размером 3 байта. Аналогичное
пространство есть в самом конце структуры, после второго символа.

Для понимания происходящего следует вспомнить, как работает адресация памяти.
Процессору удобнее всего работать с адресами, которые выровнены по тому или
иному модулю степени двойки. Именно для этого компиляторы добавляют между
полями структур (и классов в C++) так называемые `padding bytes` - пустые
байты, которые позволяют добиться выравнивания адресов полей. Отсутствие
выравнивания может в разы уменьшить производительность работы с памятью, в
некоторых случаях это даже может вызвать ошибки шины и другие подобные вещи.

В нашем контексте важно то, что механизмы выравнивания отличаются на разных
архитектурах (и зависят в том числе от компилятора). Соответственно, одна и та
же структура может по-разному располагаться в памяти в процессах, запущенных
на компьютерах разной архитектуры. Значит, рассмотренный нами пример пересылки
структур в общем случае будет работать некорректно.

Для решения данной проблемы можно попробовать самостоятельно управлять
выравниванием полей структур. Два способа сделать это присутствуют в примерах.

Во-первых, при создании структуры мы можем попросить компилятор отключить
выравнивание для нее с помощью атрибута `packed` (синтаксис будет различаться
в разных компиляторах). Нетрудно убедиться, что соответствующая структура
`PackedStruct` будет занимать 10 байт. Тем не менее, как уже упоминалось выше,
подобное решение может сильно снизить производительность.

Во-вторых, можно настроить параметры выравнивания с помощью ключевого слова
`alignas`. Например, в `ManuallyAlignedStruct` каждое поле будет выравниваться
по границе в 8 байт. Размер структуры в итоге будет равен 32 байтам.
Производительность не пострадает, но мы будем использовать сильно больше
памяти.

Чтобы избежать подобных решений, которые требуют тонкой работы с атрибутами и
знания особенностей выравнивания на всех архитектурах, где теоретически может
работать программа, для пересылки структур мы можем использовать механизмы MPI.

## 5. Определение типа структур средствами MPI

Как уже упоминалось выше, фактически за `MPI_INT` и другими подобными макросами
скрываются структуры типа `MPI_Datatype`. Данный тип содержит ряд полей, с
помощью которых можно описать любой тип данных так, чтобы MPI мог с ним
работать (в том числе пересылать). В числе этих полей:
* Количество полей в описываемой структуре данных. Например, если это просто
переменная типа `int`, оно равно 1, а если это структура из трех полей - 3.
* Массив с количеством элементов в каждом поле. Подразумевается, что полями
могут быть как единичные переменные, так и массивы. Если поле - обычная
переменная, то соответствующее количество равно 1, если поле - это массив, то
следует указать его размер (в элементах, не в байтах!).
* Массив отступов (смещений от начала структуры) каждого из полей. Это ровно
то, что мы получали в предыдущем примере с помощью макроса `offsetof`.
* Массив типов полей.

С помощью этой информации можно описать любую структуру данных в C. Синтаксис
описания своего типа можно рассмотреть в примере. Сначала создаются все нужные
массивы с данными и переменная типа `MPI_Datatype` (ее имя далее будет
использоваться в качестве типа, то есть будет чем-то вроде аналога `MPI_INT` 
для нашей структуры). Затем все параметры передаются в функцию
`MPI_Type_create_struct`, которая записывает нужную информацию в переменную
`MPI_Datatype`. Далее необходимо "закоммитить" тип с помощью функции
`MPI_Type_Commit`, и после этого его можно использовать наравне со встроенными
типами. Обратите внимание, что когда тип становится не нужен, нужно отменить
его коммит с помощью функции `MPI_Type_free`.

Отметим, что все действия по определению типа должны проходить в каждом
процессе (а не только, например, в нулевом). Очевидно, что на разных
архитектурах описание типа может отличаться (банально по причинам различного
выравнивания), и каждый процесс должен проинициализировать тип под себя.
Конвертацией же представления типа при его пересылке между разными процессами
будет заниматься MPI.

Если запустить пятый пример, в котором происходит пересылка одного объекта
своего типа, можно убедиться, что все работает.

## 6. Подводные камни

Казалось бы, все работает, можно заканчивать рассмотрение механизмов
определения своих типов в MPI. Но на всякий случай попробуем сэмулировать
работу программы на процессорах с разной архитектурой, где используется
разное выравнивание, и наша структура размещена в памяти по-разному.

В шестом примере мы имеем два процесса. Первый использует структуру `Struct`
со стандартным для нашего окружения выравниванием, второй - структуру
`ManuallyAlignedStruct` с иными параметрами выравнивания (будем считать, что
столкнулись с ними на какой-то архитектуре). В каждом процессе создается
соответствующий тип данных, который соответствующим образом инициализируется.

Чтобы использовать все возможности MPI, передадим из нулевого в первый процесс
не просто один объект нашего типа, а массив из двух (одинаковых) объектов
нашего типа.

Запустим программу. Можно обнаружить, что хотя первый объект в массиве был
передан корректно, второй при передаче был поврежден, и на экран выводится
какой-то мусор. Почему так произошло, ведь мы создали свой собственный тип
по всем правилам?

## 7. Пустые байты в конце структуры

Для того, чтобы понять, что не так в нашей текущей реализации передачи объектов
своего типа, нужно подумать, как эта передача работает внутри. По
предоставленной нами при конструировании типа информации MPI узнает, где в
памяти лежат поля структуры, и сколько памяти они занимают. При пересылке
структуры MPI отправляет ее в сыром виде, а после при необходимости передвигает
поля так, чтобы они соответствовали формату, используемому на стороне приема.
Что же может пойти не так?

Нетрудно заметить, что информации, которую мы предоставили MPI при
конструировании типа, недостаточно для того, чтобы определить, где в памяти
находится конец структуры. Очевидно, что конец структуры не всегда совпадает
с концом последнего ее поля по причинам выравнивания (в конце компилятор
может добавить пустые байты). Соответственно, когда мы передаем массив
элементов нашей структуры, MPI по ошибке накладывает начало второго элемента
на конец первого, и при чтении данных мы получаем мусор.

Чтобы избежать этого, мы можем отредактировать наш созданный тип и явно сказать
MPI, сколько пустых байт находится перед первым полем и после последнего. Для
этого используется функция `MPI_Type_create_resized`, которая принимает
исходный тип (который мы уже создали), новую нижнюю и верхнюю границы, а также
указатель на переменную `MPI_Datatype`, куда надо записать новый тип с
"отредактированными" характеристиками. Чтобы разобраться с пустыми байтами в
конце структуры, достаточно оставить нижнюю границу равной 0, а верхнюю сделать
равной `sizeof(Struct)`.

Отметим, что коммитить временный тип, который используется при создании
полноценного (с информацией о границах) необязательно.

Теперь можно запустить седьмой пример и убедиться, что все работает ровно так,
как этого можно ожидать. Теперь у MPI есть вся нужная информация о структуре,
что позволяет ему корректно изменять ее представление в случае, если на стороне
отправки и приема оно различное.

## 8. Последний пример со структурой

Здесь не будет ничего нового, пример просто показывает, что редактированием
нижних и верхних границ типа мы не сломали ничего, что работало раньше 
(пересылка единичных объектов).

## 9. Создание других типов в MPI

На самом деле, MPI позволяет описывать не только структуры, но и другие типы
переменных. Три примера - с обычным массивом, одинаковыми блоками массива и
различными блоками массива - можно увидеть прямо в коде. Есть и другие
функции, подробно ознакомиться с ними можно в стандарте (если они вам
когда-либо понадобятся).

## 10. MPI и объекты C++

Если вы будете работать с MPI с использованием C++, стоит понимать, что многие
механизмы C++ сильно усложняют внутреннюю структуру соответствующих объектов,
и взаимодействие между ними и MPI (например, пересылка) сильно усложнены.
Приведем несколько примеров.

### Контейнеры из стандартной библиотеки

Очевидно, вы не сможете в явном виде отправить другому процессу объект
`std::vector`. Как известно (не углубляясь в детали), данный объект фактически
является структурой с тремя указателями - на начало вектора, на конец вектора и
на конец выделенного диапазона памяти. Если вы передадите его другому процессу
в таком виде, вы не получите ничего хорошего, так как соответствующие указатели
будут невалидными. Поэтому для пересылки `std::vector` вам придется
углубляться в его внутреннее устройство, получать указатель на внутренний
массив методом `data()` и работать именно с ним.

Такие же проблемы будут возникать и с другими контейнерами. Вам в любом случае
придется углубляться на уровень C, чтобы "подружить" эти объекты с MPI.

### Поля с нулевым размером

Предположим, у вас есть класс, а в нем есть поле какого-то типа, который не 
содержит нестатических полей. Возникает вопрос: что делать с этим полем,
сколько байт оно занимает? Ответа на этот вопрос нет, это *implementation
defined*. Современные компиляторы часто могут занулить размер этого поля, но
стандарт C++ этого не гарантирует.

В C++20 был добавлен специальный атрибут `[[no_unique_address]]`, которым можно
просигнализировать компилятору, что поле может иметь нулевой размер (а точнее,
его адрес может совпадать со следующим), но даже при использовании этого
атрибута у вас нет никаких гарантий, что компилятор его учтет (опять же, это
остается на совести разработчика компилятора).

Может показаться, что это редкая ситуация, но на самом деле примеры есть даже
в стандартной библиотеке. Например, такая логика часто есть у аллокаторов и
хэшеров.

### Части базовых классов при наследовании

Как известно, в памяти объект класса `Derived`, который отнаследован от класса
`Base`, размещен в виде двух частей: части `Base` и части `Derived`. Возникает
вопрос: как выглядит часть `Base`, когда в базовом классе нет нестатических
полей?

На самом деле, этот вопрос (в отличие от предыдущего) имеет конкретный ответ.
Стандарт C++ гарантирует, что в этом случае часть `Base` будет занимать 0 байт.
Соответствующая оптимизация известна под именем *empty base optimization*. О
ней стоит знать.

### Виртуальные функции

Как только в иерархии ваших классов появляются виртуальные функции, размещение
объектов этих классов в памяти усложняется, причем незаметно для вас.
Например, компилятор добавляет скрытые поля - указатели на виртуальную
таблицу. Так что вы просто не сможете дать MPI корректное описание полей
вашего класса.

Подобные явления теоретически могут проявляться и при виртуальном наследовании.

### Вывод

Полноценно использовать механизмы C++ при взаимодействии с MPI вряд ли выйдет,
вы просто будете углубляться на уровень C. Но вы все еще можете использовать
C++ для написания каких-то утилит, которые прямо не связаны с MPI, но позволяют
удобно обрабатывать те или иные вещи.

### Сторонние решения

Для более удобной работы с MPI из C++ можно использовать ряд сторонних
библиотек, которые упрощают сериализацию типов. Среди них:
* [Boost.MPI](https://www.boost.org/doc/libs/1_74_0/doc/html/mpi.html).
* [Google Protocol Buffers](https://developers.google.com/protocol-buffers/docs/cpptutorial).