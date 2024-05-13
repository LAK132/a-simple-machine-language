#ifndef ASML_EXTENDED_INTERPRETER_HPP
#define ASML_EXTENDED_INTERPRETER_HPP

#include "asml/float8.hpp"

#include "lak/array.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

/*

Modified instruction set

16 8bit registers
256 8bit memory cells

r_?: register
m_??: memory cell
imm_??: immediate value

OPCODE OPERAND MNEMONIC DESCRIPTION
0      000     HALT
1      RXY     LOAD     r_R = m_XY
2      RXY     LOAD     r_R = imm_XY
3      RXY     STORE    m_XY = r_R
4      0RS     MOVE     r_S = r_R
5      RST     ADD      r_R = r_S + r_T (2's complement)
6      RST     ADD      r_R = r_S + r_T (float)
7      RST     OR       r_R = r_S | r_T
8      RST     AND      r_R = r_S & r_T
9      RST     XOR      r_R = r_S ^ r_T
A      R0X     ROTATE   r_R = (r_R >> imm_X) | (r_R << (bits - imm_X))
B      RXY     JUMP     if (r_R == m_0) PC = m_XY
C      R0X     GET      r_R = m_(r_X)
D      R0X     PUT      m_(r_X) = r_R

*/

namespace asml
{
	namespace extended
	{
		enum struct opcode : uint8_t
		{
			HALT     = 0x0,
			LOAD_MEM = 0x1,
			LOAD_IMM = 0x2,
			STORE    = 0x3,
			MOVE     = 0x4,
			ADD_I    = 0x5,
			ADD_F    = 0x6,
			OR       = 0x7,
			AND      = 0x8,
			XOR      = 0x9,
			ROTATE   = 0xA,
			JUMP     = 0xB,
			GET      = 0xC,
			PUT      = 0xD,
		};

		template<opcode op>
		requires(op == opcode::LOAD_MEM || op == opcode::LOAD_IMM ||
		         op == opcode::STORE || op == opcode::JUMP)
		constexpr uint16_t inst(uint8_t r, uint8_t xy)
		{
			return (static_cast<uint16_t>(op) << 12U) | ((uint16_t(0xF & r)) << 8U) |
			       uint16_t(0xFF & xy);
		}

		template<opcode op>
		requires(op == opcode::ADD_I || op == opcode::ADD_F || op == opcode::OR ||
		         op == opcode::AND || op == opcode::XOR)
		constexpr uint16_t inst(uint8_t r, uint8_t s, uint8_t t)
		{
			return (static_cast<uint16_t>(op) << 12U) | ((uint16_t(0xF & r)) << 8U) |
			       (uint16_t(0xF & s) << 4U) | uint16_t(0xF & t);
		}

		template<opcode op>
		requires(op == opcode::MOVE)
		constexpr uint16_t inst(uint8_t r, uint8_t s)
		{
			return (static_cast<uint16_t>(op) << 12U) | (uint16_t(0xF & r) << 4U) |
			       uint16_t(0xF & s);
		}

		template<opcode op>
		requires(op == opcode::ROTATE || op == opcode::GET || op == opcode::PUT)
		constexpr uint16_t inst(uint8_t r, uint8_t x)
		{
			return (static_cast<uint16_t>(op) << 12U) | (uint16_t(0xF & r) << 8U) |
			       uint16_t(0xF & x);
		}

		template<opcode op>
		requires(op == opcode::HALT)
		constexpr uint16_t inst()
		{
			return (static_cast<uint16_t>(op) << 12U);
		}

		lak::result<lak::monostate, size_t> simulate(
		  lak::span<uint8_t, 16> reg,
		  lak::span<uint8_t, 256> mem,
		  lak::span<const uint16_t> program);
	}
}

#endif
