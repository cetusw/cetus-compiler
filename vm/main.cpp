#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int argc, const char* argv[])
{
	Chunk chunk;
	const int constant = chunk.AddConstant(static_cast<Value>(1.2));
	chunk.Write(OP_CONSTANT, 1);
	chunk.Write(constant, 1);
	chunk.Write(OP_RETURN, 2);
	debug::DisassembleChunk(chunk, "test chunk");
	return 0;
}