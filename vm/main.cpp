#include "types/OpCode.h"
#include "types/chunk.h"
#include "vm.h"

int main(int argc, const char* argv[])
{
	VM vm;
	Chunk chunk;
	const int constant = chunk.AddConstant(Value(100));
	const int constant2 = chunk.AddConstant(Value(7));
	const int constant3 = chunk.AddConstant(Value(3));
	const int constant4 = chunk.AddConstant(Value(45));
	const int constant5 = chunk.AddConstant(Value(5));
	const int constant6 = chunk.AddConstant(Value(2));

	chunk.Write(OP_CONSTANT, 1);
	chunk.Write(constant, 1);
	chunk.Write(OP_CONSTANT, 2);
	chunk.Write(constant2, 2);
	chunk.Write(OP_ADD, 3);
	chunk.Write(OP_CONSTANT, 4);
	chunk.Write(constant3, 4);
	chunk.Write(OP_SUBTRACT, 5);
	chunk.Write(OP_CONSTANT, 6);
	chunk.Write(constant4, 6);
	chunk.Write(OP_MULTIPLY, 5);
	chunk.Write(OP_CONSTANT, 5);
	chunk.Write(constant5, 5);
	chunk.Write(OP_DIVIDE, 5);
	chunk.Write(OP_CONSTANT, 6);
	chunk.Write(constant6, 6);
	chunk.Write(OP_DIVIDE, 5);
	chunk.Write(OP_RETURN, 7);
	vm.Interpret(chunk);
	return 0;
}