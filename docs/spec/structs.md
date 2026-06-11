# Structs

## Содержание

- [Статус](#статус)
- [Объявление типа](#объявление-типа)
- [Поля](#поля)
- [Связь с grammar](#связь-с-grammar)

## Статус

Фаза 7.1 добавила syntax и AST для struct type declarations.

Фаза 7.2 добавляет semantic symbol для пользовательских типов:

- struct declaration создаёт символ вида `TYPE`;
- имя struct можно использовать как тип переменной, параметра функции, return type и элемента массива;
- semantic analyzer проверяет дубли полей;
- semantic analyzer проверяет, что типы полей разрешаются.

Создание runtime instance, member access и методы добавляются следующими фазами 7.x.

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

На этапе 7.2 это только semantic-level тип. Default runtime representation для struct instance и доступ к полям ещё не реализованы.

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
