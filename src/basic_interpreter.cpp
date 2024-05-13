#include "asml/basic_interpreter.hpp"

lak::result<lak::monostate, size_t> asml::basic::simulate(
  lak::span<uint8_t, 16> reg,
  lak::span<uint8_t, 256> mem,
  lak::span<const uint16_t> program)
{
	if (program.size() > UINT8_MAX) return lak::err_t{program.size()};

	for (size_t pc = 0; pc < program.size();)
	{
		const uint16_t inst = program[pc];
		const opcode op =
		  static_cast<opcode>(0xF & static_cast<uint8_t>(inst >> 12U));
		const uint8_t operand_0  = 0xF & static_cast<uint8_t>(inst >> 8U);
		const uint8_t operand_1  = 0xF & static_cast<uint8_t>(inst >> 4U);
		const uint8_t operand_2  = 0xF & static_cast<uint8_t>(inst);
		const uint8_t operand_12 = static_cast<uint8_t>(inst);

		switch (op)
		{
			case opcode::LOAD_MEM:
				reg[operand_0] = mem[operand_12];
				break;

			case opcode::LOAD_IMM:
				reg[operand_0] = operand_12;
				break;

			case opcode::STORE:
				mem[operand_12] = reg[operand_0];
				break;

			case opcode::MOVE:
				reg[operand_2] = reg[operand_1];
				break;

			case opcode::ADD_I:
				reg[operand_0] = reg[operand_1] + reg[operand_2];
				break;

			case opcode::ADD_F:
				reg[operand_0] =
				  uint8_t(asml::float8(reg[operand_1]) + asml::float8(reg[operand_2]));
				break;

			case opcode::OR:
				reg[operand_0] = reg[operand_1] | reg[operand_2];
				break;

			case opcode::AND:
				reg[operand_0] = reg[operand_1] & reg[operand_2];
				break;

			case opcode::XOR:
				reg[operand_0] = reg[operand_1] ^ reg[operand_2];
				break;

			case opcode::ROTATE:
				reg[operand_0] = (reg[operand_0] >> operand_12) |
				                 (reg[operand_0] << (8U - operand_12));
				break;

			case opcode::HALT:
				return lak::ok_t{};

			case opcode::JUMP:
				if (reg[operand_0] == reg[0U])
				{
					if (operand_12 >= program.size()) [[unlikely]]
						return lak::err_t{pc};
					pc = operand_12;
					continue;
				}
				break;

			default:
				return lak::err_t{pc};
		}

		++pc;
	}

	return lak::ok_t{};
}