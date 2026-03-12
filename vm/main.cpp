#include "disassembler/Disassembler.h"
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

	// 15 % 10 = 5
	chunk.Write(OP_CONSTANT, 6);
	chunk.Write(c15, 6);
	chunk.Write(OP_CONSTANT, 6);
	chunk.Write(c10, 6);
	chunk.Write(OP_MODULO, 6);

	// 15 / 10 = 1.5
	chunk.Write(OP_CONSTANT, 7);
	chunk.Write(c15, 7);
	chunk.Write(OP_CONSTANT, 7);
	chunk.Write(c10, 7);
	chunk.Write(OP_DIVIDE, 7);

	chunk.Write(OP_RETURN, 8);

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