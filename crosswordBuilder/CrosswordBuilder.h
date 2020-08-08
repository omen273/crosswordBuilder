#pragma once

#include <optional>

#include "Crossword.h"

class CrosswordBuilder final
{
	//TODO think about moving of words or shared words
public:
	[[nodiscard]]  static std::optional<Crossword> build(const std::vector<crosswordString> words, std::size_t limit = 30);
};
