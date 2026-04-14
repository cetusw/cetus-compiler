cmake_minimum_required(VERSION 4.0)
project(cetus_compiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMAND_LINE_INTERFACE_SOURCES
        main.cpp
        src/cli/CommandLineInterface.cpp
        src/cli/driver/TableGeneratorDriver.cpp
)

set(COMMON_LOGIC_SOURCES
        src/common/grammar/Grammar.cpp
        src/common/grammar/Production.cpp
        src/common/grammar/Symbol.cpp
)

set(COMPILER_CORE_SOURCES
        src/compiler/lexer/Lexer.cpp
        src/compiler/lalr/LALRBuilder.cpp
        src/compiler/lalr/LALRBuilder.h
        src/compiler/lalr/LR1ClosureCalculator.cpp
        src/compiler/lalr/LR1ClosureCalculator.h
        src/compiler/lalr/types/LR.h
        src/compiler/lalr/types/LALR.h
)

set(UTILITY_SOURCES
        src/utils/guding-set-provider/GuidingSetProvider.cpp
        src/utils/io/GrammarLoader.cpp
        src/utils/io/TableVisualizer.cpp
)

set(VIRTUAL_MACHINE_SOURCES
        src/vm/vm.cpp
        src/vm/types/chunk.cpp
        src/vm/types/value.cpp
        src/vm/objects/Obj.cpp
        src/vm/objects/ObjString.cpp
        src/vm/disassembler/Disassembler.cpp
        src/vm/parser/BytecodeParser.cpp
        src/vm/instructions/ConstantInstruction.cpp
        src/vm/instructions/GetLocalInstruction.cpp
        src/vm/instructions/NegateInstruction.cpp
        src/vm/instructions/PopInstruction.cpp
        src/vm/instructions/ReturnInstruction.cpp
        src/vm/instructions/SetLocalInstruction.cpp
        src/utils/io/TableExporter.cpp
        src/utils/io/TableExporter.h
        src/cli/driver/BytecodeExecutionDriver.cpp
        src/cli/driver/BytecodeExecutionDriver.h
        src/vm/instructions/JumpInstruction.cpp
        src/vm/instructions/JumpInstruction.h
        src/vm/instructions/JumpIfFalseInstruction.cpp
        src/vm/instructions/JumpIfFalseInstruction.h
        src/vm/instructions/LoopInstruction.cpp
        src/vm/instructions/LoopInstruction.h
        src/vm/objects/ObjFunction.cpp
        src/vm/objects/ObjFunction.h
        src/vm/types/CallFrame.h
        src/vm/instructions/CallInstruction.cpp
        src/vm/instructions/CallInstruction.h
        src/vm/parser/Types.h
        src/vm/objects/ObjNative.cpp
        src/vm/objects/ObjNative.h
        src/vm/instructions/DefineGlobalInstruction.cpp
        src/vm/instructions/DefineGlobalInstruction.h
        src/vm/instructions/GetGlobalInstruction.cpp
        src/vm/instructions/GetGlobalInstruction.h
        src/vm/instructions/SetGlobalInstruction.cpp
        src/vm/instructions/SetGlobalInstruction.h
        src/vm/NativeFunctions.h
)

add_executable(cetus
        ${COMMAND_LINE_INTERFACE_SOURCES}
        ${COMMON_LOGIC_SOURCES}
        ${COMPILER_CORE_SOURCES}
        ${UTILITY_SOURCES}
        ${VIRTUAL_MACHINE_SOURCES}
)

target_include_directories(cetus PRIVATE ${CMAKE_SOURCE_DIR})

if (MSVC)
    target_compile_options(cetus PRIVATE /W4 /WX)
else ()
    target_compile_options(cetus PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    remove_definitions(-D_GLIBCXX_DEBUG)
endif ()