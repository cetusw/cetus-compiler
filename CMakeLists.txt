cmake_minimum_required(VERSION 3.31)
project(cetus_compiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(cetus_support STATIC
        src/support/io/FileReader.cpp
)

add_library(cetus_frontend STATIC
        src/frontend/codegen/asm/AsmProgram.cpp
        src/frontend/codegen/CodegenVisitor.cpp
        src/frontend/lexical/LexicalAnalyzer.cpp
        src/frontend/syntax/SyntaxAnalyzer.cpp
        src/frontend/syntax/ast/AstDumper.cpp
        src/frontend/syntax/ast/ASTNode.cpp
        src/frontend/syntax/analysis/guiding-set-provider/GuidingSetProvider.cpp
        src/frontend/syntax/grammar/Grammar.cpp
        src/frontend/syntax/grammar/GrammarLoader.cpp
        src/frontend/syntax/grammar/Production.cpp
        src/frontend/syntax/grammar/Symbol.cpp
        src/frontend/syntax/grammar/transformation/transformations/AugmentStartGrammarTransformation.cpp
        src/frontend/syntax/lalr/ActionFormatter.cpp
        src/frontend/syntax/lalr/LALRBuilder.cpp
        src/frontend/syntax/lalr/LR1ClosureCalculator.cpp
        src/frontend/syntax/presentation/presenters/ConsolePresenter.cpp
        src/frontend/semantic/symbols/SymbolTable.cpp
        src/frontend/semantic/rules/TypeRules.cpp
        src/frontend/semantic/SemanticAnalyzer.cpp
        src/frontend/syntax/parser/AstReductionBuilder.cpp
        src/frontend/syntax/storage/JsonPreparedGrammarStorage.cpp
        src/frontend/syntax/parser/TokenSymbolMapper.cpp
        src/frontend/codegen/asm/AsmCodegenVisitor.cpp
        src/frontend/syntax/presentation/presenters/CsvPresenter.cpp
        src/frontend/syntax/presentation/presenters/CsvPresenter.h
)

add_library(cetus_tooling STATIC
        src/frontend/syntax/GrammarPreparator.cpp
)

add_library(cetus_backend_vm STATIC
        src/backend/vm/vm.cpp
        src/backend/vm/types/Chunk.cpp
        src/backend/vm/types/Value.cpp
        src/backend/vm/objects/Obj.cpp
        src/backend/vm/objects/ObjString.cpp
        src/backend/vm/disassembler/Disassembler.cpp
        src/backend/vm/parser/BytecodeParser.cpp
        src/backend/vm/instructions/CallInstruction.cpp
        src/backend/vm/instructions/ConstantInstruction.cpp
        src/backend/vm/instructions/DefineGlobalInstruction.cpp
        src/backend/vm/instructions/GetGlobalInstruction.cpp
        src/backend/vm/instructions/GetLocalInstruction.cpp
        src/backend/vm/instructions/JumpIfFalseInstruction.cpp
        src/backend/vm/instructions/JumpInstruction.cpp
        src/backend/vm/instructions/LoopInstruction.cpp
        src/backend/vm/instructions/NegateInstruction.cpp
        src/backend/vm/instructions/NotInstruction.cpp
        src/backend/vm/instructions/PopInstruction.cpp
        src/backend/vm/instructions/ReturnInstruction.cpp
        src/backend/vm/instructions/SetGlobalInstruction.cpp
        src/backend/vm/instructions/SetLocalInstruction.cpp
        src/backend/vm/objects/ObjFunction.cpp
        src/backend/vm/objects/ObjNative.cpp
)

add_library(cetus_app_cli STATIC
        src/app/cli/CommandLineInterface.cpp
        src/app/cli/commands/BytecodeExecutionDriver.cpp
        src/app/cli/commands/ParserDriver.cpp
        src/app/cli/commands/PredefinedSymbols.cpp
        src/app/cli/commands/RunAsmExpressionDriver.cpp
        src/app/cli/commands/RunExpressionDriver.cpp
        src/app/cli/commands/TableGeneratorDriver.cpp
        src/app/cli/commands/TypeCheckDriver.cpp
)

foreach(target
        cetus_support
        cetus_frontend
        cetus_tooling
        cetus_backend_vm
        cetus_app_cli)
    target_include_directories(${target} PUBLIC ${CMAKE_SOURCE_DIR})
endforeach()

target_link_libraries(cetus_frontend PUBLIC cetus_support nlohmann_json::nlohmann_json)
target_link_libraries(cetus_tooling PUBLIC cetus_frontend cetus_backend_vm)
target_link_libraries(cetus_app_cli PUBLIC cetus_frontend cetus_tooling cetus_backend_vm)

add_executable(cetus
        src/app/main.cpp
)

target_include_directories(cetus PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cetus PRIVATE cetus_app_cli)

if (MSVC)
    foreach(target
            cetus_support
            cetus_frontend
            cetus_tooling
            cetus_backend_vm
            cetus_app_cli
            cetus)
        target_compile_options(${target} PRIVATE /W4 /WX)
    endforeach()
else ()
    foreach(target
            cetus_support
            cetus_frontend
            cetus_tooling
            cetus_backend_vm
            cetus_app_cli
            cetus)
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endforeach()
endif ()

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    remove_definitions(-D_GLIBCXX_DEBUG)
endif ()
