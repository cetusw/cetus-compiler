# CETUS Documentation

CETUS — TDD-ориентированный язык программирования. В языке тестирование является частью семантики: целевая модель запрещает запуск программы до успешного выполнения встроенных тестов.

## Документация

- [Introduction](./spec/intro.md)
- [Lexical Structure](./spec/lexical-structure.md)
- [Syntax](./spec/syntax.md)
- [Types](./spec/types.md)
- [Arrays](./spec/arrays.md)
- [Structs](./spec/structs.md)
- [Semantic Analysis](./spec/semantic-analysis.md)
- [Statements](./spec/statements.md)
- [Expressions](./spec/expressions.md)
- [Functions](./spec/functions.md)
- [Tests](./spec/tests.md)
- [Mocks](./spec/mocks.md)
- [Execution Model](./spec/execution-model.md)
- [Runtime](./spec/runtime.md)
- [Interpreter](./spec/interpreter.md)
- [AST](./spec/ast.md)
- [Standard Library](./spec/standard-library.md)
- [Modules](./spec/modules.md)
- [Errors](./spec/errors.md)
- [Diagnostics](./spec/diagnostics.md)
- [Grammar](./spec/grammar.md)
- [Roadmap](./spec/roadmap.md)

## Практические команды

Сборка:

```bash
cmake --build cmake-build-debug
```

Вывод AST:

```bash
./cmake-build-debug/cetus --parse-ast test.txt
```

Семантическая проверка:

```bash
./cmake-build-debug/cetus --typecheck test.txt
```

Обновление подготовленной таблицы грамматики:

```bash
./cmake-build-debug/cetus --parse-ast --regen-table test.txt
```
