#include "pch.h"

#include <Playground.hpp>

#include <numeric>
#include <unordered_map>

Path::Path(const std::string& path)
{
	std::transform(std::begin(path), std::end(path), std::back_inserter(m_atoms), 
		[](char c) { return c == '1' ? Atom::Up : Atom::Down; });
}

int Path::getLength() const
{
	return m_atoms.size();
}

int Path::countNetEffect() const
{
	return std::accumulate(std::begin(m_atoms), std::end(m_atoms), 0, 
		[](int netEffect, Atom atom) { return netEffect + 2*static_cast<int>(atom)-1;  });
}

Path Path::computeNextUpPath() const
{
	Path upPath{ *this };
	upPath.m_atoms.push_back(Atom::Up);
	return upPath;
}

Path Path::computeNextDownPath() const
{
	Path downPath{ *this };
	downPath.m_atoms.push_back(Atom::Down);
	return downPath;

}

bool Path::operator==(const Path& other) const
{
	return getLength() == other.getLength()
		&& std::equal(std::begin(m_atoms), std::end(m_atoms), std::begin(other.m_atoms));
}

std::string Path::print() const
{
	return std::accumulate(std::begin(m_atoms), std::end(m_atoms), std::string{}, 
		[](std::string path, Atom atom) { return std::move(path) + std::to_string(static_cast<int>(atom)); });
}

BinomialTree::BinomialTree(int numberPeriods)
{
	std::vector<Path> previousPeriodPaths{};
	previousPeriodPaths.push_back(Path{});
	auto timeZeroNodes = std::unordered_map<Path, Node>{};
	timeZeroNodes[Path{}] = {};
	m_nodes[0] = std::move(timeZeroNodes);

	for (int period = 1; period <= numberPeriods; ++period)
	{
		std::vector<Path> currentPeriodPaths{};
		std::unordered_map<Path, Node> currentTimeNodes{};
		std::for_each(std::begin(previousPeriodPaths), std::end(previousPeriodPaths),
			[&](const Path& path) 
			{ 
				Path upPath{ path.computeNextUpPath() };
				currentPeriodPaths.push_back(upPath);
				currentTimeNodes[upPath] = {};

				Path downPath{ path.computeNextDownPath() };
				currentPeriodPaths.push_back(downPath);
				currentTimeNodes[downPath] = {};
			});
		previousPeriodPaths = std::move(currentPeriodPaths);
		m_nodes[period] = std::move(currentTimeNodes);
	}
}

int BinomialTree::getTimePeriods() const
{
	return m_nodes.size() - 1;
}

double BinomialTree::getValue(const Path& path) const
{
	const int maximumTimePeriod = getTimePeriods();
	const int pathLength = path.getLength();
	if (pathLength > maximumTimePeriod)
		return 0.;

	const auto nodesAtRelevantTime = m_nodes.at(pathLength);
	auto it = nodesAtRelevantTime.find(path);
	return it != std::end(nodesAtRelevantTime) ? nodesAtRelevantTime.at(path).getValue() : 0.;
}

void BinomialTree::setValue(const Path& path, double value)
{
	const int maximumTimePeriod = getTimePeriods();
	const int pathLength = path.getLength();
	if (pathLength > maximumTimePeriod)
		return;

	m_nodes.at(pathLength)[path].setValue(value);
}

MarketParameters::MarketParameters(double riskFreeRate, double upMagnitude)
{
	// rate and magnitude validity 
	if (riskFreeRate < -1. || upMagnitude < 1.)
	{
		m_riskFreeRate = 0.25;
		m_upMagnitude = 2.;
	}
	// No-arbitrage condition
	else if (1 + riskFreeRate > upMagnitude || upMagnitude * (1 + riskFreeRate) < 1)
	{
		m_riskFreeRate = 0.25;
		m_upMagnitude = 2.;
	}
	else
	{
		m_riskFreeRate = riskFreeRate;
		m_upMagnitude = upMagnitude;
	}
}

double MarketParameters::compound(double value) const
{
	return value * (1+m_riskFreeRate);
}

std::string MarketParameters::print() const
{
	return "r=" + std::to_string(m_riskFreeRate) + " " 
		+ "u=" + std::to_string(m_upMagnitude);
}

double Node::getValue() const
{
	return m_value;
}

void Node::setValue(double value)
{
	m_value = value;
}
