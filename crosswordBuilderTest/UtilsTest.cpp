#include "pch.h"

#include "../crosswordBuilder/Utils.h"

#include <algorithm>

#include "testUtils.h"

using namespace Utils;
using namespace testUtils;

namespace
{
    [[nodiscard]] auto isPossibileToIntersect(std::vector<std::string> words)
    {
        return findGroupWithSizeN(words.begin(), words.end(), words.size());
    }
}

TEST(UtilsTest, findIntersections)
{
    auto test = [](const auto& str1, const auto& str2, const std::vector<Intersection>& intersections)
    {
        auto res = Utils::findIntersections(str1, str2);
        std::sort(res.begin(), res.end());
        ASSERT_EQ(res, intersections);
    };
    test("DOG", "DIG", { {0, 0}, { 2, 2 } });
    test("DOG", "", {});
    test("", "CAT", {});
    test("", "", {});
    test("DOG", "POOR", { { 1, 1 }, { 1, 2 } });
    test("CORE", "POOR", { {1, 1}, {1, 2}, {2, 3} });
    test("COOL", "POOR", { {1, 1}, {1, 2}, {2, 1}, {2, 2} });
    auto str1 = "RAT", str2 = "RIVAL";
    auto res1 = Utils::findIntersections(str1, str2), res2 = Utils::findIntersections(str2, str1);
    std::sort(res1.begin(), res1.end()), std::sort(res2.begin(), res2.end());
    for (auto& in : res2) std::swap(in.firstWordPos, in.secondWordPos);
    ASSERT_EQ(res1, res2);
}

TEST(UtilsTest, findGroupWithSizeN)
{
    ASSERT_FALSE(isPossibileToIntersect(toUpper({ "veldt", "jynx", "grimps", "wrqf", "zho", "aaa" })));
    ASSERT_FALSE(isPossibileToIntersect(toUpper({ "yak", "owl" })));
    ASSERT_FALSE(isPossibileToIntersect(toUpper({ "hedgehog", "deer", "snake", "squirrel", "rhinoceros", "tiger", "hare",
        "elephant", "crocodile", "whale", "eagle", "xxx" })));
    ASSERT_FALSE(isPossibileToIntersect(toUpper({ "albatross", "biddy", "blackbird", "canary", "crow", "cuckoo", "dove", "pigeon", "duck", "eagle", "falcon", "finch",
        "flamingo", "goose", "gull", "hawk", "jackdaw", "jay", "kestrel", "kookaburra", "mallard", "nightingale", "nuthatch", "ostrich", "owl", "parakeet",
        "parrot", "peacock", "pelican", "penguin", "pheasant", "piranha", "raven", "robin", "rooster", "sparrow", "stork", "swallow", "swan",
        "swift", "tit", "turkey", "vulture", "woodpecker", "wren", "xxx" })));
    ASSERT_FALSE(isPossibileToIntersect(toUpper({ "aaa", "aaaab", "bbbb", "bbbc", "ddddd", "dddee", "eefff", "fff" })));
    ASSERT_TRUE(isPossibileToIntersect(toUpper({ "albatross", "biddy", "blackbird", "canary", "crow" })));
}

TEST(Utils, findGroupWithSizeN)
{
    std::vector<std::string> v{ "PAINTER", "FULL", "DEMAND", "TOWARD", "CARE", "SORE", "PLACE", "LAST", "DESPITE",
            "FOOT", "MUSEUM", "ACROSS", "DOCUMENT", "WITHIN", "ANSWER", "WHETHER", "SUCK", "VIEW", "SAD", "DEFINITION",
            "VALUE", "CRUSH", "FIGURE", "HOLD", "NEARBY", "GUARD", "MIGHT", "LUCKY", "HUGE", "OFFENSE" };
    auto res = Utils::findGroupWithSizeN(v.begin(), v.end(), 5);
    ASSERT_TRUE(res);
}
