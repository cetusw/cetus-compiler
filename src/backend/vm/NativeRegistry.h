#pragma once

class VM;

class NativeRegistry
{
public:
	static void RegisterStdLib(VM& vm);
};
