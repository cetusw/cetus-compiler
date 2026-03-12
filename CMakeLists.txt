cmake_minimum_required(VERSION 4.0)
project(cetus_compiler)

set(CMAKE_CXX_STANDARD 20)

include_directories(vm)

add_executable(cetus_compiler
        vm/common.h
        vm/main.cpp
        vm/types/chunk.h
        vm/types/chunk.cpp
        vm/types/value.cpp
        vm/types/value.h
        vm/vm.cpp
        vm/vm.h
        vm/instructions/Instruction.h
        vm/instructions/ConstantInstruction.h
        vm/instructions/InstructionRegistry.h
        vm/instructions/ReturnInstruction.h
        vm/instructions/ConstantInstruction.cpp
        vm/instructions/ReturnInstruction.cpp
        vm/types/InterpretResult.h
        vm/disassembler/Disassembler.cpp
        vm/disassembler/Disassembler.h
        vm/types/OpCode.h
        vm/instructions/NegateInstruction.cpp
        vm/instructions/NegateInstruction.h
        vm/instructions/BinaryInstruction.h
        vm/objects/Obj.h
        vm/objects/Obj.cpp
        vm/objects/ObjString.cpp
        vm/objects/ObjString.h
)

remove_definitions(-D_GLIBCXX_DEBUG)
