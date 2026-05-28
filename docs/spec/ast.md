# AST

## Содержание

- [Модель AST](#модель-ast)
- [Отсутствие Parse Tree](#отсутствие-parse-tree)
- [AST nodes](#ast-nodes)
- [Разделение последовательностей](#разделение-последовательностей)
- [Semantic actions](#semantic-actions)
- [Аннотации типов](#аннотации-типов)

## Модель AST

AST является основным представлением программы после синтаксического анализа.

Владение дочерними узлами выполняется через `std::unique_ptr<ASTNode>`.

## Отсутствие Parse Tree

Отдельное Concrete Syntax Tree или Parse Tree не создаётся. Парсер сразу строит AST во время reduce-действий.

## AST nodes

Текущие AST-узлы:

- `ASTNode`;
- `IntLiteralASTNode`;
- `FloatLiteralASTNode`;
- `BoolLiteralASTNode`;
- `UnaryASTNode`;
- `BinaryASTNode`;
- `AssignmentASTNode`;
- `SequenceASTNode`;
- `IdentifierASTNode`;
- `MemberAccessASTNode`;
- `IndexASTNode`;
- `IfASTNode`;
- `PrintfASTNode`.

## Разделение последовательностей

> TODO
>
> `SequenceASTNode` должен быть разделён на отдельные AST-узлы, соответствующие обычной структуре языка программирования:
>
> - `ProgramASTNode` — корень AST для всего исходного файла;
> - `StatementListASTNode` — список операторов внутри программы или блока;
> - `BlockASTNode` — блок `{ ... }` с собственной семантикой области видимости.
>
> После этого `SequenceASTNode` должен быть удалён или оставлен только как внутренний временный узел, если он нужен parser implementation.

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
| `@sequence` | `SequenceASTNode` |
| `@if` | `IfASTNode` без else |
| `@if_else` | `IfASTNode` с else |
| `@printf` | `PrintfASTNode` |

## Аннотации типов

`ASTNode` содержит optional-аннотацию выведенного типа. Семантический анализатор заполняет её после проверки узла.
