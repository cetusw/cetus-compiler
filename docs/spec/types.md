# Types

## Содержание

- [Встроенные типы](#встроенные-типы)
- [Вывод типа](#вывод-типа)
- [Truthiness](#truthiness)
- [Операции](#операции)
- [Неявное приведение числовых типов](#неявное-приведение-числовых-типов)
- [Массивы](#массивы)
- [Пользовательские типы](#пользовательские-типы)
- [Указатели и Nullable-значения](#указатели-и-nullable-значения)
- [Addressability](#addressability)
- [Struct Composite Literal](#struct-composite-literal)
- [Совместимость](#совместимость)
- [Будущие типы](#будущие-типы)

## Встроенные типы

Система типов CETUS определяет следующие базовые типы:

| Тип | Назначение |
|---|---|
| `int` | целое число |
| `float` | вещественное число |
| `bool` | логическое значение |
| `string` | строка |
| `void` | отсутствие значения у оператора |
| `error` | внутренний тип ошибки |

## Вывод типа

Оператор:

```cetus
a := 2;
```

создаёт символ `a` в текущей области видимости и выводит его тип из правой части.

Объявление:

```cetus
var city = "Moscow";
```

также выводит тип из initializer.

Операторы объявления и присваивания имеют тип `void`.

## Truthiness

`int` и `bool` являются truthy-compatible типами.

Правило:

- `0` считается ложью;
- любое ненулевое значение считается истиной;
- `false` считается ложью;
- `true` считается истиной.

Это правило применяется к:

- условию `if`;
- `!`;
- `&&`;
- `||`;
- операторам сравнения.

## Операции

Арифметические операции:

```text
+ - * /
```

требуют числовые операнды: `int` или `float`.

Оператор `+` также поддерживает конкатенацию строк, если оба операнда имеют тип `string`.

Остаток от деления:

```text
%
```

требует два `int`.

Сравнения:

```text
< <= > >= == !=
```

возвращают `bool`.

Логические операции:

```text
! && ||
```

возвращают `bool`.

Statement-конструкции `if`, присваивание и последовательность операторов имеют тип `void`. Вызов `void`-функции допустим как expression statement, но результат `void` нельзя использовать как значение.

Контексты значения не принимают `void`:

- initializer переменной;
- правая часть присваивания;
- аргумент функции;
- значение `return`;
- условие `if`;
- операнд унарного или бинарного оператора.

## Неявное приведение числовых типов

`int` и `float` являются разными типами языка и разными runtime-представлениями VM.

Runtime-представление:

- `int` хранится как `RuntimeInt`, сейчас это `int64`;
- `float` хранится как `RuntimeFloat`, сейчас это 64-bit floating point (`double`);
- `bool` хранится как `RuntimeBool`.

Арифметика использует правило числового продвижения:

| Операнды | Результат |
|---|---|
| `int op int` | `int` |
| `int op float` | `float` |
| `float op int` | `float` |
| `float op float` | `float` |

Пример:

```cetus
func main() {
    printf(1 + 2);     // int: 3
    printf(1 + 2.5);   // float: 3.5
    printf(1.5 - 1);   // float: 0.5
}
```

Если хотя бы один операнд арифметической операции имеет тип `float`, второй операнд приводится к `float` для вычисления, а результат имеет тип `float`.

Если оба операнда имеют тип `int`, операция выполняется как integer operation, а результат имеет тип `int`.

Оператор `%` не участвует в числовом продвижении. Он разрешён только для пары `int % int` и возвращает `int`.

Пример:

```cetus
func main() {
    printf(5 % 2);       // ok, int: 1
    printf(5.0 % 2.0);   // semantic error
}
```

## Массивы

Целевой синтаксис массивов описан в [Arrays](./arrays.md).

Массив имеет фиксированную длину и тип элемента:

```cetus
[3]int
[2][3]float
```

Длина является частью типа. Поэтому `[3]int` и `[4]int` являются разными типами.

Slice имеет тип элемента, но не содержит длину в типе:

```cetus
[]int
[][]float
```

`[]int` и `[3]int` являются разными типами. `TypeDescriptor` представляет fixed array как `Array(length, elementType)`, а slice как `Slice(elementType)`.

Общее поведение fixed array и slice выражается через sequence-предикат: оба типа имеют element type и поддерживают index access. Для `len` используется indexable-предикат: fixed array, slice и string.

Многомерный массив задаётся как массив массивов:

```cetus
[2][3]int
```

На текущем этапе parser и semantic analyzer понимают fixed array type и slice type в typed declarations, function parameters и return types. Runtime поддерживает default fixed array creation, array/slice literals через `ObjArray`, index reading/writing, bounds check, `len(array)` и `len(slice)`.

## Пользовательские Типы

Struct declaration создаёт пользовательский named type:

```cetus
type Point struct {
    x int;
    y int;
}
```

`Point` можно использовать как тип переменной, параметра функции, возвращаемого значения и элемента массива:

```cetus
var point Point;
var points [2]Point;
```

`TypeDescriptor` представляет такой тип как named type. Semantic analyzer разрешает имя через symbol table и требует, чтобы оно указывало на символ вида `TYPE`.

Runtime-представление struct instance хранится в `ObjStruct`. Default value для struct создаёт instance со значениями полей по умолчанию. Member access возвращает тип соответствующего поля.

Named type участвует в системе типов как отдельное имя, а не как alias встроенного типа. Поэтому:

- `Point` и `int` несовместимы;
- `Point` и `OtherPoint` несовместимы, даже если имеют одинаковый набор полей;
- поле struct и parameter function могут иметь тип named struct.

## Указатели и Nullable-значения

Pointer type записывается как `*T`:

```cetus
*int
*Point
[]*Point
```

`*T` является отдельным типовым конструктором, так же как `[]T` и `[N]T`.

Целевой контракт языка:

- `&value` возвращает значение типа `*T`, если `value` имеет тип `T` и является addressable;
- `&Point{...}` возвращает `*Point` и создаёт heap-backed instance;
- `*T` является nullable type;
- default value для `*T` равен `nil`.

Literal `nil` является отдельным значением языка и совместим только с nullable types. На текущем этапе nullable считаются:

- `*T`;
- `[]T`;
- function values, если позже они станут пользовательскими значениями.

`nil` несовместим с:

- `int`;
- `float`;
- `bool`;
- `string`;
- struct value `T`.

Указатели на текущем этапе не поддерживают:

- pointer arithmetic;
- явное разыменование через отдельный оператор `*expr`;
- сравнение порядка указателей;
- произвольное преобразование pointer types.

Неявное разыменование разрешено только в member access и method dispatch для pointer-to-struct.

## Addressability

Addressability определяет, можно ли получить стабильную ссылку на значение или передать его как mutating receiver.

Addressable выражения:

- identifier переменной;
- member access на addressable объекте;
- index access на sequence, если runtime поддерживает стабильную ссылку на элемент;
- future explicit dereference expression.

Не addressable:

- literal;
- результат арифметического выражения;
- результат comparison;
- результат function call, если он не возвращает pointer;
- временный struct literal без address-of.

Правила:

- `&expr` допустим только для addressable выражения или для composite literal, для которого язык явно разрешает heap allocation;
- pointer receiver method call с receiver типа `T` допустим только для addressable receiver;
- pointer receiver method call с receiver типа `*T` допустим как обычный value call.

## Struct Composite Literal

Struct literal создаёт значение named struct type:

```cetus
Point{
    x: 1,
    y: 2,
}
```

Field names являются обязательной частью текущего контракта. Positional struct literal не поддерживается.

Семантические правила:

- имя типа должно разрешаться в struct type;
- каждое поле должно существовать в declaration типа;
- поле нельзя указывать более одного раза;
- тип initializer должен совпадать с типом поля;
- неуказанные поля получают default value своего типа;
- пустой struct literal допустим, если syntax поддерживает `Point{}`.

`&Point{...}` отличается от `Point{...}`:

- `Point{...}` создаёт struct value;
- `&Point{...}` создаёт pointer на heap-backed struct instance.

## Pointer Receiver Dispatch

Если method объявлен с receiver типа `*T`:

```cetus
func (p *Point) Move(dx int) {
    p.x = p.x + dx;
}
```

то вызов подчиняется следующим правилам:

- если receiver expression уже имеет тип `*T`, method получает это pointer value;
- если receiver expression имеет тип `T` и является addressable, compiler неявно берёт адрес receiver;
- если receiver expression имеет тип `T`, но не является addressable, вызов является semantic error.

Примеры:

```cetus
var point Point;
point.Move(1);      // ok, point addressable

stack := NewStack();
stack.Push(10);     // ok, stack already has type *Stack

MakePoint().Move(1);  // semantic error
```

Pointer receiver не означает отдельный синтаксис вызова. Пользователь по-прежнему пишет `value.Method()`.

## Совместимость

Совместимыми для `==` и `!=` считаются:

- одинаковые типы;
- пары truthy-compatible типов.

Для pointer и nullable types:

- `*T` совместим только с `*T`;
- `*T` совместим с `nil`;
- `[]T` совместим с `nil`;
- разные pointer target types несовместимы;
- struct values сравниваются только если язык явно определит такую операцию позже.

Для арифметики `int` и `float` объединяются в `float`, если хотя бы один операнд имеет тип `float`; иначе результат имеет тип `int`.

Функция может возвращать несколько значений. Multiple return не является sequence type. Это специальный function-result contract:

```cetus
func Pop() (int, bool)
```

`(int, bool)` нельзя интерпретировать как массив, slice или struct. Это упорядоченный список результатов функции, который используется в:

- `return value, ok`;
- multiple assignment;
- argument expansion в call context, если language rule это разрешает.

## Будущие типы

В будущих фазах планируются:

- полноценные pointer values для всех addressable объектов;
- safe escape model для `&local`;
- явное dereference expression;
- richer nullable model;
- interface-like or generic aggregate contracts.
