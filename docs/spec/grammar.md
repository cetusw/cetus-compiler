# Grammar

## Содержание

- [Нотация](#нотация)
- [Grammar](#grammar)
- [Semantic actions](#semantic-actions)
- [Ассоциативность и приоритет](#ассоциативность-и-приоритет)
- [Подготовленная таблица](#подготовленная-таблица)

## Нотация

Нетерминалы записываются в `~...~`. Терминалы соответствуют токенам лексера. Semantic action записывается после `@`.


> TODO
> 
> Разделить файл грамматики на логические блоки, соединять при генерации таблицы.

## Grammar

```text
~Program~ -> ~StmtList~ @program

~StmtList~ -> ~StmtList~ ~Stmt~ @statement_list
~StmtList~ -> ~Stmt~ @statement_list_single

~Stmt~ -> ~SimpleStmt~ SEMICOLON @pass_expr
~Stmt~ -> IF ~Con~ ~Block~ @if
~Stmt~ -> IF ~Con~ ~Block~ ELSE ~Block~ @if_else

~SimpleStmt~ -> ~IdentifierList~ COLON_EQUAL ~ExpressionList~ @short_var_declaration
~SimpleStmt~ -> VAR ~IdentifierList~ EQUAL ~ExpressionList~ @var_inferred_declaration
~SimpleStmt~ -> VAR ~IdentifierList~ ~TypeName~ @var_typed_declaration
~SimpleStmt~ -> VAR ~IdentifierList~ ~TypeName~ EQUAL ~ExpressionList~ @var_typed_initialized_declaration
~SimpleStmt~ -> ~IdentifierList~ EQUAL ~ExpressionList~ @assignment
~SimpleStmt~ -> PRINTF LPAREN ~Con~ RPAREN @printf
~SimpleStmt~ -> ~Con~ @expression_statement

~Block~ -> LBRACE ~StmtList~ RBRACE @block
~Block~ -> LBRACE RBRACE @block_empty

~IdentifierList~ -> ~IdentifierList~ COMMA IDENTIFIER @identifier_list
~IdentifierList~ -> IDENTIFIER @identifier_list_single

~ExpressionList~ -> ~ExpressionList~ COMMA ~Con~ @expression_list
~ExpressionList~ -> ~Con~ @expression_list_single

~TypeName~ -> IDENTIFIER @type_name

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
~Exp2~ -> STRING @string_literal

~LargId~ -> IDENTIFIER @identifier
~LargId~ -> ~LargId~ DOT IDENTIFIER @member_access
~LargId~ -> ~LargId~ LBRACKET ~Exp~ RBRACKET @index_access

~MulOp~ -> STAR @pass_token
~MulOp~ -> SLASH @pass_token
~MulOp~ -> PERCENT @pass_token

~ROp~ -> LESS @pass_token
~ROp~ -> LESS_EQUAL @pass_token
~ROp~ -> BANG_EQUAL @pass_token
~ROp~ -> EQUAL_EQUAL @pass_token
~ROp~ -> GREATER @pass_token
~ROp~ -> GREATER_EQUAL @pass_token
```

## Semantic actions

Semantic actions выполняются во время parsing и строят AST напрямую.

## Ассоциативность и приоритет

Приоритет определяется структурой grammar. Лево-рекурсивные уровни задают левую ассоциативность бинарных операторов.

## Подготовленная таблица

Подготовленная grammar хранится в `docs/prepared_grammar.json`.

```bash
./cmake-build-debug/cetus --parse-ast --regen-table test.txt
```
