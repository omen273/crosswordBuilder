#include "Crossword.h"

#include <stdexcept>
#include <unordered_set>

#include "Constants.h"

//TODO exception safe

using namespace Utils;

Crossword::Crossword(const crosswordString& firstWord, bool removeTouchesWithSameOrientation) :
    removeTouchesWithSameOrientation{ removeTouchesWithSameOrientation }
{
    if (firstWord.size() < 2) [[unlikely]] throw std::runtime_error{ "A word's size should be greater or equal two." };
    const auto start = getCoordinateStart();
    board.resize(MAX_CROSSWORD_HEIGHT, std::vector<CrosswordCell>(MAX_CROSSWORD_WIDTH));
    const auto wordParams = WordParams{ start,  WordOrientation::HORIZONTAL };
    if (outsideBorders(wordParams, firstWord.size())) throw std::runtime_error{ "Out of the board." };
    words.emplace(firstWord, wordParams);

    board[start.y][start.x].addHorizontalBeginLetter(firstWord[0]);
    for (std::size_t i = 1; i < firstWord.size() - 1; ++i)
    {
        board[start.y][start.x + i].addHorizontalInsideLetter(firstWord[i]);
    }
    board[start.y][start.x + firstWord.size() - 1].addHorizontalEndLetter(firstWord.back());

    limitKeeper.addWord(wordParams, firstWord.size());
    letterN += firstWord.size();
}

std::optional<std::size_t> Crossword::canBeInserted(const WordParams& params, const crosswordString& word) const noexcept
{
    auto fromWordToCellOrientation = [](WordOrientation o)
    {
        return o == WordOrientation::HORIZONTAL ? CellOrientation::HORIZONTAL : CellOrientation::VERTICAL;
    };

    std::size_t intersectionNumber = 0;
    if (params.orientation == WordOrientation::HORIZONTAL)
    {
        if (outsideBorders(params, word.size())) [[unlikely]] return std::nullopt;

        const auto& startPos = params.start;
        const auto& endPos = Position{ params.start.x + static_cast<int>(word.size()) - 1, params.start.y };
        const auto isLeftContact = startPos.x > 0 ?
            board[startPos.y][static_cast<std::size_t>(startPos.x) - 1].orientation() != CellOrientation::NONE : false;
        const auto isRightContact = endPos.x + 1 < static_cast<int>(board[0].size()) ?
            board[endPos.y][static_cast<std::size_t>(endPos.x) + 1].orientation() != CellOrientation::NONE : false;
        if (isLeftContact || isRightContact) return std::nullopt;

        for (auto i = 0; i < word.size(); ++i)
        {
            auto areTouches = [&, this]()
            {

                auto x = params.start.x + i, y = params.start.y;
                const auto isDownContact = y + 1 < static_cast<int>(board.size()) ?
                    (board[y + 1][x].isVerticalBegin() && (board[y + 1][x].orientation() == CellOrientation::VERTICAL
                        || board[y + 1][x].orientation() == CellOrientation::BOTH)) ||
                    (removeTouchesWithSameOrientation && board[y + 1][x].orientation() == CellOrientation::HORIZONTAL) : false;
                const auto isUpContact = y > 0 ?
                    (board[y - 1][x].isVerticalEnd() && (board[y - 1][x].orientation() == CellOrientation::VERTICAL
                        || board[y - 1][x].orientation() == CellOrientation::BOTH)) ||
                    (removeTouchesWithSameOrientation && board[y - 1][x].orientation() == CellOrientation::HORIZONTAL) : false;
                return isDownContact || isUpContact;

            };
            const auto& cell = board[params.start.y][params.start.x + i];
            if (cell.orientation() == CellOrientation::BOTH ||
                (cell.orientation() != CellOrientation::NONE && cell.letter() != word[i]) ||
                cell.orientation() == fromWordToCellOrientation(params.orientation) || areTouches())
            {
                return std::nullopt;
            }
            if (cell.orientation() != CellOrientation::NONE) ++intersectionNumber;
        }
    }
    else
    {
        if (outsideBorders(params, word.size())) [[unlikely]] return std::nullopt;

        const auto& startPos = params.start;
        const auto& endPos = Position{ params.start.x, params.start.y + static_cast<int>(word.size()) - 1 };
        const auto isUpContact = startPos.y > 0 ?
            board[static_cast<std::size_t>(startPos.y) - 1][startPos.x].orientation() != CellOrientation::NONE : false;
        const auto isDownContact = endPos.y + 1 < static_cast<int>(board.size())
            ? board[static_cast<std::size_t>(endPos.y) + 1][endPos.x].orientation()
            != CellOrientation::NONE : false;
        if (isUpContact || isDownContact) return std::nullopt;

        for (auto i = 0; i < word.size(); ++i)
        {
            const auto& cell = board[params.start.y + i][params.start.x];
            auto areTouches = [&, this]()
            {
                auto x = params.start.x, y = params.start.y + i;
                const auto isRightContact = x + 1 < static_cast<int>(board[y].size()) ?
                    (board[y][x + 1].isHorizontalBegin() && (board[y][x + 1].orientation() == CellOrientation::HORIZONTAL ||
                        board[y][x + 1].orientation() == CellOrientation::BOTH)) ||
                    (removeTouchesWithSameOrientation && board[y][x + 1].orientation() == CellOrientation::VERTICAL) : false;
                const auto isLeftContact = x > 0 ?
                    (board[y][x - 1].isHorizontalEnd() && (board[y][x - 1].orientation() == CellOrientation::HORIZONTAL ||
                        board[y][x - 1].orientation() == CellOrientation::BOTH)) ||
                    (removeTouchesWithSameOrientation && board[y][x - 1].orientation() == CellOrientation::VERTICAL) : false;
                return isRightContact || isLeftContact;
            };

            if (cell.orientation() == CellOrientation::BOTH ||
                (cell.orientation() != CellOrientation::NONE && cell.letter() != word[i]) ||
                cell.orientation() == fromWordToCellOrientation(params.orientation) || areTouches())
            {
                return std::nullopt;
            }

            if (cell.orientation() != CellOrientation::NONE) ++intersectionNumber;
        }
    }

    return intersectionNumber;
}

