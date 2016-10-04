// CallDump
// by KN4CK3R
//
// A tool to dump informations about the caller of a function.
// Use case: Find out what calls a function if you can't attach a debugger.

#include <windows.h>
#include <fstream>
#include <deque>
#include <tlhelp32.h>
#include <tuple>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdlib>

#include "Disassembler.hpp"
#include "Detour.hpp"

//---------------------------------------------------------------------------
const auto DUMP_FILE_PATH = "C:/retn_dump.txt";
const auto INSTRUCTIONS_COUNT = 8;
const auto HEX_DUMP_HALF_SIZE = 104;
//---------------------------------------------------------------------------

/// <summary>
/// Collects informations about the set return address and writes them into the log file.
/// </summary>
/// <param name="address">The address to dump.</param>
void DumpAddressInfo(uintptr_t address);

/// <summary>
/// Returns the module the given address is located in.
/// </summary>
/// <param name="address">The address to check for.</param>
/// <returns>A tuple with the name of the module and the base address.</returns>
std::tuple<std::string, uintptr_t> FindModule(uintptr_t address);

/// <summary>
/// Disassembles <paramref name="countPreviousAndNext"> instructions located in before and after the given address.
/// </summary>
/// <param name="address">The (middle) address to disassemble.</param>
/// <param name="countPreviousAndNext">The count of instructions before and after the address to disassemble.</param>
/// <returns>A list of instructions at the given address.</returns>
std::deque<Disassembler::DisassembledData> DisassembleInstructions(uintptr_t address, size_t countPreviousAndNext);

/// <summary>
/// Creates hex dump of the given memory region.
/// </summary>
/// <param name="startAddress">The start address.</param>
/// <param name="endAddress">The end address.</param>
/// <returns>The textual hex dump of the memory region.</returns>
std::string CreateHexDump(uintptr_t startAddress, uintptr_t endAddress);

//---------------------------------------------------------------------------
// These three functions can be used as hook to dump informations about the calling address.
// All of these functions will terminate the target!
//---------------------------------------------------------------------------
void __cdecl CdeclFunction()
{
	DumpAddressInfo(reinterpret_cast<uintptr_t>(_ReturnAddress()));

	std::exit(0);
}

void __stdcall StdCallFunction()
{
	DumpAddressInfo(reinterpret_cast<uintptr_t>(_ReturnAddress()));

	std::exit(0);
}

void __fastcall FastCallFunction()
{
	DumpAddressInfo(reinterpret_cast<uintptr_t>(_ReturnAddress()));

	std::exit(0);
}
//---------------------------------------------------------------------------

BOOL WINAPI DllMain(HMODULE handle, DWORD reason, PVOID reversed)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// Test: Redirect MessageBoxA to StdCallFunction and call it. This will dump the code of the DllMain function.
		HookFunction(reinterpret_cast<uintptr_t>(MessageBoxA), reinterpret_cast<uintptr_t>(StdCallFunction));
		MessageBoxA(0, "asd", "asd", 0);

		return TRUE;
	}

	return FALSE;
}
//---------------------------------------------------------------------------
void DumpAddressInfo(uintptr_t address)
{
	if (address == 0)
	{
		return;
	}

	std::ofstream o(DUMP_FILE_PATH, std::ios::app);
	o << std::hex << std::setfill('0');

	o << "Found Return Address: 0x" << address << "\n\n";

	auto module = FindModule(address);
	o << "Module: " << std::get<0>(module) << " (0x" << std::get<1>(module) << ")   Offset: 0x" << (address - std::get<1>(module)) << "\n\n";

	o << "Instructions:\n";
	auto instructions = DisassembleInstructions(address, INSTRUCTIONS_COUNT);
	for (auto&& instruction : instructions)
	{
		o << std::setw(16) << instruction.Address << "   " << instruction.Instruction << '\n';
	}

	o << "\nHex Dump:\n";
	auto hexdump = CreateHexDump(address - HEX_DUMP_HALF_SIZE, address + HEX_DUMP_HALF_SIZE);
	o << hexdump;

	o << "\n\n";
}
//---------------------------------------------------------------------------
std::tuple<std::string, uintptr_t> FindModule(uintptr_t address)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);

	MODULEENTRY32 entry = { 0 };
	entry.dwSize = sizeof(entry);
	Module32First(snapshot, &entry);
	do
	{
		auto baseAddress = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
		if (baseAddress <= address && baseAddress + entry.modBaseSize > address)
		{
			CloseHandle(snapshot);

			return std::tuple<std::string, uintptr_t>(entry.szModule, baseAddress);
		}
	} while (Module32Next(snapshot, &entry));

	CloseHandle(snapshot);

	return std::tuple<std::string, uintptr_t>(std::string(), 0);
}
//---------------------------------------------------------------------------
std::deque<Disassembler::DisassembledData> DisassembleInstructions(uintptr_t address, size_t countPreviousAndNext)
{
	std::deque<Disassembler::DisassembledData> opcodes;

	auto caller = Disassembler::PreviousOpCode(address);
	opcodes.push_back(caller);

	for (auto i = 0; i < countPreviousAndNext; ++i)
	{
		auto prev = Disassembler::PreviousOpCode(opcodes.front().Address);
		if (prev.Length > 0)
		{
			opcodes.push_front(prev);
		}
		else
		{
			break;
		}
	}
	for (auto i = 0; i < countPreviousAndNext; ++i)
	{
		auto next = Disassembler::Disassemble(opcodes.back().Address + opcodes.back().Length);
		if (next.Length > 0)
		{
			opcodes.push_back(next);
		}
		else
		{
			break;
		}
	}

	return opcodes;
}
//---------------------------------------------------------------------------
std::string CreateHexDump(uintptr_t startAddress, uintptr_t endAddress)
{
	std::ostringstream ss;
	ss << std::hex << std::setfill('0');

	while (startAddress < endAddress)
	{
		uint8_t buffer[16];
		int nread;

		for (nread = 0; nread < 16 && startAddress + nread < endAddress; nread++)
		{
			buffer[nread] = *reinterpret_cast<uint8_t*>(startAddress + nread);
		}
		if (nread == 0)
		{
			break;
		}

		ss << std::setw(16) << startAddress;

		for (int i = 0; i < 16; i++)
		{
			if (i % 8 == 0)
			{
				ss << ' ';
			}
			if (i < nread)
			{
				ss << ' ' << std::setw(2) << (unsigned)buffer[i];
			}
			else
			{
				ss << "   ";
			}
		}

		ss << "  ";
		for (int i = 0; i < nread; i++)
		{
			if (buffer[i] < 32)
			{
				ss << '.';
			}
			else
			{
				ss << buffer[i];
			}
		}

		ss << '\n';

		startAddress += 16;
	}

	return ss.str();
}
//---------------------------------------------------------------------------
