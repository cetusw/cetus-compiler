#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int argc, const char* argv[])
{
	Chunk chunk;
	chunk.Write(OP_RETURN, 1);
	debug::DisassembleChunk(chunk, "test chunk");
	return 0;
}