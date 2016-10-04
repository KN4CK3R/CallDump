#pragma once

#include <cstdint>
#include <string>

#include <beaengine/BeaEngine.h>

class Disassembler
{
public:
	struct DisassembledData
	{
		/// <summary>The address of the instruction.</summary>
		uintptr_t Address;
		/// <summary>The byte length of the instruction. If <see cref="Length"/> is 0, the data is not valid.</summary>
		size_t Length;
		/// <summary>The textual representation of the instruction.</summary>
		std::string Instruction;
	};

	/// <summary>
	/// Disassembles the instruction at the given address.
	/// </summary>
	/// <param name="address">The address to disassemble.</param>
	/// <returns>The disassembled data.</returns>
	static DisassembledData Disassemble(uintptr_t address);

	/// <summary>
	/// Disassembles the instruction which is before the given address.
	/// </summary>
	/// <param name="address">The address to check.</param>
	/// <returns>the disassembled data.</returns>
	static DisassembledData PreviousOpCode(uintptr_t address);

private:
	/// <summary>
	/// Helper method to find a valid address in front of the given address.
	/// </summary>
	/// <param name="address">The address to check.</param>
	/// <param name="distance">The distance to the desired address.</param>
	/// <param name="result">[out] The disassembled data result.</param>
	/// <returns>The address which can be reached with the given parameters.</returns>
	static uintptr_t PreviousOpCodeHelper(uintptr_t address, int distance, DisassembledData& result);
};
