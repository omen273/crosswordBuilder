#include "pch.h"

#include "../crosswordBuilder/Crossword.h"

#include <algorithm>
#include <functional>

#include "testUtils.h"
#include "../crosswordBuilder/Utils.h"
#include "../crosswordBuilder/Constants.h"

using namespace Utils;
using namespace testUtils;

TEST(CrosswordTest, insertWords)
{
	Crossword cross( "SNAKE" );
	auto insert = [&cross](const crosswordString& word, const insertionParams& params)
	{
		auto inses = cross.testWord(word);
  		auto ins = params + cross.getCoordinateStart();
		ASSERT_TRUE(std::find(inses.begin(), inses.end(), ins) != inses.end());
		cross.insertWord(word, ins.wordParams);
	};

	insert("SQUIRREL", {{{0, 0}, WordOrientation::VERTICAL}, { 0, 5, 8, 0 }, 1 });
	insert("RHINOCEROS", {{{0, 4}, WordOrientation::HORIZONTAL}, { 0, 10, 8, 0 }, 2 });
	insert("ELEPHANT", { {{0, 6}, WordOrientation::HORIZONTAL}, { 0, 10, 8, 0 }, 3 });
	insert("TIGER", { {{2, 3}, WordOrientation::VERTICAL}, { 0, 10, 8, 0 }, 5 });
	insert("HARE", { {{4, 6}, WordOrientation::VERTICAL}, { 0, 10, 10, 0 }, 6 });
	insert("CROCODILE", { {{3, 8}, WordOrientation::HORIZONTAL}, { 0, 12, 10, 0 }, 7 });
	insert("WHALE", { {{10, 5}, WordOrientation::VERTICAL}, { 0, 12, 10, 0 }, 8 });
	insert("WOLF", { {{10, 5}, WordOrientation::HORIZONTAL}, { 0, 14, 10, 0 }, 9 });
	insert("EAGLE", { {{12, 2}, WordOrientation::VERTICAL}, { 0, 14, 10, 0 }, 10 });
	insert("MONKEY", { {{8, 2}, WordOrientation::HORIZONTAL}, { 0, 14, 10, 0 }, 11 });
	insert("SHARK", { {{11, -2}, WordOrientation::VERTICAL}, { -2, 14, 10, 0 }, 12 });
	insert("LIZARD", { {{8, 0}, WordOrientation::HORIZONTAL}, { -2, 14, 10, 0 }, 13 });
	insert("OWL", { {{8, -2}, WordOrientation::VERTICAL}, { -2, 14, 10, 0 }, 14 });
	insert("HEDGEHOG", { {{2, -2}, WordOrientation::HORIZONTAL}, { -2, 14, 10, 0 }, 15 });
	insert("RAT", { {{7, 4}, WordOrientation::VERTICAL}, { -2, 14, 10, 0 }, 17 });
	insert("DEER", { {{4, -2}, WordOrientation::VERTICAL}, { -2, 14, 10, 0 }, 19 });
}

namespace
{
	auto insert(const crosswordString& word, const WordParams& params, Crossword& cross)
	{
		cross.insertWord(word, params + cross.getCoordinateStart());
	};
}

