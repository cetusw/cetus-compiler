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
~Program~ -> ~StmtList~ @pass_expr
```

Программа является списком операторов.

## Операторы

```text
~StmtList~ -> ~StmtList~ ~Stmt~ @sequence
~StmtList~ -> ~Stmt~ @pass_expr

~Stmt~ -> ~SimpleStmt~ SEMICOLON @pass_expr
~Stmt~ -> IF ~Con~ ~Block~ @if
~Stmt~ -> IF ~Con~ ~Block~ ELSE ~Block~ @if_else
```

Простой оператор завершается `;`. `if` является составным оператором и не требует `;` после блока.

```text
~SimpleStmt~ -> IDENTIFIER COLON_EQUAL ~Con~ @assignment
~SimpleStmt~ -> PRINTF LPAREN ~Con~ RPAREN @printf
~SimpleStmt~ -> ~Con~ @pass_expr
```

## Блоки

```text
~Block~ -> LBRACE ~StmtList~ RBRACE @block
```

Блок содержит непустой список операторов.

> TODO
>
> Требуется определить синтаксис пустого блока.

## Выражения

Текущая грамматика выражений разделяет логические, сравнительные и арифметические уровни:

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
~Exp2~ -> MINUS ~Exp2~ @unary
~Exp2~ -> ~LargId~ @pass_expr
~Exp2~ -> INT_LIT @int_literal
~Exp2~ -> FLOAT_LIT @float_literal
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
| 7 | `.` и `[]`        |

Бинарные операторы в грамматике лево-ассоциативны.

## Semantic actions

Semantic actions указываются после `@`:

```text
~Exp~ -> ~Exp~ PLUS ~Exp1~ @binary
```

Они не являются частью синтаксиса языка. В компиляторе они управляют построением AST во время разбора.
