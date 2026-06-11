# Syntax

## Содержание

- [Модель синтаксического анализа](#модель-синтаксического-анализа)
- [Программа](#программа)
- [Операторы](#операторы)
- [Блоки](#блоки)
- [Выражения](#выражения)
- [Приоритет операторов](#приоритет-операторов)
- [Semantic actions](#semantic-actions)

## Модель синтаксического анализа

Синтаксис CETUS задан контекстно-свободной грамматикой (CFG). Синтаксический анализ использует подготовленную таблицу разбора и semantic actions.

Парсер при reduce-действиях сразу создаёт AST-узлы.

## Программа

```text
~Program~ -> ~TopLevelDeclList~ @program
~Program~ -> e @program_empty
```

Программа является корневым AST-узлом и содержит список объявлений верхнего уровня. Пустая программа синтаксически разрешена.

На верхнем уровне разрешены только объявления.

```text
~TopLevelDeclList~ -> ~TopLevelDeclList~ ~TopLevelDecl~ @statement_list
~TopLevelDeclList~ -> ~TopLevelDecl~ @statement_list_single

~TopLevelDecl~ -> ~FunctionDecl~ @pass_expr
~TopLevelDecl~ -> ~TypeDecl~ @pass_expr
```

Исполняемые операторы верхнего уровня являются синтаксической ошибкой.

```cetus
func main() {
    printf(1);
}
```

```cetus
printf(1);

func main() {
}
```

## Операторы

```text
~StmtList~ -> ~StmtList~ ~Stmt~ @statement_list
~StmtList~ -> ~Stmt~ @statement_list_single

~Stmt~ -> ~SimpleStmt~ SEMICOLON @pass_expr
~Stmt~ -> RETURN SEMICOLON @return_void
~Stmt~ -> RETURN ~Con~ SEMICOLON @return_value
~Stmt~ -> IF ~Con~ ~Block~ @if
~Stmt~ -> IF ~Con~ ~Block~ ELSE ~Block~ @if_else
```

Простой оператор завершается `;`. `if` является составным оператором и не требует `;` после блока.

Объявления функций являются объявлениями верхнего уровня:

```text
~FunctionDecl~ -> FUNC IDENTIFIER LPAREN RPAREN ~Block~ @function_void_no_params
~FunctionDecl~ -> FUNC IDENTIFIER LPAREN RPAREN ~Type~ ~Block~ @function_return_no_params
~FunctionDecl~ -> FUNC IDENTIFIER LPAREN ~ParamList~ RPAREN ~Block~ @function_void
~FunctionDecl~ -> FUNC IDENTIFIER LPAREN ~ParamList~ RPAREN ~Type~ ~Block~ @function_return
~FunctionDecl~ -> FUNC LPAREN ~Param~ RPAREN IDENTIFIER LPAREN RPAREN ~Block~ @method_void_no_params
~FunctionDecl~ -> FUNC LPAREN ~Param~ RPAREN IDENTIFIER LPAREN RPAREN ~Type~ ~Block~ @method_return_no_params
~FunctionDecl~ -> FUNC LPAREN ~Param~ RPAREN IDENTIFIER LPAREN ~ParamList~ RPAREN ~Block~ @method_void
~FunctionDecl~ -> FUNC LPAREN ~Param~ RPAREN IDENTIFIER LPAREN ~ParamList~ RPAREN ~Type~ ~Block~ @method_return
```

Объявления struct type также являются объявлениями верхнего уровня:

```text
~TypeDecl~ -> TYPE IDENTIFIER STRUCT LBRACE ~StructFieldList~ RBRACE @struct_declaration

~StructFieldList~ -> ~StructFieldList~ ~StructField~ @struct_field_list
~StructFieldList~ -> ~StructField~ @struct_field_list_single

~StructField~ -> IDENTIFIER ~Type~ SEMICOLON @struct_field
```

```text
~SimpleStmt~ -> ~AssignableList~ COLON_EQUAL ~ExpressionList~ @short_var_declaration
~SimpleStmt~ -> VAR ~IdentifierList~ EQUAL ~ExpressionList~ @var_inferred_declaration
~SimpleStmt~ -> VAR ~IdentifierList~ ~Type~ @var_typed_declaration
~SimpleStmt~ -> VAR ~IdentifierList~ ~Type~ EQUAL ~ExpressionList~ @var_typed_initialized_declaration
~SimpleStmt~ -> ~AssignableList~ EQUAL ~ExpressionList~ @assignment
~SimpleStmt~ -> ~LargId~ PLUS_PLUS @increment
~SimpleStmt~ -> ~Con~ @expression_statement

~AssignableList~ -> ~AssignableList~ COMMA ~LargId~ @assignable_list
~AssignableList~ -> ~LargId~ @assignable_list_single
```

Параметры функции:

```text
~ParamList~ -> ~ParamList~ COMMA ~Param~ @param_list
~ParamList~ -> ~Param~ @param_list_single

~Param~ -> IDENTIFIER ~Type~ @param
```

## Блоки

```text
~Block~ -> LBRACE ~StmtList~ RBRACE @block
~Block~ -> LBRACE RBRACE @block_empty
```

Блок содержит список операторов. Пустой блок разрешён.

## Типы

Текущая live grammar описывает scalar type name и array type:

```text
~Type~ -> IDENTIFIER @type_name
~Type~ -> LBRACKET INT_LIT RBRACKET ~Type~ @array_type
```

Целевой синтаксис массивов расширяет type grammar формой `[N]T`:

```cetus
var a [2][3]int;
```

Полная спецификация массивов описана в [Arrays](./arrays.md).

## Выражения

Грамматика выражений разделяет логические, сравнительные и арифметические уровни:

```text
~Con~ -> ~Con~ OR_OR ~Con1~ @binary
~Con~ -> ~Con1~ @pass_expr

~Con1~ -> ~Con1~ AND_AND ~Con2~ @binary
~Con1~ -> ~Con2~ @pass_expr

~Con2~ -> BANG ~Con2~ @unary
~Con2~ -> TRUE @bool_literal
~Con2~ -> FALSE @bool_literal
~Con2~ -> ~RCon~ @pass_expr

~RCon~ -> ~Exp~ ~ROp~ ~Exp~ @binary
~RCon~ -> ~Exp~ @pass_expr

~Exp~ -> ~Exp~ PLUS ~Exp1~ @binary
~Exp~ -> ~Exp~ MINUS ~Exp1~ @binary
~Exp~ -> ~Exp1~ @pass_expr

~Exp1~ -> ~Exp1~ ~MulOp~ ~Exp2~ @binary
~Exp1~ -> ~Exp2~ @pass_expr

~Exp2~ -> LPAREN ~Con~ RPAREN @group
~Exp2~ -> IDENTIFIER LPAREN RPAREN @call_no_args
~Exp2~ -> IDENTIFIER LPAREN ~ExpressionList~ RPAREN @call
~Exp2~ -> MINUS ~Exp2~ @unary
~Exp2~ -> ~LargId~ @pass_expr
~Exp2~ -> INT_LIT @int_literal
~Exp2~ -> FLOAT_LIT @float_literal
~Exp2~ -> STRING @string_literal
```

## Приоритет операторов

От меньшего приоритета к большему:

| Приоритет | Операторы         |
|---:|-------------------|
| 1 | `\|\|` |
| 2 | `&&`              |
| 3 | `== != < <= > >=` |
| 4 | `+ -`             |
| 5 | `* / %`           |
| 6 | унарные `! -`     |
| 7 | вызовы функций, `.` и `[]` |

Бинарные операторы в грамматике лево-ассоциативны.

## Semantic actions

Semantic actions указываются после `@`:

```text
~Exp~ -> ~Exp~ PLUS ~Exp1~ @binary
```

Они не являются частью синтаксиса языка. В компиляторе они управляют построением AST во время разбора.
