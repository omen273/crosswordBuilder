#include "pch.h"

#include "../crosswordBuilder/WordRandomizer.h"

#include <numeric>
#include <unordered_map>
#include <unordered_set>

#include"TestUtils.h"

using namespace testUtils;

namespace
{
	auto computeChi(const std::vector<std::size_t>& Ys, float probability, std::size_t testCount)
	{
		auto expValue = testCount * probability;
		return std::accumulate(Ys.cbegin(), Ys.end(), 0.0f,
			[testCount, expValue](auto sum, auto Y)
			{
				auto dif = Y - expValue;
				return dif * dif / expValue;
			});
	}

	struct wordsHash
	{
		template<typename T>
		[[nodiscard]] auto operator()(const std::pair<T, T>& p) const noexcept
		{
			auto first = p.first < p.second ? p.first : p.second;
			auto second = first == p.first ? p.second : p.first;
			std::size_t h1 = std::hash<T>{}(first);
			std::size_t h2 = std::hash<T>{}(second);
			return h1 ^ (h2 << 1);
		}
	};

	struct wordsEqual
	{
		template<typename T>
		[[nodiscard]] auto operator()(const std::pair<T, T>& p1, const std::pair<T, T>& p2) const noexcept
		{
			return (p1.first == p2.first && p1.second == p2.second) || (p1.first == p2.second && p1.second == p2.first);
		}
	};
}

//can fail sometimes because it depends on a random generator 
TEST(WordRandomizerTest, DISABLED_shuffleNFirstWords)
{
	std::vector<std::string> inp = { "AB", "BC", "CD" };
	std::vector<std::size_t> counts(inp.size());
	static constexpr std::size_t SIZE = 2;
	std::unordered_map<std::pair<std::string, std::string>, std::size_t, wordsHash, wordsEqual> counter;
	static constexpr std::size_t TEST_COUNT = 5000;
	for (std::size_t i = 0; i < TEST_COUNT; ++i)
	{
		WordRandomizer::shuffleNFirstWords(inp, SIZE);
		++counter[{inp[0], inp[1]}];
		
	}

	std::size_t variantCount = 3;
	std::vector<std::size_t> Ys;
	Ys.reserve(variantCount);
	transform(counter.begin(), counter.end(), std::back_inserter(Ys), [](const auto& p) {return p.second; });
	static constexpr auto MAX_CHI = 0.7713;
	auto t = computeChi(Ys, 1.0f / variantCount, TEST_COUNT);
	ASSERT_LE(computeChi(Ys, 1.0f / variantCount, TEST_COUNT), MAX_CHI);
}

//can fail sometimes because it depends on a random generator 
TEST(WordRandomizerTest, DISABLED_getRandomWords)
{
	std::vector<std::string> inp = { "AB", "BC", "CD" };
	std::vector<std::size_t> counts(inp.size());
	static constexpr std::size_t SIZE = 2;
	std::unordered_map<std::pair<std::string, std::string>, std::size_t, wordsHash, wordsEqual> counter;
	static constexpr std::size_t TEST_COUNT = 5000;
	for (std::size_t i = 0; i < TEST_COUNT; ++i)
	{
		auto res = WordRandomizer::getRandomWords(inp, SIZE);
		++counter[{res[0], res[1]}];
	}

	std::size_t variantCount = 3;
	std::vector<std::size_t> Ys;
	Ys.reserve(variantCount);
	transform(counter.begin(), counter.end(), std::back_inserter(Ys), [](const auto& p) {return p.second; });
	static constexpr auto MAX_CHI = 0.7713;
	auto t = computeChi(Ys, 1.0f / variantCount, TEST_COUNT);
	ASSERT_LE(computeChi(Ys, 1.0f / variantCount, TEST_COUNT), MAX_CHI);
}

TEST(WordRandomizerTest, getRandomWordsUniqness)
{
	std::vector<std::string> inp = { "HELLO", "WORLD", "AND", "GOOBYE", "BEAUTIFUL" };
	static constexpr std::size_t SIZE = 4;
	static constexpr std::size_t TEST_COUNT = 5000;

	for (std::size_t i = 0; i < TEST_COUNT; ++i)
	{
		auto res = WordRandomizer::getRandomWords(inp, SIZE);
		std::unordered_set<std::string> words(inp.size());
		for (const auto& word : res)
		{
			ASSERT_FALSE(words.contains(word));
			words.insert(word);
		}
	}
}

TEST(WordRandomizerTest, shuffleNFirstWordsException)
{
	std::vector<std::string> inp = { "AA", "BB" };
	testException([&inp]() {WordRandomizer::shuffleNFirstWords(inp, 2); }, "The output words' number should be more or equal to the input words' number minus one");
}

TEST(WordRandomizerTest, getRandomWordsException)
{
	testException([]() {WordRandomizer::getRandomWords({ "AA", "BB" }, 2); }, "The output words' number should be more or equal to the input words' number minus one");
}
