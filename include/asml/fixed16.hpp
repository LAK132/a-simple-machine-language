#ifndef ASML_FIXED16_HPP
#define ASML_FIXED16_HPP

#include "lak/stdint.hpp"
#include "lak/streamify.hpp"

namespace asml
{
	struct fixed16
	{
		int16_t whole : 9 = 0;
		uint16_t part : 7 = 0;

		constexpr fixed16()                           = default;
		constexpr fixed16(const fixed16 &)            = default;
		constexpr fixed16 &operator=(const fixed16 &) = default;

		explicit constexpr fixed16(int16_t packed)
		: whole(0b1'1111'1111 & (packed >> 7U)), part(0b111'1111 & packed)
		{
		}

		explicit constexpr operator int16_t() const
		{
			return (int16_t(whole) << 7U) | (int16_t(part));
		}

		constexpr fixed16 operator+(const fixed16 &rhs) const
		{
			return fixed16(int16_t(*this) + int16_t(rhs));
		}

		constexpr fixed16 operator-(const fixed16 &rhs) const
		{
			return fixed16(int16_t(*this) - int16_t(rhs));
		}

		friend std::ostream &operator<<(std::ostream &strm, const fixed16 &value)
		{
			strm << std::hex << std::noshowbase << std::uppercase;
			return (value.whole < 0 ? strm << "-0x" << -value.whole
			                        : strm << "0x" << value.whole)
			       << "." << std::setfill('0') << std::setw(2) << (value.part << 1U);
		}

		constexpr bool operator==(const fixed16 &rhs) const
		{
			return whole == rhs.whole && part == rhs.part;
		}

		constexpr bool operator!=(const fixed16 &rhs) const
		{
			return !(*this == rhs);
		}
	};
}

static_assert(lak::concepts::streamable<asml::fixed16>);

#endif
