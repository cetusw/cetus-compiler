# Structs

## Содержание

- [Статус](#статус)
- [Объявление типа](#объявление-типа)
- [Поля](#поля)
- [Использование как тип](#использование-как-тип)
- [Runtime Instance](#runtime-instance)
- [Member Access](#member-access)
- [Связь с grammar](#связь-с-grammar)

## Статус

Фаза 7.1 добавила syntax и AST для struct type declarations.

Фаза 7.2 добавила semantic symbol для пользовательских типов:

- struct declaration создаёт символ вида `TYPE`;
- имя struct можно использовать как тип переменной, параметра функции, return type и элемента массива;
- semantic analyzer проверяет дубли полей;
- semantic analyzer проверяет, что типы полей разрешаются.

Фаза 7.3 добавляет runtime instance representation и member access:

- `ObjStruct` хранит имя типа и значения полей;
- default declaration `var p Point;` создаёт instance со значениями полей по умолчанию;
- `p.x` читает поле;
- `p.x = value;` записывает поле;
- semantic analyzer проверяет, что поле существует.

Методы добавляются следующими фазами 7.x.

## Объявление типа

Struct type объявляется на верхнем уровне программы:

```cetus
type Point struct {
    x int;
    y int;
}
```

Имя после `type` является именем пользовательского типа. Тело `struct` содержит список полей.

## Поля

Поле задаётся именем и типом:

```cetus
x int;
y [3]float;
```

Каждое поле завершается `;`.

Имена полей внутри одного struct не должны повторяться:

```cetus
type Point struct {
    x int;
    x int; // semantic error
}
```

Тип поля может быть built-in типом, массивом или пользовательским типом, объявленным как top-level struct type.

## Использование Как Тип

После объявления struct имя типа можно использовать в typed declarations:

```cetus
type Point struct {
    x int;
    y int;
}

func identity(point Point) Point {
    return point;
}

func main() {
    var point Point;
    var points [2]Point;
    identity(point);
}
```

## Runtime Instance

Объявление переменной struct-типа без initializer создаёт runtime instance со значениями по умолчанию:

```cetus
type Point struct {
    x int;
    y int;
}

func main() {
    var p Point; // x = 0, y = 0
}
```

Default value строится рекурсивно:

- `int` -> `0`;
- `float` -> `0.0`;
- `bool` -> `false`;
- `string` -> `""`;
- array -> массив default values;
- struct -> `ObjStruct` с default values всех полей.

## Member Access

Поле struct читается через dot syntax:

```cetus
printf(p.x);
```

Запись в поле:

```cetus
p.x = 1;
```

Semantic analyzer требует, чтобы левая часть dot expression имела named struct type и чтобы поле было объявлено в layout типа.

Пример:

```cetus
type Point struct {
    x int;
    y int;
}

func main() {
    var p Point;
    p.x = 1;
    printf(p.x);
}
```

Ожидаемый вывод:

```text
1
```

## Связь с grammar

Live grammar содержит:

```text
~TopLevelDecl~ -> ~TypeDecl~ @pass_expr

~TypeDecl~ -> TYPE IDENTIFIER STRUCT LBRACE ~StructFieldList~ RBRACE @struct_declaration

~StructFieldList~ -> ~StructFieldList~ ~StructField~ @struct_field_list
~StructFieldList~ -> ~StructField~ @struct_field_list_single

~StructField~ -> IDENTIFIER ~Type~ SEMICOLON @struct_field
```

AST node:

```text
StructDeclarationASTNode(Point)
  StructField(x int)
  StructField(y int)
```
