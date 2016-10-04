#include "Detour.hpp"
#include "Disassembler.hpp"
#include <windows.h>
#include <cstring>

size_t FindMinInstructionLength(uintptr_t address, size_t minLength)
{
	auto length = 0;
	while (length < minLength)
	{
		auto data = Disassembler::Disassemble(address + length);
		if (data.Length == 0)
		{
			break;
		}
		length += data.Length;
	}
	return length;
}
//---------------------------------------------------------------------------
uintptr_t HookFunction32(uintptr_t src, uintptr_t dst)
{
	const size_t TRAMPOLINE_SIZE = 5;

	auto length = FindMinInstructionLength(src, 5);

	union
	{
		void* srcPtr;
		uint8_t* src8;
		uint32_t* src32;
	};
	srcPtr = reinterpret_cast<void*>(src);

	DWORD protection;
	VirtualProtect(srcPtr, length, PAGE_EXECUTE_READWRITE, &protection);

	union
	{
		void* jmpPtr;
		uint8_t* jmp8;
		uint32_t* jmp32;
	};

	auto jmp = new uint8_t[length + TRAMPOLINE_SIZE];
	jmp8 = jmp;

	std::memcpy(jmpPtr, srcPtr, length);
	jmp8 += length;
	*jmp8++ = 0xE9;
	*jmp32 = src - reinterpret_cast<uintptr_t>(jmp) - 5;

	VirtualProtect(jmp, length + TRAMPOLINE_SIZE, PAGE_EXECUTE_READWRITE, &protection);

	*src8++ = 0xE9;
	*src32 = (dst - src) - 5;

	return reinterpret_cast<uintptr_t>(jmp);
}
//---------------------------------------------------------------------------
uintptr_t HookFunction64(uintptr_t src, uintptr_t dst)
{
	const size_t TRAMPOLINE_SIZE = 12;

	auto length = FindMinInstructionLength(src, 12);

	union
	{
		void* srcPtr;
		uint8_t* src8;
		uint64_t* src64;
	};
	srcPtr = reinterpret_cast<void*>(src);

	DWORD protection;
	VirtualProtect(srcPtr, length, PAGE_EXECUTE_READWRITE, &protection);

	union
	{
		void* jmpPtr;
		uint8_t* jmp8;
		uint64_t* jmp64;
	};

	auto jmp = new uint8_t[length + TRAMPOLINE_SIZE];
	jmp8 = jmp;

	std::memcpy(jmpPtr, srcPtr, length);
	jmp8 += length;
	*jmp8++ = 0x48;
	*jmp8++ = 0xB8;
	*jmp64++ = src + length;
	*jmp8++ = 0xFF;
	*jmp8++ = 0xE0;

	VirtualProtect(jmp, length + TRAMPOLINE_SIZE, PAGE_EXECUTE_READWRITE, &protection);

	*src8++ = 0x48;
	*src8++ = 0xB8;
	*src64++ = dst;
	*src8++ = 0xFF;
	*src8++ = 0xE0;

	return reinterpret_cast<uintptr_t>(jmp);
}
//---------------------------------------------------------------------------
uintptr_t HookFunction(uintptr_t src, uintptr_t dst)
{
#ifdef _WIN64
	return HookFunction64(src, dst);
#else
	return HookFunction32(src, dst);
#endif
}
//---------------------------------------------------------------------------
