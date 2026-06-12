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

- `print(value, ...)`;
- `println(value, ...)`;
- `len(value)`;
- `scan(&value)`.

## Вывод

### `print(value, ...)`

Печатает одно или несколько значений без перевода строки. Между аргументами печатается один пробел.

Поддерживаемые типы аргументов:

- `int`;
- `float`;
- `bool`;
- `string`.
- array/slice runtime values.

Возвращает `void`.

Пример:

```cetus
func main() {
    print("a");
    print("b");
}
```

Ожидаемый вывод: `ab`.

### `println(value, ...)`

Печатает одно или несколько значений и добавляет перевод строки. Между аргументами печатается один пробел.

Поддерживаемые типы аргументов:

- `int`;
- `float`;
- `bool`;
- `string`.
- array/slice runtime values.

Возвращает `void`.

Пример:

```cetus
func main() {
    println(1);
    println("ok");
    println("values:", []int{1, 2, 3});
}
```

Ожидаемый вывод:

```text
1
ok
values: [1, 2, 3]
```

## Длина

### `len(value)`

Поддерживает `string`, fixed array и slice.

Сигнатура:

```text
len(string) int
len([N]T) int
len([]T) int
```

Пример:

```cetus
func main() {
    println(len("abc"));
}
```

Ожидаемый вывод: `3`.

`len(array)` и `len(slice)` возвращают длину первого измерения. Для `[2][3]int` результат `len(matrix)` равен `2`.

## Ввод

Input native-функция `scan` читает значение из `stdin` и записывает его в destination-переменную.

Go-like модель ввода в текущем синтаксисе Cetus:

```cetus
func main() {
    var value int;
    scan(&value);
    println(value);
}
```

В этой модели тип чтения определяется destination-переменной, а не именем функции. `scan` принимает адрес destination-переменной, поэтому аргумент должен иметь форму `&identifier`.

### `scan(&value)`

Сигнатура:

```text
scan(value *T) void
```

Поддерживаемые destination-типы:

- `int`;
- `float`;
- `bool`;
- `string`.

Пример:

```cetus
func main() {
    var value int;
    scan(&value);
    println(value);
}
```

Если во входном потоке находится `42`, ожидаемый вывод: `42`.

## `printf` compatibility

`printf(value, ...)` сохранён как compatibility alias для `println(value, ...)`.

Это не C-like `printf`: форматные строки не поддерживаются.

Поддерживаемые типы аргумента такие же, как у `println`:

- `int`;
- `float`;
- `bool`;
- `string`.

Codegen резолвит вызов `printf(value, ...)` в runtime-вызов `println(value, ...)`.

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
- `scan`.

## Будущие модули

> TODO
>
> Требуется определить базовые модули: ввод-вывод, коллекции, строки, математика, тестирование.
