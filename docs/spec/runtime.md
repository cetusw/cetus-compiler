# Runtime

## Содержание

- [Назначение runtime](#назначение-runtime)
- [Состав runtime](#состав-runtime)
- [Runtime Objects](#runtime-objects)
- [Встроенные операции](#встроенные-операции)
- [Ошибки выполнения](#ошибки-выполнения)
- [Инварианты Object Model](#инварианты-object-model)

## Назначение runtime

Runtime отвечает за выполнение программы после успешных фаз проверки.

## Состав runtime

Runtime включает VM backend, значения времени выполнения, native-функции и механизм выполнения программы после успешной проверки.

> TODO
>
> Требуется связать полный frontend AST с генерацией байткода для VM.

## Runtime Objects

Heap objects используются для значений, которые не помещаются в scalar `Value` напрямую.

Текущие runtime objects:

- `ObjString`;
- `ObjFunction`;
- `ObjNative`;
- `ObjRef`;
- `ObjArray`;
- `ObjSlice`;
- `ObjStruct`.

Целевой object model дополнительно включает pointer-like runtime value для `*T`. Он не должен смешиваться с `ObjRef`, который может использоваться как внутренняя reference-механика для slot/field/index access.

`ObjArray` используется как backing storage для fixed arrays и slice storage.

`ObjSlice` является view-дескриптором поверх `ObjArray` и хранит:

- storage;
- offset;
- length;
- capacity.

`ObjStruct` хранит имя struct-типа и значения полей. Layout полей приходит из semantic symbol типа, а instance создаётся codegen/VM при default declaration:

```cetus
var p Point;
```

Member access выполняется bytecode-инструкциями:

- `OP_GET_MEMBER` читает поле из `ObjStruct`;
- `OP_SET_MEMBER` записывает поле в `ObjStruct`;
- `OP_STRUCT` создаёт новый `ObjStruct` из значений полей на стеке.

## Инварианты Object Model

Runtime должен соблюдать следующие инварианты:

- pointer value либо указывает на корректный target ожидаемого вида, либо равен `nil`;
- heap-backed struct, возвращённый из функции через `&Struct{...}`, остаётся жив после выхода из callee;
- runtime reference на stack slot не должен переживать lifetime этого slot, если только объект не был явно promoted/boxed;
- member access через pointer выполняет checked dereference;
- struct field lookup в runtime не должен silently invent field. Если semantic analyzer пропустил несуществующее поле, runtime трактует это как internal/runtime error.

Следствие: `ObjRef` не является пользовательским контрактом языка для всех pointer values. Это внутренний механизм, который может сосуществовать с полноценным runtime pointer object.

## Встроенные операции

Frontend grammar содержит вызов встроенной функции:

```cetus
printf(1);
```

Он рассматривается как временная встроенная функция вывода.

> TODO
>
> Перенести вывод в стандартную библиотеку после спецификации модулей и import-модели.

## Ошибки выполнения

Runtime errors возникают только после успешного parse и semantic analysis.

Нормальная граница ответственности:

- parser ловит syntax errors;
- semantic analyzer ловит type errors, unknown symbols, arity mismatch и invalid use-sites;
- runtime ловит ошибки, зависящие от конкретных значений времени выполнения.

К runtime errors относятся:

- division by zero, если операция не была исключена заранее;
- array/slice/string bounds violation;
- nil pointer dereference;
- invalid pointer target;
- invalid runtime member access;
- invalid native function precondition, если она зависит от runtime state.

### Nil Pointer Dereference

Если expression требует доступ к target значения типа `*T`, а значение равно `nil`, VM должна завершить выполнение с runtime error.

Примеры:

- `ptr.field`;
- pointer receiver call, который требует dereference receiver;
- future explicit dereference expression.

### Unknown Field At Runtime

Unknown field в исходной программе должен ловиться semantic analyzer.

Если runtime всё же получает инструкцию чтения/записи поля, которого нет в `ObjStruct`, это считается internal/runtime error, а не допустимым способом создать новое поле динамически.

### Invalid Pointer Target

Invalid pointer target означает, что pointer value:

- указывает на объект неверного вида для выполняемой операции;
- указывает на storage с недопустимым lifetime;
- содержит повреждённый или semantically impossible target.

Такая ситуация не должна возникать в корректной программе после успешного semantic analysis. Если возникает, VM завершает выполнение с runtime error.

### Multiple Return Arity Mismatch

Arity mismatch для multiple return values не должен доходить до runtime.

Примеры semantic errors:

```cetus
value := stack.Pop();
return 1;
return 1, true, 3;
```

Compiler обязан отлавливать такие случаи в semantic analyzer до codegen.
