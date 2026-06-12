# AST

## Содержание

- [Модель AST](#модель-ast)
- [Отсутствие Parse Tree](#отсутствие-parse-tree)
- [AST nodes](#ast-nodes)
- [Statement и Expression](#statement-и-expression)
- [Semantic actions](#semantic-actions)
- [Аннотации типов](#аннотации-типов)

## Модель AST

AST является основным представлением программы после синтаксического анализа.

Владение дочерними узлами выполняется через `std::unique_ptr<ASTNode>`.

## Отсутствие Parse Tree

Отдельное Concrete Syntax Tree или Parse Tree не создаётся. Парсер сразу строит AST во время reduce-действий.

## AST nodes

AST-узлы:

- `ASTNode`;
- `StatementASTNode`;
- `ExpressionASTNode`;
- `ProgramASTNode`;
- `StatementListASTNode`;
- `BlockASTNode`;
- `IntLiteralASTNode`;
- `FloatLiteralASTNode`;
- `BoolLiteralASTNode`;
- `StringLiteralASTNode`;
- `ArrayLiteralASTNode`;
- `UnaryASTNode`;
- `BinaryASTNode`;
- `AssignmentASTNode`;
- `IncrementASTNode`;
- `DecrementASTNode`;
- `ShortVariableDeclarationASTNode`;
- `VariableDeclarationASTNode`;
- `ExpressionStatementASTNode`;
- `IdentifierASTNode`;
- `MemberAccessASTNode`;
- `IndexASTNode`;
- `CallExpressionASTNode`;
- `TypeDescriptor` описывает scalar, fixed array, slice и named types для declared/inferred types;
- `IfASTNode`;
- `ReturnASTNode`;
- `FunctionDeclarationASTNode`.
- `StructDeclarationASTNode`.

## Statement и Expression

`StatementASTNode` является базовым классом для операторов. Операторы выполняют действие и имеют тип `void`.

`ExpressionASTNode` является базовым классом для выражений. Выражения вычисляют значение и имеют предметный тип: `int`, `float`, `bool` или другой тип языка.

Выражение, записанное как отдельный оператор, представляется узлом `ExpressionStatementASTNode`.

## Semantic actions

Примеры соответствия:

| Semantic action | AST |
|---|---|
| `@int_literal` | `IntLiteralASTNode` |
| `@float_literal` | `FloatLiteralASTNode` |
| `@bool_literal` | `BoolLiteralASTNode` |
| `@string_literal` | `StringLiteralASTNode` |
| `@array_literal` | `ArrayLiteralASTNode` |
| `@unary` | `UnaryASTNode` |
| `@binary` | `BinaryASTNode` |
| `@assignment` | `AssignmentASTNode` |
| `@increment` | `IncrementASTNode` |
| `@decrement` | `DecrementASTNode` |
| `@short_var_declaration` | `ShortVariableDeclarationASTNode` |
| `@var_inferred_declaration` | `VariableDeclarationASTNode` |
| `@var_typed_declaration` | `VariableDeclarationASTNode` |
| `@var_typed_initialized_declaration` | `VariableDeclarationASTNode` |
| `@expression_statement` | `ExpressionStatementASTNode` |
| `@program` | `ProgramASTNode` |
| `@program_empty` | `ProgramASTNode` |
| `@statement_list` | `StatementListASTNode` |
| `@statement_list_single` | `StatementListASTNode` |
| `@block` | `BlockASTNode` |
| `@block_empty` | `BlockASTNode` |
| `@if` | `IfASTNode` без else |
| `@if_else` | `IfASTNode` с else |
| `@return_void` | `ReturnASTNode` без значения |
| `@return_value` | `ReturnASTNode` со значением |
| `@call_no_args` | `CallExpressionASTNode` без аргументов |
| `@call` | `CallExpressionASTNode` с аргументами |
| `@method_call_no_args` | `CallExpressionASTNode` с receiver без аргументов |
| `@method_call` | `CallExpressionASTNode` с receiver и аргументами |
| `@function_void_no_params` | `FunctionDeclarationASTNode` без параметров и без возвращаемого значения |
| `@function_return_no_params` | `FunctionDeclarationASTNode` без параметров с возвращаемым значением |
| `@function_void` | `FunctionDeclarationASTNode` с параметрами без возвращаемого значения |
| `@function_return` | `FunctionDeclarationASTNode` с параметрами и возвращаемым значением |
| `@method_void_no_params` | `FunctionDeclarationASTNode` с receiver, без параметров и без возвращаемого значения |
| `@method_return_no_params` | `FunctionDeclarationASTNode` с receiver, без параметров с возвращаемым значением |
| `@method_void` | `FunctionDeclarationASTNode` с receiver и параметрами без возвращаемого значения |
| `@method_return` | `FunctionDeclarationASTNode` с receiver и параметрами с возвращаемым значением |
| `@struct_declaration` | `StructDeclarationASTNode` |
| `@struct_field` | `StructField` |

## Аннотации типов

`ASTNode` содержит optional-аннотацию выведенного типа. Семантический анализатор заполняет её после проверки узла.
