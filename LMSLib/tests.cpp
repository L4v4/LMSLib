#include "tests.hpp"

#include <iostream>
#include <string>
#include <chrono>

#include <cassert>

#include "Enumerable.hpp"

namespace linq
{
	void runTests(void)
	{
		using s_clock = std::chrono::steady_clock;

		s_clock::time_point begin, end;

		std::cout << "Running tests..." << std::endl;

		// Start time measurement
		begin = s_clock::now();

		// int initialization
		std::array list{ 1, 2, 3, 4 };
		Enumerable enumerable_list(list);
		// empty int initialization
		std::vector<int> empty{};
		Enumerable enumerable_empty(empty);
		// string intialization
		std::array<std::string, 9> sentence{ "the", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog" };
		Enumerable enumerable_sentence(sentence);

		// Test foreach(), Select() and Where()
		std::vector<int> foreachTestVec;
		std::vector foreachCompVec = { 6, 8 };
		enumerable_list
			.Select([](int x) { return long(x * 2); })
			.Where([](int x) { return x > 5; })
			.foreach([&](int x) { foreachTestVec.push_back(x); });
		assert(foreachTestVec == foreachCompVec);

		// Tests Aggregate()
		assert(enumerable_list.Aggregate([](int x, int y) { return x + y; }) == 10);

		// Tests All()
		assert(!enumerable_list.All([](int x) { return x > 5; }));
		assert(enumerable_list.All([](int x) { return x < 100; }));
		assert(enumerable_empty.All([](int x) { return x % 1; }));

		// Tests Any()
		assert(!enumerable_empty.Any());
		assert(enumerable_list.Any([](int x) { return x > 3; }));
		assert(!enumerable_list.Any([](int x) { return x > 100; }));

		// Tests Append()
		auto appended_enumerable = enumerable_list.Append(5);
		assert(appended_enumerable.Any([](int x) { return x == 5; }));
		assert(appended_enumerable.Count() == 5);

		// Test Average()
		assert(enumerable_list.Average() == 2.5);

		// Test Cast()
		std::array castTestList{ (float)1, (float)2, (float)3, (float)4 };
		Enumerable castTestEnum(castTestList);
		assert(enumerable_list.Cast<float>().SequenceEqual(castTestEnum));

		// Test Concat()
		std::array concatTestList{ 5, 6, 7 };
		std::array concatCompList{ 1, 2, 3, 4, 5, 6, 7 };
		Enumerable concatTestEnum(concatTestList);
		Enumerable concatCompEnum(concatCompList);
		assert(concatCompEnum.SequenceEqual(enumerable_list.Concat(concatTestEnum)));

		// Test Contains()
		assert(enumerable_list.Contains(2));
		assert(!enumerable_list.Contains(999));
		assert(!enumerable_empty.Contains(999));

		// Tests Count()
		assert(enumerable_list.Count() == 4);

		// Test Distinct()
		std::array distinctTestList{ 1, 1, 2, 3, 3, 4 };
		Enumerable distinctTestEnum(distinctTestList);
		assert(enumerable_list.SequenceEqual(distinctTestEnum.Distinct()));

		// Test ElementAt()
		assert(enumerable_list.ElementAt(0) == 1);

		// Test Empty()
		std::array<int, 0> emptyTestList;
		Enumerable emptyTestEnum(emptyTestList);
		assert(emptyTestEnum.SequenceEqual(Enumerable<int>::Empty()));

		// Test Except()
		std::array<int, 2> exceptTestList{ 1, 3 };
		std::array<int, 2> exceptCompList{ 2, 4 };
		Enumerable exceptTestEnum(exceptTestList);
		Enumerable exceptCompEnum(exceptCompList);
		assert(exceptCompEnum.SequenceEqual(enumerable_list.Except(exceptTestEnum)));

		// Test First()
		assert(enumerable_list.First() == 1);
		assert(enumerable_list.First([](int x) {return x > 2; }) == 3);

		// Test Intersect()
		std::array intersectTestList{ 1, 3, 999 };
		std::array intersectCompList{ 1, 3 };
		Enumerable intersectTestEnum(intersectTestList);
		Enumerable intersectCompEnum(intersectCompList);
		assert(intersectCompEnum.SequenceEqual(enumerable_list.Intersect(intersectTestEnum)));

		// Test Last()
		assert(enumerable_list.Last() == 4);
		assert(enumerable_list.Last([](int x) {return x < 4; }) == 3);

		// Test Max()
		assert(enumerable_list.Max() == 4);

		// Test Min()
		assert(enumerable_list.Min() == 1);

		// Test Prepend()
		auto prependable_enumerable = enumerable_list.Prepend(0);
		assert(prependable_enumerable.Any([](int x) { return x == 0; }));
		assert(prependable_enumerable.Count() == 5);

		// Test Range()
		std::array rangeTestList{ 1, 2, 3, 4, 5 };
		Enumerable rangeTestEnum(rangeTestList);
		assert(rangeTestEnum.SequenceEqual(Enumerable<int>::Range(1, 5)));

		// Test Repeat()
		std::array repeatTestList{ 0, 0, 0 };
		Enumerable repeatTestEnum(repeatTestList);
		assert(repeatTestEnum.SequenceEqual(Enumerable<int>::Repeat(0, 3)));

		// Test Reverse()
		std::array reverseTestList{ 4, 3, 2, 1 };
		Enumerable reverseTestEnum(reverseTestList);
		assert(enumerable_list.Reverse().SequenceEqual(reverseTestEnum));

		// Test SequenceEqual()
		std::array equalTestList = { 1, 2, 3 };
		Enumerable equalTestEnum(equalTestList);
		assert(!enumerable_list.SequenceEqual(equalTestEnum));
		equalTestEnum = equalTestEnum.Append(4);
		assert(enumerable_list.SequenceEqual(equalTestEnum));

		// Test Single()
		std::array singleTestList{ 1 };
		Enumerable singleTestEnum(singleTestList);
		assert(singleTestEnum.Single() == 1);
		assert(enumerable_list.Single([](int x) {return x < 2; }) == 1);

		// Test Skip()
		std::array skipTestList{ 2, 3, 4 };
		Enumerable skipTestEnum(skipTestList);
		assert(enumerable_list.Skip(1).Count() == 3);
		assert(enumerable_list.Skip(1).SequenceEqual(skipTestEnum));

		// Test SkipLast()
		std::array skipLastTestList{ 1, 2, 3, 4, 5, 6 };
		Enumerable skipLastTestEnum(skipLastTestList);
		assert(enumerable_list.SequenceEqual(skipLastTestEnum.SkipLast(2)));

		// Test SkipWhile()
		std::array skipWhileTestList{ -1, 0, 1, 2, 3, 4 };
		Enumerable skipWhileTestEnum(skipWhileTestList);
		assert(enumerable_list.SequenceEqual(
			skipWhileTestEnum.SkipWhile([](int x) {return x < 1; })
		));
		assert(enumerable_list.SequenceEqual(
			skipWhileTestEnum.SkipWhile([](int x, int i) { return i < 2; })
		));

		// Test Sum()
		assert(enumerable_list.Sum() == 10);

		// Tests Take()
		std::array takeTestList{ 1, 2, 3 };
		Enumerable takeTestEnum(takeTestList);
		assert(enumerable_list.Take(3).Count() == 3);
		assert(enumerable_list.Take(3).SequenceEqual(takeTestEnum));

		// Test TakeLast()
		std::array takeLastTestList{ 2, 3, 4 };
		Enumerable takeLastTestEnum(takeLastTestList);
		assert(enumerable_list.TakeLast(3).Count() == 3);
		assert(enumerable_list.TakeLast(3).SequenceEqual(takeLastTestEnum));

		// Test TakeWhile()
		std::array takeWhileTestList{ 1, 2, 3, 4, 5, 6 };
		Enumerable takeWhileTestEnum(takeWhileTestList);
		assert(enumerable_list.SequenceEqual(
			takeWhileTestEnum.TakeWhile([](int x) {return x < 5; })
		));
		assert(enumerable_list.SequenceEqual(
			takeWhileTestEnum.TakeWhile([](int x, int i) {return i < 4; })
		));

		// Test Union()
		std::array u1TestList{ 1, 2, 1 };
		std::array u2TestList{ 2, 3, 1, 4 };
		Enumerable u1TestEnum(u1TestList);
		Enumerable u2TestEnum(u2TestList);
		assert(enumerable_list.SequenceEqual(u1TestEnum.Union(u2TestEnum)));

		// Test Zip
		std::array zip1List{ 1, 2, 3 };
		std::array<std::string, 3> zip2List{ "One", "Two", "Three" };
		std::array<std::string, 3> zipCompList{ "1 One", "2 Two", "3 Three" };
		Enumerable zip1Enum(zip1List);
		Enumerable zip2Enum(zip2List);
		Enumerable zipCompEnum(zipCompList);
		assert(zipCompEnum.SequenceEqual(zip1Enum.Zip(zip2Enum, [](int first, std::string second)
		{
			return std::to_string(first) + " " + second;
		})));

		// Test Take() and Skip() complementation
		assert(enumerable_list.SequenceEqual(enumerable_list.Take(2).Concat(enumerable_list.Skip(2))));

		// Test TakeWhile() and SkipWhile() complementation
		assert(enumerable_list.SequenceEqual(
			enumerable_list.TakeWhile([](int x) {return x < 2; }).Concat(
				enumerable_list.SkipWhile([](int x) {return x < 2; })
			)
		));
		assert(enumerable_list.SequenceEqual(
			enumerable_list.TakeWhile([](int x, int i) {return i > 2; }).Concat(
				enumerable_list.SkipWhile([](int x, int i) {return i > 2; })
			)
		));

		// End time measurement
		end = s_clock::now();

		std::cout << "All tests passed successfully!" << std::endl
			<< "This took " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " microseconds." << std::endl;
	}
}  // namespace linq