TEST(CrosswordTest, constraints)
{
	Crossword cross{ "AB" };
	auto insert = [](const crosswordString& word, const WordParams& params, Crossword& cross)
	{
		cross.insertWord(word, params + cross.getCoordinateStart());
	};
	insert("AC", {{0, 0}, WordOrientation::VERTICAL }, cross);
	insert("CD", {{0, 1}, WordOrientation::HORIZONTAL }, cross);
	insert("BD", {{1, 0}, WordOrientation::VERTICAL }, cross);

	for (const auto& word : std::vector{ "BE", "DE", "CE", "EC", "EA", "EB" }) ASSERT_TRUE(cross.testWord(word).empty());

	insert("BB", {{ 0, -2 }, WordOrientation::HORIZONTAL }, cross);
	insert("BF", {{ -2, 0 }, WordOrientation::VERTICAL }, cross);
	insert("EE", {{ 0, 3 }, WordOrientation::HORIZONTAL }, cross);
	insert("EF", {{ 3, 0 }, WordOrientation::VERTICAL }, cross);
	ASSERT_TRUE(cross.testWord("BE").empty());

	Crossword cross1{ "AC" };
	insert("BB", {{ 0, -2 }, WordOrientation::HORIZONTAL }, cross1);
	ASSERT_TRUE(cross1.testWord("BE").empty());
	insert("AE", {{0, 0}, WordOrientation::VERTICAL }, cross1);
	insert("DD", {{ -2, 0 }, WordOrientation::VERTICAL }, cross1);
	auto inses = cross1.testWord("DE");
	ASSERT_TRUE(cross1.testWord("DE").empty());

	Crossword cross2{ "AC" };
	insert("CB", {{ 1, 0 },  WordOrientation::VERTICAL }, cross2);
 	insert("DD", {{ 3, 0 }, WordOrientation::VERTICAL }, cross2);
	ASSERT_TRUE(cross2.testWord("BE").empty());

	Crossword cross3{ "AC" };
	insert("CFB", {{ 1, 0 },  WordOrientation::VERTICAL }, cross3);
	insert("BB", {{ 0, 2 }, WordOrientation::HORIZONTAL }, cross3);
	insert("DD", {{ 0, 4 }, WordOrientation::HORIZONTAL }, cross3);
	ASSERT_TRUE(cross3.testWord("BD").empty());
}

TEST(CrosswordTest, constraintsWithSameOrientation)
{
	Crossword cross("AB");
	insert("BCD", {{1,0}, WordOrientation::VERTICAL }, cross);
	insert("DE", {{1,2}, WordOrientation::HORIZONTAL }, cross);
	for (const auto& word : std::vector{ "FC", "CF", "AF", "FE"}) ASSERT_TRUE(cross.testWord(word).empty());

	Crossword cross1("AB", false);
	insert("BCD", {{1,0}, WordOrientation::VERTICAL }, cross1);
	insert("DE", {{1,2}, WordOrientation::HORIZONTAL }, cross1);
	for (const auto& word : std::vector{ "FC", "CF", "AF", "FE" }) ASSERT_FALSE(cross1.testWord(word).empty());
}

TEST(CrosswordTest, eraseAbsentWord)
{
	Crossword cross("SNAKE");
	testException([&cross]() {cross.eraseWord("RAT"); }, "The crossword doesn't consist this word.");
	insert("RAT", {{0,0}, WordOrientation::VERTICAL }, cross);
	testException([&cross]() {cross.eraseWord("SNAKE"); }, "The crossword doesn't consist this word in this position.");
}

