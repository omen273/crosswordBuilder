#pragma once
#include <array>
#include <compare>
#include <queue>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>


namespace Utils
{
	struct Intersection final
	{
		Intersection(std::size_t x, std::size_t y) noexcept : firstWordPos(x), secondWordPos(y) {}
		std::size_t firstWordPos;
		std::size_t secondWordPos;

		[[nodiscard]] auto operator<=> (const Intersection&) const = default;
	};

	//works only for Latin big letters
	[[nodiscard]] auto inline findIntersections(const std::string& str1, const std::string& str2)
	{
		std::vector<Intersection> res;
		std::array<std::vector<std::size_t>, 26> lettersStr;
		for (std::size_t i = 0; i < str1.size(); ++i) lettersStr[str1[i] - 'A'].push_back(i);
		for (std::size_t i = 0; i < str2.size(); ++i)
		{
			for (auto pos1 : lettersStr[str2[i] - 'A']) res.emplace_back(pos1, i);
		}

		return res;
	}

	struct wordsHashOrdered final
	{
		template<typename T>
		[[nodiscard]] auto operator()(const std::pair<T, T>& p) const noexcept
		{
			std::size_t h1 = std::hash<T>{}(p.first);
			std::size_t h2 = std::hash<T>{}(p.second);
			return h1 ^ (h2 << 1);
		}
	};

	[[nodiscard]] std::optional<std::vector<std::string>> inline findGroupWithSizeN(const std::vector<std::string>& words, std::size_t n)
	{
		auto visit = [](const auto& words, std::size_t n)
		{
			std::queue<std::size_t> q;
			q.push(0);
			std::vector<std::size_t> visited(words.size());
			std::size_t count = 1;
			visited.front() = true;
			while (!q.empty() && count < n)
			{
				auto front = q.front();
				q.pop();
				for (std::size_t i = 0; i < words.size() && count < n ; ++i)
				{
					if (i != front && !visited[i] && !findIntersections(words[front], words[i]).empty())
					{
						q.push(i);
						visited[i] = true;
						++count;
					}
				}
			}
			return std::pair(count == n, visited);
		};

		std::vector<std::string> nonIntersected{words};
		do{
			auto visited = visit(nonIntersected, n);
			if (visited.first)
			{
				std::vector<std::string> res;
				res.reserve(n);
				for (std::size_t i = 0; i < nonIntersected.size(); ++i)
				{
					if(visited.second[i]) res.push_back(nonIntersected[i]);
				}
				return res;
			}
			std::vector<std::string> newNonIntersected;
			for(std::size_t i = 0; i < nonIntersected.size(); ++i)
			{
				if (!visited.second[i]) newNonIntersected.push_back(nonIntersected[i]);
			}
			nonIntersected = std::move(newNonIntersected);
		}while(nonIntersected.size() >= n);

		return std::nullopt;
	}
	
	[[nodiscard]] auto inline isPossibileToIntersect(const std::vector<std::string>& words)
	{
		return findGroupWithSizeN(words, words.size());
	}

	struct Position final
	{
		int x;
		int y;

		auto& operator+=(const Position& pos) noexcept
		{
			x += pos.x, y += pos.y;
			return *this;
		}

		auto& operator-=(const Position& pos) noexcept
		{
			x -= pos.x, y -= pos.y;
			return *this;
		}

		[[nodiscard]] friend auto operator+(Position first, const Position& second) noexcept
		{
			return first += second;
		}

		[[nodiscard]] friend auto operator-(Position first, const Position& second) noexcept
		{
			return first -= second;
		}

		[[nodiscard]] friend auto operator==(const Position& first, const Position& second) noexcept
		{
			return std::tie(first.x, first.y) == std::tie(second.x, second.y);
		}

		[[nodiscard]] friend auto operator<(const Position& first, const Position& second) noexcept
		{
			return std::tie(first.x, first.y) < std::tie(second.x, second.y);
		}
	};

	struct positionHash final
	{
		[[nodiscard]] auto operator()(const Position& p) const noexcept
		{
			auto h1 = std::hash<int>{}(p.x);
			auto h2 = std::hash<int>{}(p.y);
			return h1 ^ (h2 << 1);
		}
	};

	struct Limits final
	{
		int top;
		int right;
		int bottom;
		int left;

		[[nodiscard]] friend auto operator==(const Limits& first, const Limits& second) noexcept
		{
			return std::tie(first.top, first.right, first.bottom, first.left) ==
				std::tie(second.top, second.right, second.bottom, second.left);
		}

		auto& operator+=(const Position& pos) noexcept
		{
			top += pos.y, right += pos.x, bottom += pos.y, left += pos.x;
			return *this;
		}

		[[nodiscard]] friend auto operator+(Limits l, const Position& pos) noexcept
		{
			return l += pos;
		}
	};

	enum class WordOrientation : bool
	{
		VERTICAL,
		HORIZONTAL
	};

	struct WordParams final
	{
		Utils::Position start;
		WordOrientation orientation;

		[[nodiscard]] friend auto operator==(const WordParams& first, const WordParams& second) noexcept
		{
			return std::tie(first.orientation, first.start) == std::tie(second.orientation, second.start);
		}

		auto& operator+=(const Position& pos) noexcept
		{
			start += pos;
			return *this;
		}

		auto& operator-=(const Position& pos) noexcept
		{
			start -= pos;
			return *this;
		}

		[[nodiscard]] friend auto operator+(WordParams lhs, const Position& rhs) noexcept
		{
			return lhs += rhs;
		}

		[[nodiscard]] friend auto operator-(WordParams lhs, const Position& rhs) noexcept
		{
			return lhs -= rhs;
		}


	};

	struct insertionParams final
	{
		insertionParams(const WordParams& params, const Limits& limits, std::size_t n) noexcept :
			wordParams{ params }, limits{ limits }, crosswordIntersectionNumber{ n }{}
		WordParams wordParams;
		Limits limits;
		std::size_t crosswordIntersectionNumber;

		[[nodiscard]] friend auto operator==(const insertionParams& first, const insertionParams& second) noexcept
		{
			return std::tie(first.wordParams, first.limits, first.crosswordIntersectionNumber) ==
				std::tie(second.wordParams, second.limits, second.crosswordIntersectionNumber);
		}

		[[nodiscard]] friend auto operator+(insertionParams lhs, const Position& rhs) noexcept
		{
			lhs.wordParams += rhs, lhs.limits += rhs;
			return lhs;
		}
	};

	template<typename T>
	void toUpper(T begin, T end)
	{
		for (auto it = begin; it != end; ++it) for (auto& ch : *it) ch = std::toupper(ch);
	}

	auto inline toUpper(std::vector<std::string> inp)
	{
		for (auto& word : inp) for (auto& ch : word) ch = std::toupper(ch);
		return inp;
	}
}
