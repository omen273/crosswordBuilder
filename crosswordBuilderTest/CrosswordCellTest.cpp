#include "pch.h"

#include "../crosswordBuilder/CrosswordCell.h"

#include <functional>

#define TEST_TWO_ORIENTATIONS(name1, name2) {\
	CrosswordCell cell;\
	cell.add##name1##Letter('A'); \
	cell.add##name2##Letter('A'); \
	ASSERT_EQ(CellOrientation::BOTH, cell.orientation());\
}

TEST(CrosswordCellTest, orientation)
{
    {
        CrosswordCell cell;
        ASSERT_EQ(CellOrientation::NONE, cell.orientation());
        cell.addHorizontalInsideLetter('A');
        ASSERT_EQ(CellOrientation::HORIZONTAL, cell.orientation());
        cell.addVerticalBeginLetter('A');
        ASSERT_EQ(CellOrientation::BOTH, cell.orientation());
        cell.addVerticalEndLetter('B');
        ASSERT_EQ(CellOrientation::VERTICAL, cell.orientation());
        cell.addHorizontalBeginLetter('C');
        ASSERT_EQ(CellOrientation::HORIZONTAL, cell.orientation());
        cell.removeVerticalLetter();
        ASSERT_EQ(CellOrientation::HORIZONTAL, cell.orientation());
        cell.removeHorizontalLetter();
        ASSERT_EQ(CellOrientation::NONE, cell.orientation());
        cell.addHorizontalInsideLetter('A');
        cell.addVerticalEndLetter('A');
        cell.removeHorizontalLetter();
        ASSERT_EQ(CellOrientation::VERTICAL, cell.orientation());
        cell.addHorizontalInsideLetter('A');
        cell.removeVerticalLetter();
        ASSERT_EQ(CellOrientation::HORIZONTAL, cell.orientation());
    }

    {
        CrosswordCell cell;
        cell.addHorizontalBeginLetter('A');
        ASSERT_EQ(CellOrientation::HORIZONTAL, cell.orientation());
    }

    {
        CrosswordCell cell;
        cell.addHorizontalInsideLetter('A');
        ASSERT_EQ(CellOrientation::HORIZONTAL, cell.orientation());
    }

    {
        CrosswordCell cell;
        cell.addHorizontalEndLetter('A');
        ASSERT_EQ(CellOrientation::HORIZONTAL, cell.orientation());
    }

    {
        CrosswordCell cell;
        cell.addVerticalBeginLetter('A');
        ASSERT_EQ(CellOrientation::VERTICAL, cell.orientation());
    }

    {
        CrosswordCell cell;
        cell.addVerticalInsideLetter('A');
        ASSERT_EQ(CellOrientation::VERTICAL, cell.orientation());
    }

    {
        CrosswordCell cell;
        cell.addVerticalEndLetter('A');
        ASSERT_EQ(CellOrientation::VERTICAL, cell.orientation());
    }

    TEST_TWO_ORIENTATIONS(HorizontalBegin, VerticalBegin)
        TEST_TWO_ORIENTATIONS(HorizontalBegin, VerticalInside)
        TEST_TWO_ORIENTATIONS(HorizontalBegin, VerticalEnd)
        TEST_TWO_ORIENTATIONS(HorizontalInside, VerticalBegin)
        TEST_TWO_ORIENTATIONS(HorizontalInside, VerticalInside)
        TEST_TWO_ORIENTATIONS(HorizontalInside, VerticalEnd)
        TEST_TWO_ORIENTATIONS(HorizontalEnd, VerticalBegin)
        TEST_TWO_ORIENTATIONS(HorizontalEnd, VerticalInside)
        TEST_TWO_ORIENTATIONS(HorizontalEnd, VerticalEnd)
}

