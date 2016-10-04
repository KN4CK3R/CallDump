#include "Disassembler.hpp"

Disassembler::DisassembledData Disassembler::Disassemble(uintptr_t address)
{
	DISASM disasm;
	std::memset(&disasm, 0, sizeof(DISASM));
	disasm.Options = NasmSyntax + PrefixedNumeral;
	disasm.EIP = address;
#ifdef _WIN64
	disasm.Archi = 64;
#endif

	auto length = Disasm(&disasm);
	if (length != UNKNOWN_OPCODE)
	{
		return DisassembledData{ address, (size_t)length, disasm.CompleteInstr };
	}
	return DisassembledData{ address, 0 };
}
//---------------------------------------------------------------------------
Disassembler::DisassembledData Disassembler::PreviousOpCode(uintptr_t address)
{
	DisassembledData retn;

	auto x = PreviousOpCodeHelper(address, 80, retn);
	if (x != address)
	{
		x = PreviousOpCodeHelper(address, 40, retn);
		if (x != address)
		{
			x = PreviousOpCodeHelper(address, 20, retn);
			if (x != address)
			{
				x = PreviousOpCodeHelper(address, 10, retn);
				if (x != address)
				{
					for (auto i = 1; i < 20; ++i)
					{
						x = address - i;
						auto data = Disassemble(x);
						if (data.Length > 0)
						{
							retn = data;
							break;
						}
					}
				}
			}
		}
	}

	return retn;
}
//---------------------------------------------------------------------------
uintptr_t Disassembler::PreviousOpCodeHelper(uintptr_t address, int distance, DisassembledData& result)
{
	auto x = address - distance;
	while (x < address)
	{
		result = Disassemble(x);
		if (result.Length > 0)
		{
			x += result.Length;
		}
		else
		{
			break;
		}
	}

	return x;
}
//---------------------------------------------------------------------------
