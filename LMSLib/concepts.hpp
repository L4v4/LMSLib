#pragma once

namespace linq
{
	template <typename T>
	concept Number = std::is_integral<T>::value && !std::is_same<T, bool>::value;
}  // namespace linq