TEST(CrosswordTest, eraseWords)
{
	Crossword cross("EXEMPT");
	auto testErase = [&cross](const crosswordString& word, const WordParams& params)
	{
		insert(word, params, cross);
		EXPECT_NO_THROW(cross.eraseWord(word));
		insert(word, params, cross);
	};

	EXPECT_NO_THROW(cross.eraseWord("EXEMPT"));
	insert("EXEMPT", {{0, 0}, WordOrientation::HORIZONTAL }, cross);
	auto vec = std::vector<std::pair<crosswordString, WordParams>>{ {"ENIGMA", {{0,0}, WordOrientation::VERTICAL}},{"IMPROVEMENT", {{0,2}, WordOrientation::HORIZONTAL}},
		{"MERCHANT", {{0,4}, WordOrientation::HORIZONTAL}}, {"PERMANENT", {{2,2}, WordOrientation::VERTICAL}}, {"PROPHESY", {{4,0}, WordOrientation::VERTICAL}},
		{"BEING", {{6,1}, WordOrientation::VERTICAL}},  {"SEEK", {{8,0}, WordOrientation::VERTICAL}},  {"TOLERANCE", {{10,2}, WordOrientation::VERTICAL}},
		{"RAISE", {{1,6}, WordOrientation::HORIZONTAL}} , {"NEED", {{0,8}, WordOrientation::HORIZONTAL}},  {"ASSIST", {{7,0}, WordOrientation::HORIZONTAL}},
		{"CLUE", {{9,4}, WordOrientation::HORIZONTAL}}, {"ENTRY", {{7,6}, WordOrientation::HORIZONTAL}}, {"PRETENSE", {{5,8}, WordOrientation::HORIZONTAL}},
		{ "TRANSPARENT", {{2,10}, WordOrientation::HORIZONTAL}}, {"TALKER", {{12,0}, WordOrientation::VERTICAL}}, {"INFORM", {{0, 7}, WordOrientation::VERTICAL}},
		{"INITIATE", {{8, 5}, WordOrientation::VERTICAL}}, {"FAIR", {{4, 9}, WordOrientation::VERTICAL}}, {"ERASE", {{6, 7}, WordOrientation::VERTICAL}},
		{"LETTER", {{12, 7}, WordOrientation::VERTICAL}}, {"MATURE", {{0,12}, WordOrientation::HORIZONTAL}}, {"BEGGAR", {{7,12}, WordOrientation::HORIZONTAL}} };

	for (const auto& [word, params] : vec) testErase(word, params);
	static constexpr auto Nintersections = 37;
	ASSERT_EQ(cross.testWord("RAT")[0].crosswordIntersectionNumber, Nintersections);
	for (const auto& [word, params] : vec) EXPECT_NO_THROW(cross.eraseWord(word));
}

TEST(CrosswordTest, TestWordExceptioins)
{
	testException([]() {Crossword{ "WORD" }.testWord("W"); }, "A word's size should be greater or equal two.");
	testException([]() {Crossword{ "WORD" }.testWord(""); }, "A word's size should be greater or equal two.");
}

TEST(CrosswordTest, InsertWordExceptioins)
{
	testException([]() {Crossword{ "WORD" }.insertWord("w", { {0,0}, WordOrientation::HORIZONTAL }); }, "A word's size should be greater or equal two.");
	testException([]() {Crossword{ "WORD" }.insertWord("", { {0, 0}, WordOrientation::HORIZONTAL }); }, "A word's size should be greater or equal two.");
	testException([](){Crossword{ "SNAKE" }.insertWord("SNAKE", { {0,2}, WordOrientation::HORIZONTAL }); }, "The crossword consists this word.");
	testException([](){Crossword{ "WORD" }.insertWord("OWL", { { MAX_CROSSWORD_WIDTH - 2, 0}, WordOrientation::HORIZONTAL }); }, "Out of the board.");
	ASSERT_NO_THROW(Crossword{ "WORD" }.insertWord("OWL", { { MAX_CROSSWORD_WIDTH - 3, 0}, WordOrientation::HORIZONTAL }));
	testException([]() {Crossword{ "WORD" }.insertWord("OWL", { { -1, 0}, WordOrientation::HORIZONTAL }); }, "Out of the board.");
	ASSERT_NO_THROW(Crossword{ "WORD" }.insertWord("OWL", { { 0, 0}, WordOrientation::HORIZONTAL }));
	testException([]() {Crossword{ "WORD" }.insertWord("OWL", { {0, MAX_CROSSWORD_HEIGHT - 2}, WordOrientation::VERTICAL }); }, "Out of the board.");
	ASSERT_NO_THROW(Crossword{ "WORD" }.insertWord("OWL", { {0, MAX_CROSSWORD_HEIGHT - 3}, WordOrientation::VERTICAL }));
	testException([]() {Crossword{ "WORD" }.insertWord("OWL", { {0, -1}, WordOrientation::VERTICAL }); }, "Out of the board.");
	ASSERT_NO_THROW(Crossword{ "WORD" }.insertWord("OWL", { {0, 0}, WordOrientation::VERTICAL }));
}

TEST(CrosswordTest, ConstructorExceptioins)
{
	testException([]() {Crossword{ "W" }; }, "A word's size should be greater or equal two.");
	testException([]() {Crossword{ "" }; }, "A word's size should be greater or equal two.");
}