std::vector<Utils::insertionParams> Crossword::testWord(const crosswordString& word) const
{
    if (word.size() < 2) [[unlikely]] throw std::runtime_error{ "A word's size should be greater or equal two." };

    std::vector< insertionParams> res;
    std::unordered_set<Position, positionHash> positions;
    for (const auto& [intersectedWord, param] : words)
    {
        std::vector<Intersection> intersections;
        const auto it = intersectionCache.find(std::pair{ intersectedWord, word });
        if (it != intersectionCache.end()) intersections = it->second;
        else
        {
            intersections = findIntersections(intersectedWord, word);
            intersectionCache.emplace(std::pair{ intersectedWord, word }, intersections);
        }

        for (const auto intersection : intersections)
        {
            const auto wordParams = param.orientation == WordOrientation::HORIZONTAL ?
                WordParams{ param.start +
                            Position{ static_cast<int>(intersection.firstWordPos), -static_cast<int>(intersection.secondWordPos) },
                            WordOrientation::VERTICAL } :
                WordParams{ param.start +
                            Position{ -static_cast<int>(intersection.secondWordPos), static_cast<int>(intersection.firstWordPos) },
                            WordOrientation::HORIZONTAL };
            if (!positions.contains(wordParams.start))
            {
                positions.insert(wordParams.start);
                if (const auto interN = canBeInserted(wordParams, word))
                {
                    auto newLimits = limitKeeper.tryLimits(wordParams, word.size());
                    res.emplace_back(wordParams, newLimits, interN.value() + intersectionN);
                }
            }
        }
    }

    return res;
}

bool Crossword::outsideBorders(const WordParams& wordParams, std::size_t size) const
{
    const auto newLimits = limitKeeper.tryLimits(wordParams, size);
    return newLimits.top < 0 || newLimits.bottom > MAX_CROSSWORD_HEIGHT ||
        newLimits.left < 0 || newLimits.right > MAX_CROSSWORD_WIDTH;
}

