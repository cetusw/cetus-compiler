cmake_minimum_required(VERSION 4.0)
project(cetus_compiler)

set(CMAKE_CXX_STANDARD 20)

include_directories(vm)

set(COMMON_SOURCES
        vm/types/OpCode.h
        vm/types/chunk.cpp
        vm/types/chunk.h
        vm/types/value.cpp
        vm/types/value.h
        vm/vm.cpp
        vm/vm.h
        vm/objects/Obj.cpp
        vm/objects/Obj.h
        vm/objects/ObjString.cpp
        vm/objects/ObjString.h
        vm/disassembler/Disassembler.cpp
        vm/disassembler/Disassembler.h
        vm/instructions/ConstantInstruction.cpp
        vm/instructions/ReturnInstruction.cpp
        vm/instructions/NegateInstruction.cpp
        vm/instructions/PopInstruction.cpp
        vm/instructions/GetLocalInstruction.cpp
        vm/instructions/SetLocalInstruction.cpp
)

add_executable(cetus_compiler vm/main.cpp ${COMMON_SOURCES})

add_executable(bytecode_parser
        vm/parser_main.cpp
        vm/parser/BytecodeParser.cpp
        vm/parser/BytecodeParser.h
        ${COMMON_SOURCES}
        vm/parser/BytecodeParser.cpp
        vm/parser/BytecodeParser.h
)

remove_definitions(-D_GLIBCXX_DEBUG)
