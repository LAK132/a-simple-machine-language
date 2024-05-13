
#include "asml/basic_interpreter.hpp"
#include "asml/extended_interpreter.hpp"

#include "lak/debug.hpp"

#include <bit>

int main()
{
	{
		using namespace asml::basic;

		asml::fixed16 two;
		two.whole = 2;

		lak::array<uint8_t, 16> reg;
		lak::array<uint8_t, 256> mem;
		lak::array<uint16_t> program{{
		  inst<opcode::LOAD_IMM>(0, 0x12),
		  inst<opcode::LOAD_IMM>(1, 0x12),
		  inst<opcode::ADD_I>(2, 0, 1),
		  inst<opcode::STORE>(2, 0x00),
		  inst<opcode::LOAD_IMM>(3, uint8_t(asml::float8(two))),
		  inst<opcode::LOAD_IMM>(4, uint8_t(asml::float8(two))),
		  inst<opcode::ADD_F>(3, 3, 4),
		  inst<opcode::ADD_F>(3, 3, 4),
		  inst<opcode::HALT>(),
		}};

		simulate(reg, mem, program).IF_ERR("failed at");

		DEBUG_EXPR(asml::float8(reg[3]));

		DEBUG(
		  "REG: ",
		  lak::spaced_streamify(u8", ", lak::span<byte_t, 16>(lak::span(reg))));
		DEBUG(
		  "MEM: ",
		  lak::spaced_streamify(u8", ", lak::span<byte_t, 256>(lak::span(mem))));
	}

	{
		using namespace asml::extended;

		lak::array<uint8_t, 16> reg;
		lak::array<uint8_t, 256> mem;
		lak::array<uint16_t> program{{
		  inst<opcode::LOAD_IMM>(0, 0x12),
		  inst<opcode::LOAD_IMM>(1, 0x12),
		  inst<opcode::ADD_I>(2, 0, 1),
		  inst<opcode::STORE>(2, 0x00),
		  inst<opcode::LOAD_IMM>(0, 0x00),
		  inst<opcode::LOAD_IMM>(1, 0x01),
		  inst<opcode::GET>(0, 0),
		  inst<opcode::PUT>(0, 1),
		  inst<opcode::HALT>(),
		}};

		simulate(reg, mem, program).IF_ERR("failed at");

		DEBUG(
		  "REG: ",
		  lak::spaced_streamify(u8", ", lak::span<byte_t, 16>(lak::span(reg))));
		DEBUG(
		  "MEM: ",
		  lak::spaced_streamify(u8", ", lak::span<byte_t, 256>(lak::span(mem))));
	}
}