#define TEST_IS(name)\
	TEST(CrossowrdCellTest, is##name)\
	{\
		CrosswordCell cell;\
		cell.add##name##Letter('A');\
		ASSERT_TRUE(cell.is##name##());\
	}

TEST_IS(HorizontalBegin)
TEST_IS(HorizontalEnd)
TEST_IS(VerticalBegin)
TEST_IS(VerticalEnd)

TEST(CrossowrdCellTest, addHorizontalBeginLetter)
{
    CrosswordCell cell;
    cell.addHorizontalBeginLetter('A');
    ASSERT_EQ('A', cell.letter());
    ASSERT_TRUE(cell.isHorizontalBegin());
    cell.addHorizontalBeginLetter('B');
    ASSERT_EQ('B', cell.letter());
    ASSERT_TRUE(cell.isHorizontalBegin());
    cell.addVerticalBeginLetter('B');
    ASSERT_TRUE(cell.isHorizontalBegin());
    cell.addVerticalBeginLetter('C');
    ASSERT_FALSE(cell.isHorizontalBegin());
}

TEST(CrossowrdCellTest, addHorizontalInsideLetter)
{
    CrosswordCell cell;
    cell.addHorizontalInsideLetter('A');
    ASSERT_EQ('A', cell.letter());
    ASSERT_EQ(cell.orientation(), CellOrientation::HORIZONTAL);
    cell.addHorizontalInsideLetter('B');
    ASSERT_EQ('B', cell.letter());
    ASSERT_EQ(cell.orientation(), CellOrientation::HORIZONTAL);
}

TEST(CrossowrdCellTest, addHorizontalEndLetter)
{
    CrosswordCell cell;
    cell.addHorizontalEndLetter('A');
    ASSERT_EQ('A', cell.letter());
    ASSERT_TRUE(cell.isHorizontalEnd());
    cell.addHorizontalEndLetter('B');
    ASSERT_EQ('B', cell.letter());
    ASSERT_TRUE(cell.isHorizontalEnd());
    cell.addVerticalBeginLetter('B');
    ASSERT_TRUE(cell.isHorizontalEnd());
    cell.addVerticalBeginLetter('C');
    ASSERT_FALSE(cell.isHorizontalEnd());
}


TEST(CrossowrdCellTest, addVerticalBeginLetter)
{
    CrosswordCell cell;
    cell.addVerticalBeginLetter('A');
    ASSERT_EQ('A', cell.letter());
    ASSERT_TRUE(cell.isVerticalBegin());
    cell.addVerticalBeginLetter('B');
    ASSERT_EQ('B', cell.letter());
    ASSERT_TRUE(cell.isVerticalBegin());
    cell.addHorizontalBeginLetter('B');
    ASSERT_TRUE(cell.isVerticalBegin());
    cell.addHorizontalBeginLetter('C');
    ASSERT_FALSE(cell.isVerticalBegin());
}

TEST(CrossowrdCellTest, addVerticalInsideLetter)
{
    CrosswordCell cell;
    cell.addVerticalInsideLetter('A');
    ASSERT_EQ('A', cell.letter());
    ASSERT_EQ(cell.orientation(), CellOrientation::VERTICAL);
    cell.addVerticalInsideLetter('B');
    ASSERT_EQ('B', cell.letter());
    ASSERT_EQ(cell.orientation(), CellOrientation::VERTICAL);
}

TEST(CrossowrdCellTest, addVerticalEndLetter)
{
    CrosswordCell cell;
    cell.addVerticalEndLetter('A');
    ASSERT_EQ('A', cell.letter());
    ASSERT_TRUE(cell.isVerticalEnd());
    cell.addVerticalEndLetter('B');
    ASSERT_EQ('B', cell.letter());
    ASSERT_TRUE(cell.isVerticalEnd());
    cell.addHorizontalBeginLetter('B');
    ASSERT_TRUE(cell.isVerticalEnd());
    cell.addHorizontalBeginLetter('C');
    ASSERT_FALSE(cell.isHorizontalEnd());
}

TEST(CrossowrdCellTest, removeHorizontalLetter)
{
    CrosswordCell cell;
    cell.removeHorizontalLetter();
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::NONE, cell.orientation());
    cell.addHorizontalBeginLetter('A');
    cell.removeHorizontalLetter();
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::NONE, cell.orientation());
    cell.addHorizontalInsideLetter('A');
    cell.removeHorizontalLetter();
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::NONE, cell.orientation());
    cell.addHorizontalEndLetter('A');
    cell.removeHorizontalLetter();
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::NONE, cell.orientation());
    cell.addHorizontalEndLetter('A');
    cell.addVerticalBeginLetter('A');
    cell.removeHorizontalLetter();
    ASSERT_EQ('A', cell.letter());
    ASSERT_EQ(CellOrientation::VERTICAL, cell.orientation());
}

TEST(CrossowrdCellTest, removeVerticalLetter)
{
    CrosswordCell cell;
    cell.removeVerticalLetter();
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::NONE, cell.orientation());
    cell.addVerticalBeginLetter('A');
    cell.removeVerticalLetter();
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::NONE, cell.orientation());
    cell.addVerticalInsideLetter('A');
    cell.removeVerticalLetter();
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::NONE, cell.orientation());
    cell.addVerticalEndLetter('A');
    cell.removeVerticalLetter();
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::NONE, cell.orientation());
    cell.addHorizontalEndLetter('A');
    cell.addVerticalBeginLetter('A');
    cell.removeVerticalLetter();
    ASSERT_EQ('A', cell.letter());
    ASSERT_EQ(CellOrientation::HORIZONTAL, cell.orientation());
}

TEST(CrossowrdCellTest, addSpace)
{
    CrosswordCell cell;
    cell.addHorizontalEndLetter(' ');
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::HORIZONTAL, cell.orientation());
    cell.addVerticalInsideLetter(' ');
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::BOTH, cell.orientation());
    cell.removeHorizontalLetter();
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::VERTICAL, cell.orientation());
    cell.removeVerticalLetter();
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::NONE, cell.orientation());
    cell.addHorizontalEndLetter('A');
    cell.addVerticalInsideLetter(' ');
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::VERTICAL, cell.orientation());
    cell.removeVerticalLetter();
    cell.addVerticalEndLetter('A');
    cell.addHorizontalInsideLetter(' ');
    ASSERT_EQ(' ', cell.letter());
    ASSERT_EQ(CellOrientation::HORIZONTAL, cell.orientation());
}
