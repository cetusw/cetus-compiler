# Arrays

## Содержание

- [Статус](#статус)
- [Тип массива](#тип-массива)
- [Объявление](#объявление)
- [Литералы](#литералы)
- [Индексация](#индексация)
- [Присваивание по индексу](#присваивание-по-индексу)
- [Многомерные массивы](#многомерные-массивы)
- [Семантические правила](#семантические-правила)
- [Связь с grammar](#связь-с-grammar)

## Статус

Этот документ фиксирует целевой синтаксис массивов.

На момент спецификации parser уже содержит syntax для index access:

```text
~LargId~ -> ~LargId~ LBRACKET ~Exp~ RBRACKET @index_access
```

Array type и semantic analysis для индексного чтения добавлены в фазе 6.2. Runtime `ObjArray`, default array creation, index reading, bounds check и `len(array)` добавлены в фазе 6.3.

Assignment by index и multidimensional index chains добавлены в фазе 6.4.

Array literal будет добавлен следующими задачами фазы 6. До его реализации соответствующие примеры ниже являются спецификацией поведения, а не гарантией текущего выполнения.

## Тип массива

Массив имеет фиксированную длину и тип элемента:

```cetus
[3]int
[2]float
[4]string
```

Синтаксис типа:

```text
ArrayType = "[" IntLiteral "]" Type
Type      = TypeName | ArrayType
```

Размер массива является compile-time константой типа `int` и должен быть больше нуля.

Тип `[3]int` не совместим с `[4]int`: длина является частью типа. Тип `[3]int` не совместим с `[3]float`: тип элемента также является частью типа.

## Объявление

Typed declaration без initializer создаёт массив со значениями по умолчанию для типа элемента:

```cetus
func main() {
    var numbers [3]int;
    println(numbers[0]);
}
```

Значения по умолчанию:

| Тип элемента | Значение |
|---|---|
| `int` | `0` |
| `float` | `0.0` |
| `bool` | `false` |
| `string` | `""` |
| array type | массив, рекурсивно заполненный default values |

Typed declaration с initializer:

```cetus
func main() {
    var numbers [3]int = [3]int{1, 2, 3};
}
```

Short declaration выводит тип из array literal:

```cetus
func main() {
    numbers := [3]int{1, 2, 3};
}
```

## Литералы

Array literal задаёт тип и значения элементов:

```cetus
[3]int{1, 2, 3}
[2]string{"a", "b"}
[2]bool{true, false}
```

Количество элементов в literal должно совпадать с длиной массива.

Все элементы literal должны быть совместимы с типом элемента массива. Для numeric типов применяются общие правила числового продвижения языка только там, где они уже разрешены typechecker-ом; literal `[2]int{1, 2.5}` является ошибкой.

Пустой literal допустим только для массива нулевой длины, если нулевая длина будет явно разрешена отдельным решением. В текущей спецификации размер массива должен быть больше нуля, поэтому `[0]int{}` не является допустимым типом.

## Индексация

Индексация читает элемент массива:

```cetus
func main() {
    numbers := [3]int{10, 20, 30};
    println(numbers[1]);
}
```

Индекс должен иметь тип `int`. Индексация использует zero-based indexing:

- первый элемент: `array[0]`;
- последний элемент массива длины `N`: `array[N - 1]`.

Выход за границы массива является runtime error. Если индекс является compile-time константой, semantic analyzer может диагностировать ошибку раньше.

## Присваивание по индексу

Index expression может быть lvalue:

```cetus
func main() {
    var numbers [3]int;
    numbers[1] = 42;
    println(numbers[1]);
}
```

Тип правой части должен быть совместим с типом элемента массива.

Присваивание по индексу не меняет длину массива.

На текущем runtime-этапе чтение `array[index]` и запись `array[index] = value` реализованы.

## Многомерные массивы

Многомерный массив задаётся как массив массивов:

```cetus
[2][3]int
```

Это массив длины `2`, каждый элемент которого имеет тип `[3]int`.

Объявление и запись:

```cetus
func main() {
    var matrix [2][3]int;
    matrix[0][1] = 5;
    println(matrix[0][1]);
}
```

Literal:

```cetus
func main() {
    matrix := [2][3]int{
        [3]int{1, 2, 3},
        [3]int{4, 5, 6}
    };
    println(matrix[1][2]);
}
```

Цепочка `matrix[0][1]` разбирается как последовательные index access:

```text
IndexASTNode
  IndexASTNode
    IdentifierASTNode(matrix)
    IntLiteralASTNode(0)
  IntLiteralASTNode(1)
```

## Семантические правила

Semantic analyzer должен проверять:

- размер массива является положительным integer literal;
- тип элемента существует;
- initializer literal имеет тот же array type, что и переменная;
- количество элементов literal равно длине массива;
- каждый элемент literal совместим с типом элемента;
- индексируемое выражение имеет array type;
- индекс имеет тип `int`;
- результат `array[index]` имеет тип элемента массива;
- left-hand side вида `array[index]` допустим в assignment;
- присваивание в `array[index]` принимает значение типа элемента.

## Связь с grammar

Текущая live grammar уже содержит index access:

```text
~LargId~ -> ~LargId~ LBRACKET ~Exp~ RBRACKET @index_access
```

Live grammar поддерживает array type:

```text
~Type~ -> IDENTIFIER @type_name
~Type~ -> LBRACKET INT_LIT RBRACKET ~Type~ @array_type
```

Live grammar поддерживает assignable targets для assignment:

```text
~AssignableList~ -> ~AssignableList~ COMMA ~LargId~ @assignable_list
~AssignableList~ -> ~LargId~ @assignable_list_single

~SimpleStmt~ -> ~AssignableList~ EQUAL ~ExpressionList~ @assignment
```

Для полного синтаксиса массивов grammar должна быть расширена отдельной реализационной фазой. Целевая форма оставшихся правил:

```text
~ArrayLiteral~ -> ~Type~ LBRACE ~ExpressionList~ RBRACE @array_literal
~Exp2~ -> ~ArrayLiteral~ @pass_expr
```

После добавления этих правил:

- array literal сможет создавать значения массивов;
- typed initialized declarations смогут принимать array literal.
