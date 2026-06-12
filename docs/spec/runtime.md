# Runtime

## Содержание

- [Назначение runtime](#назначение-runtime)
- [Состав runtime](#состав-runtime)
- [Runtime Objects](#runtime-objects)
- [Встроенные операции](#встроенные-операции)
- [Ошибки выполнения](#ошибки-выполнения)

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

> TODO
>
> Требуется определить классификацию runtime-ошибок: деление на ноль, выход за границы, null/nil-доступ, сбой assertion.
