#include "common.h"
#include "debug/debug.h"
#include "types/chunk.h"
#include "vm.h"

int main(int argc, const char* argv[])
{
	VM vm;
	Chunk chunk;
	const int constant = chunk.AddConstant(Value(1.2));
	const int constant2 = chunk.AddConstant(Value(3.14));
	chunk.Write(OP_CONSTANT, 1);
	chunk.Write(constant, 1);
	chunk.Write(OP_CONSTANT, 2);
	chunk.Write(constant2, 2);
	chunk.Write(OP_RETURN, 1);
	vm.Interpret(chunk);
	return 0;
}