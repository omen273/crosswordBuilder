#include "WordRandomizer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>

#include "CrosswordBuilder.h"
#include "Utils.h"

int main()
{
	for (const auto& str : CrosswordBuilder::build(Utils::toUpper(std::vector<std::string>{"exempt", "assist", "improvement", "merchant", "clue", "raise", "entry", "need", "pretense", "transparent",
		"mature", "beggar", "enigma", "permanent", "prophesy", "being", "seek", "tolerance" }), 15).value().getCrossword()) std::cout << str <<'\n';
}