#pragma once

#include <string>
#include <vector>


class WordRandomizer final
{
public:
	static std::vector<std::string> getRandomWords(const std::vector<std::string>& words, size_t n);
	//unlike std::shuffle uses all words from input to shuffle n first words;
	static void shuffleNFirstWords(std::vector<std::string>& words, size_t n);
};
