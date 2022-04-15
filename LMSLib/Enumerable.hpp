#pragma once

#include <algorithm>
#include <array>
#include <format>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <vector>

namespace linq
{
	constexpr size_t SIZE_T_MAX = std::numeric_limits<size_t>::max();

	template <typename T>
	concept Arithmetic = std::integral<T> or std::floating_point<T>;

	template <typename T, typename U>
	concept Same = std::is_same<T, U>::value;

	template <typename T, size_t S = 0>
	class Enumerable
	{
	public:
		Enumerable(void) = delete;
		Enumerable(const T*& array, const size_t& size)
		{
			for (size_t i = 0; i < size; i++)
				_vec.push_back(array[i]);
		}
		Enumerable(const std::array<T, S>& array) : _vec(std::begin(array), std::end(array)) {}
		Enumerable(const std::vector<T>& vector) : _vec(vector) {}
		template <typename T_Key, typename T_Value, typename = std::enable_if_t<std::is_same_v<T, std::map<T_Key, T_Value>>>>
		Enumerable(const std::map<T_Key, T_Value>& map)
		{
			for (const auto& [_key, _value] : map)
			{
				std::pair _pair(_value);
				_vec.push_back(_pair);
			}
		}

		Enumerable(const Enumerable<T>& enumerable)
			: _index(enumerable._index), _vec(enumerable._vec) {}
		Enumerable(Enumerable<T>&& enumerable)
			: _index(std::move(enumerable._index)), _vec(std::move(enumerable._vec)) {}

		~Enumerable(void) = default;

		auto Aggregate(std::function<T(T, T)> func)
		{
			using std::begin, std::end;
			std::vector<T> _rest(begin(_vec) + 1, end(_vec));
			Enumerable tmp(_rest);

			return tmp.Aggregate(_vec[0], func);
		}

		auto Aggregate(T seed, std::function<T(T, T)> func)
		{
			T _aggregate = seed;

			foreach([&](T x) { _aggregate = func(_aggregate, x); });

			return _aggregate;
		}

		auto All(std::function<bool(T)> predicate)
		{
			if (_vec.size() == 0) return true;

			auto _returnValue = true;

			foreach([&](T x) { if (!predicate(x)) _returnValue = false; });

			return _returnValue;
		}

		auto Any(void) { return _vec.size() != 0; }

		auto Any(std::function<bool(T)> predicate)
		{
			if (_vec.size() == 0) return false;

			auto _returnValue = false;

			foreach([&](T x) { if (predicate(x)) _returnValue = true; });

			return _returnValue;
		}

		auto Append(T element)
		{
			std::vector<T> _newVec = _vec;

			_newVec.push_back(element);

			return Enumerable<T>(_newVec);
		}

		float Average(void)
		{
			if (_vec.size() == 0)
				return 0;

			T _sum = 0;

			foreach([&](T x) {_sum += x; });

			return _sum / static_cast<float>(_vec.size());
		}

		template <typename T_Cast>
		auto Cast(void)
		{
			using std::begin, std::end;
			std::vector<T_Cast> _newVec(begin(_vec), end(_vec));

			return Enumerable<T_Cast>(_newVec);
		}

		template <size_t S_Second>
		auto Concat(Enumerable<T, S_Second> second)
		{
			using std::begin, std::end;
			auto _firstVec = _vec;
			auto _secondVec = second._vec;

			_firstVec.insert(end(_firstVec), begin(_secondVec), end(_secondVec));

			return Enumerable<T>(_firstVec);
		}

		auto Contains(T item)
		{
			auto _contains = false;

			foreach([&](T x) {if (x == item)
			{
				_contains = true;
				return;
			} });

			return _contains;
		}

		auto Count(void) { return int(_vec.size()); }

		auto Distinct(void)
		{
			std::vector<T> _newVec;

			foreach([&](T x) { if (!In(_newVec, x)) _newVec.push_back(x); });

			return Enumerable<T>(_newVec);
		}

		auto ElementAt(int index)
		{
			if (index < 0)
				throw std::out_of_range("Enumerable<T>::ElementAt() : 'index' is less than 0");
			if (index >= Count())
				throw std::out_of_range(
					"Enumerable<T>::ElementAt() : 'index' is greater than or equal to the number of elements"
				);

			return _vec[index];
		}

		static auto Empty(void)
		{
			std::vector<T> _newVec;
			return Enumerable<T>(_newVec);
		}

		template <size_t T_Second>
		auto Except(Enumerable<T, T_Second> second)
		{
			std::vector<T> _newVec;
			auto _dist = Distinct();

			_dist.foreach([&](T x) { if (!second.Contains(x)) _newVec.push_back(x); });

			return Enumerable<T>(_newVec);
		}

