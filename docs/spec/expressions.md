# Expressions

## Содержание

- [Категории выражений](#категории-выражений)
- [Литералы](#литералы)
- [Array literals](#array-literals)
- [Идентификаторы](#идентификаторы)
- [Унарные выражения](#унарные-выражения)
- [Бинарные выражения](#бинарные-выражения)
- [Вызовы функций](#вызовы-функций)
- [Доступ к члену и индексация](#доступ-к-члену-и-индексация)

## Категории выражений

Грамматика использует общий нетерминал `~Con~` для выражений, которые могут участвовать в условиях.

## Литералы

Реализованы AST-узлы:

- `IntLiteralASTNode`;
- `FloatLiteralASTNode`;
- `BoolLiteralASTNode`.

## Array literals

Целевой синтаксис array literal:

```cetus
[3]int{1, 2, 3}
[2]string{"a", "b"}
```

Полная спецификация массивов описана в [Arrays](./arrays.md).

Array literal будет добавлен в grammar отдельной задачей фазы 6. Сейчас live grammar поддерживает index access, но не поддерживает создание array literal.

## Идентификаторы

Идентификатор представлен `IdentifierASTNode`. Во время семантического анализа имя должно быть найдено в таблице символов.

## Унарные выражения

```cetus
-a
!flag
```

Унарные операции представлены `UnaryASTNode`.

## Бинарные выражения

```cetus
a + b
a <= b
a && b
```

Бинарные операции представлены `BinaryASTNode`.

## Вызовы функций

```cetus
answer()
add(1, 2)
p.Sum()
```

Вызов функции и метода представлен `CallExpressionASTNode`.

Вызываемое имя должно быть объявлено как функция. Количество и типы аргументов проверяются по параметрам функции.

Method call:

```cetus
p.Sum()
p.Move(4)
```

разрешается по типу receiver `p`. Codegen вызывает method как обычную функцию с receiver первым аргументом.

Для pointer receiver codegen автоматически передаёт receiver как reference. На текущем этапе такой receiver должен быть identifier.

## Доступ к члену и индексация

Грамматика содержит:

```text
~LargId~ -> ~LargId~ DOT IDENTIFIER @member_access
~LargId~ -> ~LargId~ LBRACKET ~Exp~ RBRACKET @index_access
```

AST-узлы существуют:

- `MemberAccessASTNode`;
- `IndexASTNode`.

Индексация поддерживается для array, slice и string.

- для `[N]T` и `[]T` индекс должен иметь тип `int`, а результат `value[index]` имеет тип элемента;
- для `string` индекс должен иметь тип `int`, а результат `text[index]` имеет тип `string`;
- `string[index]` доступен только для чтения.

На текущем этапе строка индексируется по byte offset. Это согласовано с `len(string)`, который тоже считает байты.

Member access применяется к struct values:

```cetus
p.x
```

Левая часть должна иметь named struct type. Имя справа от `.` должно быть объявленным полем этого struct. Тип результата равен типу поля.

Member access может быть assignment target:

```cetus
p.x = 1;
```
