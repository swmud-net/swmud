/*
 * swpqxx.h
 *
 *  Created on: 2009-09-06
 *      Author: Trog
 */

#ifndef SWPQXX_H_
#define SWPQXX_H_

namespace pqxx
{
template<> struct string_traits<SWString>
{
	static constexpr bool converts_to_string{false};
	static constexpr bool converts_from_string{true};

	[[nodiscard]] static SWString from_string(std::string_view text)
	{
		return SWString(std::string{text}.c_str());
	}
};

template<> struct string_traits<SWInt>
{
	static constexpr bool converts_to_string{false};
	static constexpr bool converts_from_string{true};

	[[nodiscard]] static SWInt from_string(std::string_view text)
	{
		SWInt i;
		i.setValue(SWInt::fromString(std::string{text}.c_str()));
		return i;
	}
};
}

#endif /* SWPQXX_H_ */
