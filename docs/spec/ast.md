# AST

## Содержание

- [Модель AST](#модель-ast)
- [Отсутствие Parse Tree](#отсутствие-parse-tree)
- [AST nodes](#ast-nodes)
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
- `ProgramASTNode`;
- `StatementListASTNode`;
- `BlockASTNode`;
- `IntLiteralASTNode`;
- `FloatLiteralASTNode`;
- `BoolLiteralASTNode`;
- `UnaryASTNode`;
- `BinaryASTNode`;
- `AssignmentASTNode`;
- `IdentifierASTNode`;
- `MemberAccessASTNode`;
- `IndexASTNode`;
- `IfASTNode`;
- `PrintfASTNode`.

## Semantic actions

Примеры соответствия:

| Semantic action | AST |
|---|---|
| `@int_literal` | `IntLiteralASTNode` |
| `@float_literal` | `FloatLiteralASTNode` |
| `@bool_literal` | `BoolLiteralASTNode` |
| `@unary` | `UnaryASTNode` |
| `@binary` | `BinaryASTNode` |
| `@assignment` | `AssignmentASTNode` |
| `@program` | `ProgramASTNode` |
| `@statement_list` | `StatementListASTNode` |
| `@statement_list_single` | `StatementListASTNode` |
| `@block` | `BlockASTNode` |
| `@if` | `IfASTNode` без else |
| `@if_else` | `IfASTNode` с else |
| `@printf` | `PrintfASTNode` |

## Аннотации типов

`ASTNode` содержит optional-аннотацию выведенного типа. Семантический анализатор заполняет её после проверки узла.