		auto First(void)
		{
			InvalidOperationException(__func__);
			return _vec.front();
		}

		auto First(std::function<bool(T)> predicate)
		{
			auto _filtered = Where(predicate);
			return _filtered.First();
		}

		template <typename Func_Key, typename Func_Element, typename Func_Result>
		auto GroupBy(Func_Key keySelector, Func_Element elementSelector, Func_Result resultSelector)
		{
			using T_Func_Element = typename std::invoke_result<Func_Element>::type;
			using T_Out = typename std::invoke_result<Func_Result, T_Func_Element, Enumerable<T>>::type;

			std::vector<T> _emptyVec;
			std::vector<T_Out> _newVec;
			std::map<T_Func_Element, Enumerable> _map;

			auto _select = Select(keySelector);

			_select.foreach([&](T x)
			{
				auto [_iterator, _success] = _map.insert({ elementSelector, Enumerable(_emptyVec) });
				_iterator->second.Append(x);
			});

			for (const auto& [_key, _value] : _map)
				_newVec.push_back(resultSelector(_key, _value));

			return Enumerable<T_Out>(_newVec);
		}

		template <size_t S_Second>
		auto Intersect(Enumerable<T, S_Second> second)
		{
			std::vector<T> _newVec;

			foreach([&](T x) { if (second.Contains(x)) _newVec.push_back(x); });

			return Enumerable<T>(_newVec);
		}

		auto Last(void)
		{
			InvalidOperationException(__func__);
			return _vec.back();
		}

		auto Last(std::function<bool(T)> predicate)
		{
			auto _filtered = Where(predicate);
			return _filtered.Last();
		}

		auto Max(void)
		{
			InvalidOperationException(__func__);
			using std::begin, std::end;
			return *std::max_element(begin(_vec), end(_vec));
		}

		auto Min(void)
		{
			InvalidOperationException(__func__);
			using std::begin, std::end;
			return *std::min_element(begin(_vec), end(_vec));
		}

		auto Prepend(T element)
		{
			std::vector<T> _newVec = { element };

			foreach([&](T x) {_newVec.push_back(x); });

			return Enumerable<T>(_newVec);
		}

		static auto Range(int start, int count)
		{
			if (count < 0)
				throw std::out_of_range("Enumerable<T>::Range() : 'count' is less than 0");
			if (long(start + count - 1) > INT_MAX)
				throw std::out_of_range("Enumerable<T>::Range() : Range exeeds 'INT_MAX'");

			std::vector<int> _newVec;

			for (int i = start; i <= count; i++)
				_newVec.push_back(i);

			return Enumerable<int>(_newVec);
		}

		static auto Repeat(T element, int count)
		{
			if (count < 0)
				throw std::out_of_range("Enumerable<T>::Repeat() : 'count' is less than 0");

			std::vector<T> _newVec;

			for (int i = 0; i < count; i++)
				_newVec.push_back(element);

			return Enumerable<T>(_newVec);
		}

		auto Reverse(void)
		{
			using std::begin, std::end;
			std::vector<T> _newVec;

			foreach([&](T x) {_newVec.insert(begin(_newVec), x); });

			return Enumerable<T>(_newVec);
		}

		template<typename T_Out>
		auto Select(std::function<T_Out(T)> selector)
		{
			// using T_Out = typename std::invoke_result<Function, T>::type;

			std::vector<T_Out> _newVec;

			foreach([&](T x) { _newVec.push_back(selector(x)); });

			return Enumerable<T_Out>(_newVec);
		}

		template <size_t S_Second>
		auto SequenceEqual(Enumerable<T, S_Second> second)
		{
			auto _equal = true;
			auto _i = 0;

			if (Count() != second.Count())
				return false;

			foreach([&](T x)
			{
				if (x != second.ElementAt(_i))
				{
					_equal = false;
					return;
				}
				_i++;
			});

			return _equal;
		}

		auto Single(void)
		{
			if (Count() != 1)
				throw std::runtime_error(
					"Enumerable<T>::Single() : The input sequence contains more than one element"
				);

			return First();
		}

		template <typename Function>
		auto Single(Function predicate)
		{
			InvalidOperationException(__func__);

			auto _single = Select([](T x) {return x; }).Where(predicate);

			if (_single.Count() == 0)
				throw std::runtime_error(
					"Enumerable<T>::Single() : No element satisfies the condition in 'predicate'"
				);
			if (_single.Count() > 1)
				throw std::runtime_error(
					"Enumerable<T>::Single() : More than one element satisfies the condition in 'predicate'"
				);

			return _single.First();
		}

