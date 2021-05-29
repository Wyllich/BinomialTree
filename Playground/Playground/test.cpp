#include "pch.h"

#include <Playground.hpp>

#include <cmath>

TEST(path_test, static_construction)
{
	const std::string zeroPathLabel = "0";
	Path zeroPath{ zeroPathLabel };
	EXPECT_EQ(zeroPath.print(), zeroPathLabel);

	Path letteredPath{ "abcd" };
	EXPECT_EQ(letteredPath.print(), "0000");

	const std::string longPathLabel = "01010101";
	Path validPath{ longPathLabel };
	EXPECT_EQ(validPath.print(), longPathLabel);
}

TEST(path_test, dynamic_construction)
{
	Path path{};
	EXPECT_EQ(path.getLength(), 0);
	path = path.computeNextDownPath().computeNextDownPath().computeNextUpPath()
		.computeNextDownPath().computeNextUpPath().computeNextDownPath();
	EXPECT_EQ(path.getLength(), 6);
	EXPECT_EQ(path.print(), "001010");
}

TEST(path_test, comparison)
{
	Path emptyPath{};
	Path path1{ std::string("001010") };
	EXPECT_FALSE(emptyPath == path1);
	EXPECT_TRUE(path1 == path1);

	Path path2{ std::string("010010") };
	EXPECT_FALSE(path1 == path2);
}

TEST(path_test, net_effect)
{
	Path path{};
	EXPECT_EQ(path.countNetEffect(), 0);
	path = path.computeNextUpPath().computeNextDownPath();
	EXPECT_EQ(path.countNetEffect(), 0);
	path = path.computeNextUpPath().computeNextUpPath();
	EXPECT_EQ(path.countNetEffect(), 2);
}
TEST(market_parameters_test, construction)
{
	MarketParameters defaultMP{};
	
	MarketParameters unrealisticNegativeRiskFreeRateMP{ -100., 1. };
	EXPECT_EQ(defaultMP.print(), unrealisticNegativeRiskFreeRateMP.print());

	MarketParameters nullUpMagnitudeMP{ 0.25, 0. };
	EXPECT_EQ(defaultMP.print(), nullUpMagnitudeMP.print());

	MarketParameters withUpArbitrageMP{ 0.25, 1.15 };
	EXPECT_EQ(defaultMP.print(), withUpArbitrageMP.print());

	MarketParameters withDownArbitrageMP{ 0.3, 0.75 };
	EXPECT_EQ(defaultMP.print(), withDownArbitrageMP.print());

	MarketParameters noArbitrageMP{ 0.5, 3. };
	EXPECT_NE(defaultMP.print(), noArbitrageMP.print());
}
TEST(market_parameters_test, compounding)
{
	MarketParameters defaultMP{};
	const double defaultInterestRate{ 0.25 };

	const double initialWealth = 1.;
	const double onePeriodCompoundedWealth = initialWealth * (1 + defaultInterestRate);
	EXPECT_EQ(defaultMP.compound(initialWealth), onePeriodCompoundedWealth);

	const double threePeriodCompoundedWealth = initialWealth * pow(1 + defaultInterestRate, 3);
	EXPECT_EQ(defaultMP.compound(defaultMP.compound(defaultMP.compound(initialWealth))), threePeriodCompoundedWealth);
}

TEST(binomial_tree_test, empty_tree_construction)
{
	BinomialTree tree{ 2 };
	EXPECT_EQ(tree.getValue(std::string("")), 0.);
	EXPECT_EQ(tree.getValue(std::string("0")), 0.);
	EXPECT_EQ(tree.getValue(std::string("1")), 0.);
	EXPECT_EQ(tree.getValue(std::string("00")), 0.);
	EXPECT_EQ(tree.getValue(std::string("01")), 0.);
	EXPECT_EQ(tree.getValue(std::string("10")), 0.);
	EXPECT_EQ(tree.getValue(std::string("11")), 0.);
}

TEST(binomial_tree_test, number_periods)
{
	BinomialTree leafTree{ 0 };
	EXPECT_EQ(leafTree.getTimePeriods(), 0);

	BinomialTree smallTree{ 3 };
	EXPECT_EQ(smallTree.getTimePeriods(), 3);
}

TEST(binomial_tree_test, get_set)
{
	BinomialTree tree{ 2 };
	EXPECT_EQ(tree.getValue(std::string("01")), 0.);
	tree.setValue(std::string("01"), 5.1);
	EXPECT_EQ(tree.getValue(std::string("01")), 5.1);
}

TEST(binomial_tree_test, out_of_bounds_get_set)
{
	BinomialTree tree{ 2 };
	EXPECT_EQ(tree.getValue(std::string("0111")), 0.);
	tree.setValue(std::string("0111"), 7.12);
	EXPECT_EQ(tree.getValue(std::string("0111")), 0.);
}