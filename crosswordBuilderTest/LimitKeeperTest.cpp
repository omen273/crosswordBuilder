#include "pch.h"

#include "../crosswordBuilder/LimitKeeper.h"

#include "../crosswordBuilder/Utils.h"
#include "TestUtils.h"

using namespace Utils;
using namespace testUtils;

TEST(LimitKeeperTest, keep)
{
	LimitKeeper keeper;
	auto testAdd = [&keeper](const WordParams& params, std::size_t size, const Limits& limits)
	{
		keeper.addWord(params, size);
		auto is = keeper.getLimits() == limits;
		ASSERT_TRUE(is);
	};

	testAdd({{0, 0}, WordOrientation::HORIZONTAL }, 4, { 0, 4, 1, 0 });
	testAdd({{2, -5}, WordOrientation::VERTICAL }, 7, { -5, 4, 2, 0 });
	testAdd({{0, -5}, WordOrientation::HORIZONTAL, }, 4, { -5, 4, 2, 0 });
	testAdd({{0, -5}, WordOrientation::VERTICAL }, 7, { -5, 4, 2, 0 });
	testAdd({{-3, -4}, WordOrientation::HORIZONTAL }, 4, { -5, 4, 2, -3 });
	testAdd({{-3, -2}, WordOrientation::HORIZONTAL }, 7, { -5, 4, 2, -3 });
	testAdd({{-3, 1}, WordOrientation::HORIZONTAL }, 4, { -5, 4, 2, -3 });
	testAdd({{-2, -5}, WordOrientation::VERTICAL }, 7, { -5, 4, 2, -3 });

	auto testRemove = [&keeper](const WordParams& params, std::size_t size, const Limits& limits)
	{
		keeper.removeWord(params, size);
		auto is = keeper.getLimits() == limits;
		ASSERT_TRUE(is);
	};

	testRemove({{-2, -5}, WordOrientation::VERTICAL }, 7, { -5, 4, 2, -3 });
	testRemove({{-3, 1}, WordOrientation::HORIZONTAL }, 4, { -5, 4, 2, -3 });
	testRemove({{-3, -2}, WordOrientation::HORIZONTAL }, 7, { -5, 4, 2, -3 });
	testRemove({{-3, -4}, WordOrientation::HORIZONTAL }, 4, { -5, 4, 2, 0 });
	testRemove({{0, -5}, WordOrientation::VERTICAL }, 7, { -5, 4, 2, 0 });
	testRemove({{0, -5}, WordOrientation::HORIZONTAL }, 4, { -5, 4, 2, 0 });
	testRemove({{2, -5}, WordOrientation::VERTICAL }, 7, { 0, 4, 1, 0 });
	testRemove({{0, 0}, WordOrientation::HORIZONTAL }, 4, { 0, 0, 0, 0 });
}

TEST(LimitKeeperTest, removeAbsentWord)
{
	LimitKeeper keeper;
	testException([&keeper]() {keeper.removeWord({ {0,0}, WordOrientation::HORIZONTAL }, 2); }, "This top limit doesn't exist");
	keeper.addWord({ {0,0}, WordOrientation::HORIZONTAL }, 2);
	testException([&keeper]() {keeper.removeWord({ {0,0}, WordOrientation::HORIZONTAL }, 3); }, "This right limit doesn't exist");
	testException([&keeper]() {keeper.removeWord({ {1, 0}, WordOrientation::VERTICAL }, 2); }, "This bottom limit doesn't exist");
	testException([&keeper]() {keeper.removeWord({ {-1, 0}, WordOrientation::HORIZONTAL }, 3); }, "This left limit doesn't exist");
}