		auto Skip(int count)
		{
			auto _i = 0;
			std::vector<T> _newVec;

			if (count <= 0)
				return Enumerable<T>(_vec);
			if (count > Count())
				return Enumerable<T>(_newVec);

			foreach([&](T x)
			{
				if (_i >= count)
					_newVec.push_back(x);
				_i++;
			});

			return Enumerable<T>(_newVec);
		}

		auto SkipLast(int count)
		{
			std::vector<T> _newVec;

			if (count < 0)
				return Enumerable<T>(_vec);

			return Take(Count() - count);
		}

		auto SkipWhile(std::function<bool(T)> predicate)
		{
			auto _skip = true;
			std::vector<T> _newVec;

			foreach([&](T x)
			{
				if (_skip)
					_skip = predicate(x);
				if (!_skip)
					_newVec.push_back(x);
			});

			return Enumerable<T>(_newVec);
		}

		auto SkipWhile(std::function<bool(T, int)> predicate)
		{
			auto _skip = true;
			auto _i = 0;
			std::vector<T> _newVec;

			foreach([&](T x)
			{
				if (_skip)
					_skip = predicate(x, _i);
				if (!_skip)
					_newVec.push_back(x);
				_i++;
			});

			return Enumerable<T>(_newVec);
		}

		auto Sum() requires Arithmetic<T>
		{
			T _sum = T();

			foreach([&](T x) {_sum += x; });

			return _sum;
		}

		auto Take(int count)
		{
			auto _i = 0;
			std::vector<T> _newVec;

			if (count <= 0)
				return Enumerable<T>(_newVec);

			foreach([&](T x)
			{
				if (_i >= count)
					return;
				_newVec.push_back(x);
				_i++;
			});

			return Enumerable<T>(_newVec);
		}

		auto TakeLast(int count)
		{
			auto _i = 0;
			auto _start = Count() - count;
			std::vector<T> _newVec;

			if (count < 0)
				return Enumerable<T>(_newVec);

			foreach([&](T x)
			{
				if (_i >= _start)
					_newVec.push_back(x);
				_i++;
			});

			return Enumerable<T>(_newVec);
		}

		auto TakeWhile(std::function<bool(T)> predicate)
		{
			auto _take = true;
			std::vector<T> _newVec;

			foreach([&](T x)
			{
				if (_take)
					_take = predicate(x);
				if (_take)
					_newVec.push_back(x);
			});

			return Enumerable<T>(_newVec);
		}

		auto TakeWhile(std::function<bool(T, int)> predicate)
		{
			auto _take = true;
			auto _i = 0;
			std::vector<T> _newVec;

			foreach([&](T x)
			{
				if (_take)
					_take = predicate(x, _i);
				if (_take)
					_newVec.push_back(x);
				_i++;
			});

			return Enumerable<T>(_newVec);
		}

		template <size_t S_Second>
		auto Union(Enumerable<T, S_Second> second)
		{
			std::vector<T> _newVec;

			foreach([&](T x) { if (!In(_newVec, x)) _newVec.push_back(x); });
			second.foreach([&](T x) { if (!In(_newVec, x)) _newVec.push_back(x); });

			return Enumerable<T>(_newVec);
		}

		template <typename Function>
		auto Where(Function predicate)
		{
			std::vector<T> _newVec;

			foreach([&](T x) { if (predicate(x)) _newVec.push_back(x); });

			return Enumerable<T>(_newVec);
		}

		template <typename T_Second, size_t S_Second, typename Function>
		auto Zip(Enumerable<T_Second, S_Second> second, Function resultSelector)
		{
			using T_Out = typename std::invoke_result<Function, T, T_Second>::type;

			auto _i = 0;
			auto _len = Count() < second.Count() ? Count() : second.Count();
			std::vector<T_Out> _newVec;

			foreach([&](T x)
			{
				if (_i < _len)
					_newVec.push_back(resultSelector(x, second._vec[_i]));
				_i++;
			});

			return Enumerable<T_Out>(_newVec);
		}

		void Reset(void) { _index = SIZE_T_MAX; }
		bool MoveNext(void) { return _vec.size() > ++_index; }
		T Current(void) { return _vec[_index]; }

		template <typename Function>
		void foreach(Function func)
		{
			Reset();
			while (MoveNext())
				func(Current());
		}

	private:
		size_t _index = SIZE_T_MAX;
		std::vector<T> _vec;

		friend class Enumerable;

		void InvalidOperationException(std::string _mName)
		{
			if (Count() == 0)
				throw std::runtime_error(std::string(
					"Enumerable<T>::" + _mName + "() : The source sequence is empty"
				));
		}

		static inline bool In(std::vector<T> _v, T _x)
		{
			return std::find(std::begin(_v), std::end(_v), _x) != std::end(_v);
		}
	};
}  // namespace linq
