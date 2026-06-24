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

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE CETUS_SUPPORT_HEADERS CONFIGURE_DEPENDS
        src/support/*.h
)

file(GLOB_RECURSE CETUS_FRONTEND_HEADERS CONFIGURE_DEPENDS
        src/frontend/*.h
)

file(GLOB_RECURSE CETUS_BACKEND_VM_HEADERS CONFIGURE_DEPENDS
        src/backend/vm/*.h
)

file(GLOB_RECURSE CETUS_APP_CLI_HEADERS CONFIGURE_DEPENDS
        src/app/cli/*.h
)

add_library(cetus_support STATIC
        src/support/io/FileReader.cpp
        ${CETUS_SUPPORT_HEADERS}
)

add_library(cetus_frontend STATIC
        src/frontend/codegen/BytecodeEmitter.cpp
        src/frontend/codegen/CodegenVisitor.cpp
        src/frontend/codegen/FunctionContext.cpp
        src/frontend/codegen/OperatorOpcodeResolver.cpp
        src/frontend/codegen/ProgramContext.cpp
        src/frontend/lexical/LexicalAnalyzer.cpp
        src/frontend/lexical/core/SourceCursor.cpp
        src/frontend/lexical/core/TokenFactory.cpp
        src/frontend/lexical/pipeline/SemicolonInserter.cpp
        src/frontend/lexical/pipeline/TokenPostProcessor.cpp
        src/frontend/lexical/scanning/CommentSkipper.cpp
        src/frontend/lexical/scanning/KeywordClassifier.cpp
        src/frontend/lexical/scanning/LiteralScanner.cpp
        src/frontend/lexical/scanning/OperatorScanner.cpp
        src/frontend/lexical/scanning/RawTokenScanner.cpp
        src/frontend/syntax/SyntaxAnalyzer.cpp
        src/frontend/ast/debug/AstDumper.cpp
        src/frontend/ast/ASTNode.cpp
        src/frontend/ast/format/ExpressionFormatter.cpp
        src/frontend/parsergen/grammar/Grammar.cpp
        src/frontend/parsergen/grammar/GrammarLineParser.cpp
        src/frontend/parsergen/grammar/GrammarLoader.cpp
        src/frontend/parsergen/grammar/Production.cpp
        src/frontend/parsergen/grammar/SemanticTagFormatter.cpp
        src/frontend/syntax/table/Symbol.cpp
        src/frontend/parsergen/grammar/sets/GrammarSetsBuilder.cpp
        src/frontend/parsergen/grammar/sets/calculators/FirstSetCalculator.cpp
        src/frontend/parsergen/grammar/sets/calculators/FollowSetCalculator.cpp
        src/frontend/parsergen/grammar/sets/calculators/NullableSetCalculator.cpp
        src/frontend/parsergen/grammar/sets/resolvers/FirstSetResolver.cpp
        src/frontend/parsergen/grammar/transformation/transformations/AugmentStartGrammarTransformation.cpp
        src/frontend/parsergen/lalr/ActionFormatter.cpp
        src/frontend/parsergen/lalr/LALRBuilder.cpp
        src/frontend/parsergen/lalr/LR1ClosureCalculator.cpp
        src/frontend/parsergen/presentation/presenters/ConsolePresenter.cpp
        src/frontend/semantic/symbols/SymbolTable.cpp
        src/frontend/semantic/Type.cpp
        src/frontend/semantic/rules/TypeRules.cpp
        src/frontend/semantic/SemanticAnalyzer.cpp
        src/frontend/testing/PropertyGenerator.cpp
        src/frontend/testing/TestCoverageAnalyzer.cpp
        src/frontend/testing/TestDiscovery.cpp
        src/frontend/ast/building/AstReductionBuilder.cpp
        src/frontend/syntax/parser/ParserEngine.cpp
        src/frontend/syntax/parser/ParserStack.cpp
        src/frontend/syntax/diagnostics/SyntaxErrorBuilder.cpp
        src/frontend/parsergen/storage/JsonPreparedGrammarStorage.cpp
        src/frontend/syntax/parser/TokenSymbolMapper.cpp
        src/frontend/parsergen/presentation/presenters/CsvPresenter.cpp
        src/frontend/parsergen/presentation/presenters/CsvPresenter.h
        ${CETUS_FRONTEND_HEADERS}
        src/frontend/syntax/table/ParseTable.h
)

add_library(cetus_tooling STATIC
        src/frontend/parsergen/GrammarPreparator.cpp
        src/frontend/parsergen/GrammarPreparator.h
)

add_library(cetus_backend_vm STATIC
        src/backend/vm/NativeRegistry.cpp
        src/backend/vm/vm.cpp
        src/backend/vm/types/Chunk.cpp
        src/backend/vm/types/Value.cpp
        src/backend/vm/objects/Obj.cpp
        src/backend/vm/objects/ObjArray.cpp
        src/backend/vm/objects/ObjAssertionMetadata.cpp
        src/backend/vm/objects/ObjPointer.cpp
        src/backend/vm/objects/ObjRef.cpp
        src/backend/vm/objects/ObjString.cpp
        src/backend/vm/objects/ObjSlice.cpp
        src/backend/vm/objects/ObjStruct.cpp
        src/backend/vm/disassembler/Disassembler.cpp
        src/backend/vm/parser/BytecodeParser.cpp
        src/backend/vm/testing/PropertyRunner.cpp
        src/backend/vm/testing/TestRunner.cpp
        src/backend/vm/instructions/ArrayInstruction.cpp
        src/backend/vm/instructions/AssertInstruction.cpp
        src/backend/vm/instructions/CallInstruction.cpp
        src/backend/vm/instructions/ConstantInstruction.cpp
        src/backend/vm/instructions/DefineGlobalInstruction.cpp
        src/backend/vm/instructions/GetGlobalInstruction.cpp
        src/backend/vm/instructions/GetIndexInstruction.cpp
        src/backend/vm/instructions/GetLocalInstruction.cpp
        src/backend/vm/instructions/GetMemberInstruction.cpp
        src/backend/vm/instructions/GetSliceInstruction.cpp
        src/backend/vm/instructions/JumpIfFalseInstruction.cpp
        src/backend/vm/instructions/JumpInstruction.cpp
        src/backend/vm/instructions/LoopInstruction.cpp
        src/backend/vm/instructions/NegateInstruction.cpp
        src/backend/vm/instructions/NotInstruction.cpp
        src/backend/vm/instructions/OverInstruction.cpp
        src/backend/vm/instructions/PopInstruction.cpp
        src/backend/vm/instructions/PointerInstruction.cpp
        src/backend/vm/instructions/RefGlobalInstruction.cpp
        src/backend/vm/instructions/RefIndexInstruction.cpp
        src/backend/vm/instructions/RefLocalInstruction.cpp
        src/backend/vm/instructions/RefMemberInstruction.cpp
        src/backend/vm/instructions/ReturnInstruction.cpp
        src/backend/vm/instructions/SetGlobalInstruction.cpp
        src/backend/vm/instructions/SetIndexInstruction.cpp
        src/backend/vm/instructions/SetLocalInstruction.cpp
        src/backend/vm/instructions/SetMemberInstruction.cpp
        src/backend/vm/instructions/SliceInstruction.cpp
        src/backend/vm/instructions/StructInstruction.cpp
        src/backend/vm/objects/ObjFunction.cpp
        src/backend/vm/objects/ObjNative.cpp
        ${CETUS_BACKEND_VM_HEADERS}
)

add_library(cetus_app_cli STATIC
        src/app/cli/CommandLineInterface.cpp
        src/app/cli/commands/BytecodeExecutionDriver.cpp
        src/app/cli/commands/ParserDriver.cpp
        src/app/cli/commands/RunSourceDriver.cpp
        src/app/cli/commands/TableGeneratorDriver.cpp
        src/app/cli/commands/TestDriver.cpp
        src/app/cli/commands/TypeCheckDriver.cpp
        src/app/cli/testing/TestReporter.cpp
        src/app/pipeline/CompilerPipeline.cpp
        ${CETUS_APP_CLI_HEADERS}
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
        src/app/cli/utils/utils.h
        src/app/pipeline/CompilerPipeline.cpp
        src/app/pipeline/CompilerPipeline.h
)

target_include_directories(cetus PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cetus PRIVATE cetus_app_cli)

add_executable(cetus_smoke_tests
        tests/smoke/SmokeTests.cpp
)

target_include_directories(cetus_smoke_tests PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(cetus_smoke_tests
        PRIVATE
        cetus_app_cli
        GTest::gtest
        GTest::gtest_main
)

target_compile_definitions(cetus_smoke_tests
        PRIVATE
        CETUS_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

add_test(
        NAME cetus_smoke_tests
        COMMAND cetus_smoke_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(cetus_syntax_tests
        tests/syntax/SyntaxTests.cpp
)

target_include_directories(cetus_syntax_tests PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(cetus_syntax_tests
        PRIVATE
        cetus_frontend
        GTest::gtest
        GTest::gtest_main
)

add_test(
        NAME cetus_syntax_tests
        COMMAND cetus_syntax_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

if (MSVC)
    foreach(target
            cetus_support
            cetus_frontend
            cetus_tooling
            cetus_backend_vm
            cetus_app_cli
            cetus
            cetus_smoke_tests
            cetus_syntax_tests)
        target_compile_options(${target} PRIVATE /W4 /WX)
    endforeach()
else ()
    foreach(target
            cetus_support
            cetus_frontend
            cetus_tooling
            cetus_backend_vm
            cetus_app_cli
            cetus
            cetus_smoke_tests
            cetus_syntax_tests)
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endforeach()
endif ()

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    remove_definitions(-D_GLIBCXX_DEBUG)
endif ()
