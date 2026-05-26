# Генерация и запуск ассемблерного кода

Основной сценарий запуска находится в скрипте:

```bash
./tools/cetus.sh
```

Скрипт компилирует исходный файл Cetus в x86-64 assembly, собирает assembly через `gcc` и запускает получившуюся программу.

## Подготовка

Сначала соберите проект:

```bash
cmake --build cmake-build-debug
```

По умолчанию скрипт ожидает бинарник компилятора здесь:

```plain
cmake-build-debug/cetus
```

## Запуск файла по умолчанию

Если запустить скрипт без аргументов:

```bash
./tools/cetus.sh
```

он скомпилирует файл:

```plain
test.txt
```

Для `test.txt` скрипт создаст в текущей директории:

```plain
test.asm
test
```

После этого он запустит `./test` и выведет результат программы.

## Запуск другого файла

Можно передать путь к исходному файлу первым аргументом:

```bash
./tools/cetus.sh examples/conditional.txt
```

Имя выходных файлов берётся из имени входного файла:

```plain
conditional.asm
conditional
```

## Подробный режим

Обычный режим печатает только вывод программы. Чтобы увидеть все этапы, используйте флаг `--info`:

```bash
./tools/cetus.sh --info
```

или вместе с явным входным файлом:

```bash
./tools/cetus.sh examples/conditional.txt --info
```

В режиме `--info` скрипт печатает:

- исходный код;
- AST;
- путь к сгенерированному assembly-файлу;
- полный assembly;
- путь к исполняемому файлу;
- вывод программы.

## Пример исходного файла

```plain
a := 2;
b := 1;

if a > b {
    printf(1);
} else {
    printf(0);
}

if b > a {
    printf(0);
} else {
    if a == 2 {
        printf(1);
    } else {
        printf(0);
    }
}

printf(a > b);
```

Ожидаемый вывод:

```plain
1
1
1
```

## Что делает скрипт

Скрипт выполняет те же шаги, которые можно запустить вручную:

```bash
./cmake-build-debug/cetus --asm test.txt test.asm
gcc -no-pie -x assembler test.asm -o test
./test
```

Для просмотра AST отдельно:

```bash
./cmake-build-debug/cetus --parse-ast test.txt
```

Для проверки типов:

```bash
./cmake-build-debug/cetus --typecheck test.txt
```

Если менялась грамматика, подготовленную таблицу парсера можно обновить так:

```bash
./cmake-build-debug/cetus --parse-ast --regen-table test.txt
```
