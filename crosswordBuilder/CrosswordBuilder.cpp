#include "CrosswordBuilder.h"

#include <algorithm>
#include <random>
#include <stack>

#include "Utils.h"

using namespace Utils;

namespace
{
	struct BuilderParams
	{
		std::vector<insertionParams> insertions;
		std::size_t pos;
		std::size_t wordNumber;
	};
};

std::optional<Crossword> CrosswordBuilder::build(std::vector<crosswordString> words, std::size_t limit)
{
	if (words.size() < 2) [[unlikely]] throw std::runtime_error{ "Input consists less than two words." };
	if (!isPossibileToIntersect(words)) throw std::runtime_error{ "Impossible to intersect these words." };
	if (auto it = std::find_if(words.begin(), words.end(), [&limit](const auto& word) {return word.size() > limit; }); it != words.end())
	{
		std::string s = "The input consists too long word: ";
		s += *it;
		throw std::runtime_error{ s };
	}
	std::stack<BuilderParams> buffer;
	std::vector<bool> usedWords(words.size());
	usedWords[0] = true;
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(words.begin(), words.end(), g);
	Crossword cross{ words[0] };
	size_t restartFromThisWordCount = 0;
	auto maxHeight = static_cast<int>(limit), maxWidth = static_cast<int>(limit);

	auto getNthBestPosition = [&cross, &words](auto& buildParams)
	{
		const auto newLetterAmount = cross.getLetterCount() + words[buildParams.wordNumber].size();
		const auto it = buildParams.insertions.begin() + buildParams.pos;
		std::nth_element(buildParams.insertions.begin(), it, buildParams.insertions.end(),
			[newLetterAmount, &cross](const auto& x, const auto& y)
			{
				auto computeMetric = [newLetterAmount, &cross](const auto& params)
				{
					const auto& limits = params.limits;
					const auto height = limits.bottom - limits.top;
					const auto width = limits.right - limits.left;
					const auto area = height * width;
					const auto wordCount = cross.getWordCount();
					return static_cast<float>(newLetterAmount) / area
						+ static_cast<float>(params.crosswordIntersectionNumber - wordCount) / wordCount -
						static_cast<float>(abs(height - width)) / std::max(height, width);
				};
				const auto xMetric = computeMetric(x);
				const auto yMetric = computeMetric(y);
				//TODO think about epsilon
				return abs(xMetric - yMetric) < std::numeric_limits<float>::epsilon() ? x.wordParams.start < y.wordParams.start :
					xMetric > yMetric;
			});
		return it->wordParams;
	};

	while (buffer.size() < words.size() - 1)
	{
		std::vector<insertionParams> intersections;
		std::size_t j = 1;
		bool isFound = false;
		for (; j < usedWords.size() && intersections.empty(); ++j)
		{
			if (!usedWords[j])
			{
				auto res = cross.testWord(words[j]);
				res.erase(std::remove_if(res.begin(), res.end(), 
					[&maxHeight, &maxWidth](const auto& inter)
					{
						return abs(inter.limits.bottom - inter.limits.top) > maxHeight || abs(inter.limits.right - inter.limits.left) > maxWidth;
					}), res.end());
				intersections = res;
				if (!intersections.empty())
				{
					usedWords[j] = true;
					isFound = true;
					buffer.push({ intersections, 0, j });
				}
			}
		}

		if (!isFound)
		{
			while (!buffer.empty())
			{
				if (buffer.top().pos++ < buffer.top().insertions.size() - 1)
				{
					cross.eraseWord(words[buffer.top().wordNumber]);
					cross.insertWord(words[buffer.top().wordNumber], getNthBestPosition(buffer.top()));
					break;
				}
				cross.eraseWord(words[buffer.top().wordNumber]);
				usedWords[buffer.top().wordNumber] = false;
				buffer.pop();
			}

			static constexpr auto MAX_RESTART_COUNT = 30;
			if (++restartFromThisWordCount > MAX_RESTART_COUNT || buffer.empty())
			{
				while (!buffer.empty())
				{
					cross.eraseWord(words[buffer.top().wordNumber]);
					usedWords[buffer.top().wordNumber] = false;
					buffer.pop();
				}
				cross.eraseWord(words[0]);
				std::shuffle(words.begin(), words.end(), g);
				cross.insertWord(words[0], WordParams{ {0, 0}, WordOrientation::HORIZONTAL } + cross.getCoordinateStart());
				restartFromThisWordCount = 0;
			}
		}
		else
		{
			cross.insertWord(words[j - 1], getNthBestPosition(buffer.top()));
		}
	}

	return cross;
}
