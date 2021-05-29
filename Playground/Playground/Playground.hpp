#ifndef PLAYGROUND_HPP
#define PLAYGROUND_HPP

#include <algorithm>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

class Path
{
public:
	Path() = default;
	Path(const std::string& path);

	int getLength() const;
	int countNetEffect() const;

	Path computeNextUpPath() const;
	Path computeNextDownPath() const;

	friend std::hash<Path>;
	bool operator==(const Path& other) const;

	std::string print() const;

private:
	enum class Atom
	{
		Down = 0, Up = 1
	};
	std::vector<Atom> m_atoms{};
};

template<>
struct std::hash<Path>
{
	std::size_t operator()(const Path& path) const
	{
		return std::accumulate(std::begin(path.m_atoms), std::end(path.m_atoms), static_cast<size_t>(0),
			[](size_t hash, Path::Atom atom) { return hash ^ std::hash<int>()(static_cast<int>(atom)); });
	}
};

class MarketParameters
{
public:
	MarketParameters() = default;
	MarketParameters(double riskFreeRate, double upMagnitude);

	double compound(double value) const;

	std::string print() const;

private:
	double m_riskFreeRate = 0.25;
	double m_upMagnitude = 2;
};

class Node
{
public:
	double getValue() const;
	void setValue(double value);

private:
	double m_value{};
};

class BinomialTree
{
public:
	BinomialTree(int numberPeriods);

	int getTimePeriods() const;

	double getValue(const Path& path) const;
	void setValue(const Path& path, double value);

private:
	std::map<int, std::unordered_map<Path, Node>> m_nodes{};
};

#endif // PLAYGROUND_HPP
