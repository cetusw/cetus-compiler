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
~Program~ -> ~StmtList~ @program
~Program~ -> e @program_empty
```

Программа является корневым AST-узлом и содержит список операторов. Пустая программа синтаксически разрешена.

На верхнем уровне разрешены только объявления. Исполняемые операторы верхнего уровня являются семантической ошибкой.

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
~Stmt~ -> FUNC IDENTIFIER LPAREN RPAREN ~Block~ @function_void_no_params
~Stmt~ -> FUNC IDENTIFIER LPAREN RPAREN ~TypeName~ ~Block~ @function_return_no_params
~Stmt~ -> FUNC IDENTIFIER LPAREN ~ParamList~ RPAREN ~Block~ @function_void
~Stmt~ -> FUNC IDENTIFIER LPAREN ~ParamList~ RPAREN ~TypeName~ ~Block~ @function_return
```

Простой оператор завершается `;`. `if` является составным оператором и не требует `;` после блока.

```text
~SimpleStmt~ -> ~IdentifierList~ COLON_EQUAL ~ExpressionList~ @short_var_declaration
~SimpleStmt~ -> VAR ~IdentifierList~ EQUAL ~ExpressionList~ @var_inferred_declaration
~SimpleStmt~ -> VAR ~IdentifierList~ ~TypeName~ @var_typed_declaration
~SimpleStmt~ -> VAR ~IdentifierList~ ~TypeName~ EQUAL ~ExpressionList~ @var_typed_initialized_declaration
~SimpleStmt~ -> ~IdentifierList~ EQUAL ~ExpressionList~ @assignment
~SimpleStmt~ -> PRINTF LPAREN ~Con~ RPAREN @printf
~SimpleStmt~ -> ~Con~ @expression_statement
```

Параметры функции:

```text
~ParamList~ -> ~ParamList~ COMMA ~Param~ @param_list
~ParamList~ -> ~Param~ @param_list_single

~Param~ -> IDENTIFIER ~TypeName~ @param
```

## Блоки

```text
~Block~ -> LBRACE ~StmtList~ RBRACE @block
~Block~ -> LBRACE RBRACE @block_empty
```

Блок содержит список операторов. Пустой блок разрешён.

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
