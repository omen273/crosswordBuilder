#pragma once

#include <map>

#include "Utils.h"

class LimitKeeper final
{
public:
	[[nodiscard]]  Utils::Limits getLimits() const noexcept;
	[[nodiscard]]  Utils::Limits tryLimits(const Utils::WordParams& params, std::size_t size) const noexcept;
	void addWord(const Utils::WordParams& params, std::size_t size) noexcept;
	void removeWord(const Utils::WordParams& params, std::size_t size);

private:
	std::map<int, std::size_t> tops;
	std::map<int, std::size_t> rights;
	std::map<int, std::size_t> bottoms;
	std::map<int, std::size_t> lefts;
};
