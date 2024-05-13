#ifndef ASML_FLOAT8_HPP
#define ASML_FLOAT8_HPP

#include "asml/fixed16.hpp"

namespace asml
{
	struct float8
	{
		int8_t sign : 1      = 0;
		int8_t exponent : 3  = 0;
		uint8_t mantissa : 4 = 0;

		constexpr float8()                          = default;
		constexpr float8(const float8 &)            = default;
		constexpr float8 &operator=(const float8 &) = default;

		explicit constexpr float8(uint8_t packed)
		: sign(int8_t(packed) >> 7U),
		  exponent((int8_t(packed << 1U) >> 5U)),
		  mantissa(packed & 0x0F)
		{
		}

		constexpr float8(fixed16 fp)
		{
			uint16_t ufp;
			if (fp.whole < 0)
			{
				sign = -1;
				ufp  = uint16_t(-int16_t(fp));
			}
			else
			{
				sign = 0;
				ufp  = uint16_t(int16_t(fp));
			}

			if (ufp >= 0b001111000'0000000)
			{
				exponent = 3;
				mantissa = 15U;
			}
			else if (ufp < 0b000000000'0001000)
			{
				exponent = -4;
				mantissa = 0U;
			}
			else
			{
				const int lz = std::countl_zero(uint8_t(ufp >> 7U));
				exponent     = 8 - (lz + 4);
				if (exponent >= 0)
					mantissa = uint8_t((ufp >> uint8_t(exponent)) >> 7U);
				else
					mantissa = uint8_t((ufp << uint8_t(-exponent)) >> 7U);
			}
		}

		explicit constexpr operator fixed16() const
		{
			return fixed16((sign ? -1 : 1) *
			               int16_t(uint16_t(mantissa) << uint8_t(exponent + 7)));
		}

		explicit constexpr operator uint8_t() const
		{
			return ((uint8_t(sign) & 0b1) << 7U) |
			       ((uint8_t(exponent) & 0b111) << 4U) | (mantissa << 0U);
		}

		constexpr float8 operator+(const float8 rhs)
		{
			return float8(fixed16(*this) + fixed16(rhs));
		}

		friend std::ostream &operator<<(std::ostream &strm, const float8 &value)
		{
			return strm << fixed16(value);
		}

		constexpr bool operator==(const float8 &rhs) const
		{
			return ((sign ? -1 : 1) * int16_t(mantissa << (exponent + 4))) ==
			       ((rhs.sign ? -1 : 1) *
			        int16_t(rhs.mantissa << (rhs.exponent + 4)));
		}

		constexpr bool operator!=(const float8 &rhs) const
		{
			return !(*this == rhs);
		}
	};

}

static_assert(lak::concepts::streamable<asml::float8>);

#endif
