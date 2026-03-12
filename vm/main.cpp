#include "disassembler/Disassembler.h"
#include "objects/ObjString.h"
#include "types/OpCode.h"
#include "types/chunk.h"
#include "vm.h"

int main(int argc, const char* argv[])
{
	VM vm;
	Chunk chunk;

	const int c15 = chunk.AddConstant(Value(15.0));
	const int c5 = chunk.AddConstant(Value(5.0));
	const int c2 = chunk.AddConstant(Value(2.0));
	const int c10 = chunk.AddConstant(Value(10.0));
	const int c0 = chunk.AddConstant(Value(0.0));
	const int bTrue = chunk.AddConstant(Value(true));
	const int bFalse = chunk.AddConstant(Value(false));

	const int sHello = chunk.AddConstant(Value(std::make_shared<ObjString>("Hello")));
	const int sSpace = chunk.AddConstant(Value(std::make_shared<ObjString>(" ")));
	const int sWorld = chunk.AddConstant(Value(std::make_shared<ObjString>("World!")));
	const int sCetus = chunk.AddConstant(Value(std::make_shared<ObjString>("Cetus")));

	// 15 + 5 = 20
	chunk.Write(OP_CONSTANT, 1);
	chunk.Write(c15, 1);
	chunk.Write(OP_CONSTANT, 1);
	chunk.Write(c5, 1);
	chunk.Write(OP_ADD, 1);

	// (20 * 2) / 10 = 4
	chunk.Write(OP_CONSTANT, 2);
	chunk.Write(c2, 2);
	chunk.Write(OP_MULTIPLY, 2);
	chunk.Write(OP_CONSTANT, 2);
	chunk.Write(c10, 2);
	chunk.Write(OP_DIVIDE, 2);

	// -4
	chunk.Write(OP_NEGATE, 3);

	// -4 < 0 = true
	chunk.Write(OP_CONSTANT, 4);
	chunk.Write(c0, 4);
	chunk.Write(OP_LESS, 4);

	// true == true = true
	chunk.Write(OP_CONSTANT, 5);
	chunk.Write(bTrue, 5);
	chunk.Write(OP_EQUAL, 5);

	// true != false = true
	chunk.Write(OP_CONSTANT, 6);
	chunk.Write(bFalse, 6);
	chunk.Write(OP_NOT_EQUAL, 6);

	// 15 % 10 = 5
	chunk.Write(OP_CONSTANT, 7);
	chunk.Write(c15, 7);
	chunk.Write(OP_CONSTANT, 7);
	chunk.Write(c10, 7);
	chunk.Write(OP_MODULO, 7);

	// 15 / 10 = 1.5
	chunk.Write(OP_CONSTANT, 8);
	chunk.Write(c15, 8);
	chunk.Write(OP_CONSTANT, 8);
	chunk.Write(c10, 8);
	chunk.Write(OP_DIVIDE, 8);

	// "Hello" + " " + "World!"
	chunk.Write(OP_CONSTANT, 2);
	chunk.Write(sHello, 2);
	chunk.Write(OP_CONSTANT, 2);
	chunk.Write(sSpace, 2);
	chunk.Write(OP_ADD, 2);
	chunk.Write(OP_CONSTANT, 2);
	chunk.Write(sWorld, 2);
	chunk.Write(OP_ADD, 2);

	// "Cetus" == "Cetus" = true
	chunk.Write(OP_CONSTANT, 3);
	chunk.Write(sCetus, 3);
	chunk.Write(OP_CONSTANT, 3);
	chunk.Write(sCetus, 3);
	chunk.Write(OP_EQUAL, 3);

	// "Cetus" != "Hello" = true
	chunk.Write(OP_CONSTANT, 4);
	chunk.Write(sCetus, 4);
	chunk.Write(OP_CONSTANT, 4);
	chunk.Write(sHello, 4);
	chunk.Write(OP_NOT_EQUAL, 4);

	chunk.Write(OP_RETURN, 9);

	const InterpretResult result = vm.Interpret(chunk);

	if (result == InterpretResult::OK)
	{
		std::printf("\nInterpretResult: OK\n");
	}
	else
	{
		std::printf("\nInterpretResult: ERROR\n");
	}

	return 0;
}