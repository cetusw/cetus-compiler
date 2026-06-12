# Functions

## Содержание

- [Объявление функции](#объявление-функции)
- [Параметры](#параметры)
- [Возвращаемый тип](#возвращаемый-тип)
- [Return](#return)
- [Entry point](#entry-point)
- [Методы](#методы)
- [Вызовы](#вызовы)
- [Multiple Return](#multiple-return)
- [Связь с тестами](#связь-с-тестами)

## Объявление функции

Функция объявляется оператором `func`.

```cetus
func main() {
}

func answer() int {
    return 42;
}

func add(a int, b int) int {
    return a + b;
}
```

Поддерживаются объявления без параметров и с параметрами.

> TODO
>
> Требуется добавить overload и export-модель.

## Параметры

Параметр задаётся именем и типом:

```cetus
func printFlag(flag bool) {
    printf(flag);
}
```

Параметры доступны внутри тела функции как локальные переменные. Повтор имени параметра в одной функции является семантической ошибкой.

Параметр может иметь pointer type. Синтаксис намеренно близок к Go: в сигнатуре используется `*T`, а на стороне вызова явно передаётся адрес через `&value`, если передаётся value типа `T`.

```cetus
func inc(value *int) {
    // mutating semantics defined by pointer model
}

func main() {
    value := 1;
    inc(&value);
    println(value);
}
```

Если аргумент уже имеет тип `*T`, повторный `&` не требуется:

```cetus
node := NewNode();
UseNode(node);
```

На уровне language contract pointer parameter означает обычный typed argument, а не специальный ref-slot API frontend-а. Внутренний runtime-механизм может временно использовать reference object, но пользовательский контракт описывается через `*T`.

Вызов pointer-параметра без `&` или с адресом временного выражения является семантической ошибкой:

```cetus
func inc(value *int) {
    value = value + 1;
}

func main() {
    inc(value);     // semantic error
    inc(&(1 + 2));  // syntax/semantic error
}
```

Mutating semantics pointer-параметра определяется target type и runtime pointer model. Compiler не должен допускать pointer target с невалидным lifetime.

## Возвращаемый тип

Если возвращаемый тип не указан, функция имеет тип возврата `void`.

```cetus
func log() {
    return;
}
```

Если тип указан, `return` со значением должен возвращать совместимое значение.

```cetus
func value() int {
    return 1;
}
```

Функция может возвращать несколько значений:

```cetus
func pop() (int, bool) {
    return 0, false;
}
```

Multiple return задаёт упорядоченный список результатов функции. Это часть function signature, а не отдельный aggregate type пользователя.

## Return

`return;` допустим только в функции с типом возврата `void`.

`return expr;` допустим только в функции с не-`void` типом возврата, если тип выражения совпадает с типом возврата.

Для функции с несколькими результатами:

- `return` должен вернуть то же количество значений, что и в сигнатуре;
- тип каждого возвращаемого значения должен совпадать с соответствующим типом результата;
- arity mismatch должен быть semantic error до codegen/runtime.

`return` вне функции является семантической ошибкой.

Функция с не-`void` типом возврата должна возвращать значение на всех путях выполнения.

```cetus
func value(flag bool) int {
    if flag {
        return 1;
    } else {
        return 2;
    }
}
```

`if` без `else` не гарантирует возврат значения:

```cetus
func value(flag bool) int {
    if flag {
        return 1;
    }
}
```

Оператор после `return` в том же блоке недостижим:

```cetus
func value() int {
    return 1;
    printf(2);
}
```

## Entry point

Точка входа программы задаётся функцией `main`.

```cetus
func main() {
}
```

`main` должна быть функцией верхнего уровня, не должна иметь параметров и должна иметь тип возврата `void`.

Следующие объявления являются семантическими ошибками:

```cetus
func main(argc int) {
}

func main() int {
    return 0;
}
```

## Методы

Метод объявляется как функция с receiver перед именем:

```cetus
type Point struct {
    x int;
    y int;
}

func (p Point) Sum() int {
    return p.x + p.y;
}
```

Receiver доступен внутри тела метода как локальная переменная.

Метод вызывается через dot syntax:

```cetus
p.Sum()
```

Semantic analyzer хранит методы в таблице методов соответствующего типа. Codegen генерирует метод как обычную функцию с qualified name, например `Point.Sum`, а method call передаёт receiver первым аргументом.

Pointer receiver объявляется через `*T`:

```cetus
func (p *Point) Move(dx int) {
    p.x = p.x + dx;
}
```

Вызов остаётся Go-like:

```cetus
p.Move(4);
```

`&p` на стороне вызова не пишется. Semantic analyzer видит pointer receiver в method table и применяет pointer dispatch rules:

- `p.Method()` допустим, если `p` имеет тип `*T`;
- `p.Method()` допустим, если `p` имеет тип `T` и является addressable;
- вызов на не-addressable temporary является semantic error.

Примеры:

```cetus
point.Move();        // ok, если point: T и addressable
stack.Push(10);      // ok, если stack: *Stack
MakePoint().Move();  // semantic error
```

## Вызовы

Функция вызывается по имени. Аргументы указываются в порядке объявления параметров.

```cetus
func add(a int, b int) int {
    return a + b;
}

value := add(1, 2);
```

Вызов функции без аргументов записывается с пустым списком аргументов:

```cetus
func answer() int {
    return 42;
}

value := answer();
```

Количество аргументов должно совпадать с количеством параметров. Тип каждого аргумента должен совпадать с типом соответствующего параметра.

Для pointer parameter или pointer receiver:

- value типа `T` передаётся как `&value`, если нужен `*T`;
- value типа `*T` передаётся как обычный pointer argument;
- temporary value нельзя использовать там, где нужен адрес стабильного объекта.

Идентификатор переменной не может использоваться как вызываемая функция.

Функции верхнего уровня регистрируются до проверки тел функций и остальных операторов. Поэтому функция может быть вызвана до своего объявления в исходном файле.

```cetus
value := add(1, 2);

func add(a int, b int) int {
    return a + b;
}
```

Рекурсивные и взаимно рекурсивные вызовы разрешены, если сигнатуры функций корректны.

## Multiple Return

Multiple return values допускаются в следующих контекстах:

- в `return`;
- в multiple assignment или multiple declaration;
- как expanded arguments в function or native call, если arity соответствует expected argument positions.

Примеры:

```cetus
value, ok := stack.Pop();
return value, true;
println(stack.Pop());
```

Multiple return value нельзя неявно сохранять в одну переменную обычного типа:

```cetus
value := stack.Pop();  // semantic error
```

Если контекст ожидает один аргумент или одно значение, multiple-return function call не сворачивается автоматически.

## Связь с тестами

Публичные функции должны быть доступны test discovery. В целевой модели компилятор должен уметь определить, какие публичные функции покрыты тестами.