void Crossword::insertWord(const crosswordString& word, const WordParams& wordParams)
{
    if (word.size() < 2) [[unlikely]] throw std::runtime_error{ "A word's size should be greater or equal two." };
    if (words.count(word)) [[unlikely]] throw std::runtime_error{ "The crossword consists of this word." };
    if (outsideBorders(wordParams, word.size())) throw std::runtime_error{ "Out of the board." };

    if (wordParams.orientation == WordOrientation::HORIZONTAL)
    {
        for (std::size_t i = 0; i < word.size(); ++i)
        {
            auto& cell = board[wordParams.start.y][static_cast<std::size_t>(wordParams.start.x) + i];
            if (cell.orientation() == CellOrientation::VERTICAL) ++intersectionN;
            cell.addHorizontalInsideLetter(word[i]);
        }
        board[wordParams.start.y][wordParams.start.x].addHorizontalBeginLetter(word[0]);
        board[wordParams.start.y][wordParams.start.x + word.size() - 1].addHorizontalEndLetter(word.back());
    }
    else
    {
        for (std::size_t i = 0; i < word.size(); ++i)
        {
            auto& cell = board[static_cast<std::size_t>(wordParams.start.y) + i][wordParams.start.x];
            if (cell.orientation() == CellOrientation::HORIZONTAL) ++intersectionN;
            cell.addVerticalInsideLetter(word[i]);
        }
        board[wordParams.start.y][wordParams.start.x].addVerticalBeginLetter(word[0]);
        board[wordParams.start.y + word.size() - 1][wordParams.start.x].addVerticalEndLetter(word.back());
    }

    words.emplace(word, wordParams);

    limitKeeper.addWord(wordParams, word.size());
    letterN += word.size();
}

void Crossword::eraseWord(const crosswordString& word)
{
    if (word.size() < 2) [[unlikely]] throw std::runtime_error{ "A word's size should be greater or equal two." };

    const auto it = words.find(word);
    if (it == words.end()) [[unlikely]] throw std::runtime_error{ "The crossword doesn't consist of this word." };

    const auto& wordParams = it->second;
    if (wordParams.orientation == WordOrientation::HORIZONTAL)
    {
        for (std::size_t i = 0; i < word.size(); ++i)
        {
            auto& cell = board[wordParams.start.y][static_cast<std::size_t>(wordParams.start.x) + i];
            if (cell.letter() != word[i] ||
                cell.orientation() == CellOrientation::VERTICAL ||
                cell.orientation() == CellOrientation::NONE) [[unlikely]]
            {
                throw std::runtime_error{ "The crossword doesn't consist of this word in this position." };
            }

            if (cell.orientation() == CellOrientation::BOTH) --intersectionN;
            cell.removeHorizontalLetter();
        }
    }
    else
    {
        for (std::size_t i = 0; i < word.size(); ++i)
        {
            auto& cell = board[static_cast<std::size_t>(wordParams.start.y) + i][wordParams.start.x];
            if (cell.letter() != word[i] ||
                cell.orientation() == CellOrientation::HORIZONTAL ||
                cell.orientation() == CellOrientation::NONE) [[unlikely]]
            {
                throw std::runtime_error{ "The crossword doesn't consist of this word in this position." };
            }

            if (cell.orientation() == CellOrientation::BOTH) --intersectionN;
            cell.removeVerticalLetter();
        }
    }

    limitKeeper.removeWord(wordParams, word.size());
    letterN -= word.size();
    words.erase(it);
}

Position Crossword::getCoordinateStart() const noexcept
{
    static constexpr auto X = MAX_CROSSWORD_WIDTH / 2, Y = MAX_CROSSWORD_HEIGHT / 2;
    return { X, Y };
}

CrosswordParams Crossword::getCrossword() const
{
    const auto limits = limitKeeper.getLimits();
    CrosswordParams res{ std::vector<Word>(words.size()), static_cast<size_t>(limits.right - limits.left),
            static_cast<size_t>(limits.bottom - limits.top) };
    std::transform(words.begin(), words.end(), res.words.begin(),
        [&limits](const auto& p) {return Word{ p.first, p.second - Position{limits.left, limits.top} }; });
    return res;
}
