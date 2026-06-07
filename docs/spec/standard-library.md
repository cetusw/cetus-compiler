# Standard Library

## Содержание

- [Статус](#статус)
- [Регистрация](#регистрация)
- [Вывод](#вывод)
- [Длина](#длина)
- [Ввод](#ввод)
- [`printf` compatibility](#printf-compatibility)
- [Зарезервированные имена](#зарезервированные-имена)
- [Будущие модули](#будущие-модули)

## Статус

Минимальная стандартная библиотека сейчас регистрируется VM как набор native-функций.

Функции видны semantic analyzer как `BUILTIN_FUNCTION`, поэтому пользовательский код не может переобъявлять их имена как функции, параметры или переменные.

## Регистрация

VM создаётся со стандартной библиотекой через `NativeRegistry`.

Минимальный набор native-функций:

- `print(value)`;
- `println(value)`;
- `len(value)`;
- `readString()`;
- `readInt()`;
- `readFloat()`;
- `readBool()`.

## Вывод

### `print(value)`

Печатает значение без перевода строки.

Поддерживаемые типы аргумента:

- `int`;
- `float`;
- `bool`;
- `string`.

Возвращает `void`.

Пример:

```cetus
func main() {
    print("a");
    print("b");
}
```

Ожидаемый вывод: `ab`.

### `println(value)`

Печатает значение и добавляет перевод строки.

Поддерживаемые типы аргумента:

- `int`;
- `float`;
- `bool`;
- `string`.

Возвращает `void`.

Пример:

```cetus
func main() {
    println(1);
    println("ok");
}
```

Ожидаемый вывод:

```text
1
ok
```

## Длина

### `len(value)`

Сейчас поддерживает только `string`.

Сигнатура:

```text
len(string) int
```

Пример:

```cetus
func main() {
    println(len("abc"));
}
```

Ожидаемый вывод: `3`.

Позже `len` должен быть расширен для массивов и других коллекций.

## Ввод

Input native-функции читают значение из `stdin` и возвращают типизированное значение языка.

`readString`, `readInt`, `readFloat` и `readBool` являются временным минимальным API до появления ref-параметров.

Целевая Go-like модель ввода:

```cetus
func main() {
    var value int;
    scan(&value);
    println(value);
}
```

В этой модели тип чтения определяется destination-переменной, а не именем функции. Переход к `scan/read` должен быть выполнен после реализации ссылок/ref-параметров.

### `readString()`

Сигнатура:

```text
readString() string
```

Читает следующий whitespace-delimited token. При EOF возвращает пустую строку.

### `readInt()`

Сигнатура:

```text
readInt() int
```

Читает `int`. При ошибке парсинга возвращает `0`.

### `readFloat()`

Сигнатура:

```text
readFloat() float
```

Читает `float`. При ошибке парсинга возвращает `0.0`.

### `readBool()`

Сигнатура:

```text
readBool() bool
```

Читает token. Значения `true` и `1` интерпретируются как `true`; остальные значения интерпретируются как `false`. При EOF возвращает `false`.

Пример:

```cetus
func main() {
    value := readInt();
    println(value);
}
```

Если во входном потоке находится `42`, ожидаемый вывод: `42`.

## `printf` compatibility

`printf(value)` сохранён как compatibility alias для `println(value)`.

Это не C-like `printf`: форматные строки и несколько аргументов не поддерживаются.

Поддерживаемые типы аргумента такие же, как у `println`:

- `int`;
- `float`;
- `bool`;
- `string`.

Codegen резолвит вызов `printf(value)` в runtime-вызов `println(value)`.

Пример:

```cetus
func main() {
    printf(1);
}
```

Эквивалентно:

```cetus
func main() {
    println(1);
}
```

## Зарезервированные имена

Имена стандартной библиотеки являются зарезервированными callable-именами.

Запрещено объявлять пользовательские функции, параметры или переменные с именами:

- `print`;
- `println`;
- `printf`;
- `len`;
- `readString`;
- `readInt`;
- `readFloat`;
- `readBool`.

## Будущие модули

> TODO
>
> Требуется определить базовые модули: ввод-вывод, коллекции, строки, математика, тестирование.
