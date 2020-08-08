#include "pch.h"

#include "../crosswordBuilder/CrosswordBuilder.h"

#include <fstream>

#include "../crosswordBuilder/WordRandomizer.h"
#include "../crosswordBuilder/Utils.h"
#include "testUtils.h"

using namespace Utils;

TEST(crosswordBuilderTest, buildTest)
{
	auto test = [](std::vector<std::string>& inp, int limit = 30)
	{
		toUpper(inp.begin(), inp.end());
		auto res = CrosswordBuilder::build(inp, limit);
		ASSERT_TRUE(res);
		ASSERT_EQ(res.value().getWordCount(), inp.size());
	};

	test(std::vector<std::string>{ "albatross", "biddy", "blackbird", "canary", "crow", "cuckoo", "dove", "pigeon", "duck", "eagle", "falcon", "finch",
		"flamingo", "goose", "gull", "hawk", "jackdaw", "jay", "kestrel", "kookaburra", "mallard", "nightingale", "nuthatch", "ostrich", "owl", "parakeet",
		"parrot", "peacock", "pelican", "penguin", "pheasant", "piranha", "raven", "robin", "rooster", "sparrow", "stork", "swallow", "swan",
		"swift", "tit", "turkey", "vulture", "woodpecker", "wren" });
	test(std::vector<std::string>{ "hedgehog", "deer", "snake", "squirrel", "rhinoceros", "tiger", "hare",
		"elephant", "crocodile", "whale", "eagle", "monkey", "shark", "lizard", "owl", "rat" }, 15);
	test(std::vector<std::string>{"barbel", "carp", "cod", "crab", "eel", "goldfish", "haddock", "halibut", "jellyfish", "lobster", "perch",
		"pike", "plaice", "ray", "salmon", "sawfish", "scallop", "shark", "shell", "shrimp", "trout"}, 15);
	test(std::vector<std::string>{ "exempt", "assist", "improvement", "merchant", "clue", "raise", "entry", "need", "pretense", "transparent",
		"mature", "beggar", "enigma", "permanent", "prophesy", "being", "seek", "tolerance", "talker", "inform", "fair", "erase", "initiate", "letter" });
	test(std::vector<std::string>{"later", "variation", "happen", "suffering", "rail", "stomach", "intend", "successful", "child", "violent",
		"organize", "shop", "ideally", "further", "notice"});
	test(std::vector<std::string>{"contain", "worker", "mud", "they", "dry", "bite", "pack", "minimum", "fish", "shelf", "all", "ask", "brother",
		"favor", "urgent"});
	test(std::vector<std::string>{"topic", "intention", "impatient", "correctly", "score", "translation", "tend", "covering", "shoulder", "pair",
		"bay", "my", "careless", "center", "weight"});
}

TEST(crosswordBuilderTest, impossibleToBuildTest)
{
	auto test = [](std::vector<std::string>& inp)
	{
		toUpper(inp.begin(), inp.end());
		testUtils::testException([&inp]() { CrosswordBuilder::build(inp); }, "Impossible to intersect these words.");
	};

	test(std::vector<std::string>{"veldt", "jynx", "grimps", "waqf", "zho", "buck"});
	testUtils::testException([]() { CrosswordBuilder::build(toUpper({"sophistication", "cat"}), 12); }, "The input consists too long word: SOPHISTICATION");
}

TEST(crosswordBuilderTest, randomBuilds)
{
	std::ifstream inp{ "OXFORD 3000 AMERICAN WORDS to RUSSIAN2.txt" };
	std::string str;
	std::vector < std::string > input;
	if (inp.is_open())
	{
		for (int i = 0; i < 3000; ++i)
		{
			std::getline(inp, str);
			std::stringstream ss{ str };
			std::string s;
			ss >> s;
			if (s.size() > 1 && std::all_of(s.begin(), s.end(), [](auto ch) {return ch >= 'a' && ch <= 'z'; })) input.push_back(s);
		}
	}
	std::unordered_set<std::string> set;
	for (const auto& word : input) set.insert(word);
	input.resize(set.size());
	std::copy(set.begin(), set.end(), input.begin());


	for (int i = 0; i < 10; ++i)
	{
		static constexpr std::size_t SIZE = 50;
		WordRandomizer::shuffleNFirstWords(input, SIZE);
		std::vector<std::string> w(input.begin(), std::next(input.begin(), SIZE));
		Utils::toUpper(w.begin(), w.end());
		if (isPossibileToIntersect(w))
		{
			auto res = CrosswordBuilder::build(w);
			ASSERT_TRUE(res.has_value());
			ASSERT_EQ(SIZE, res.value().getWordCount());
		}
	}
}
