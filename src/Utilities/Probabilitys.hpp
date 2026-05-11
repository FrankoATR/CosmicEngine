#include <iostream>
#include <list>
#include <random>
#include <stdexcept>

class ProbabilisticGenerator {
public:
    ProbabilisticGenerator(const std::list<double>& probabilities) {
        double sum = 0.0;
        for (double prob : probabilities) {
            sum += prob;
        }
        if (std::abs(sum - 1.0) > 0.00001) {
            throw std::invalid_argument("Las probabilidades no suman exactamente 1");
        }

        double cumulative = 0.0;
        for (double p : probabilities) {
            cumulative += p;
            cumulativeDistribution.push_back(cumulative);
        }

        std::random_device rd;
        rng.seed(rd());
    }

    size_t generate() {
        double randValue = dist(rng);
        auto it = std::upper_bound(cumulativeDistribution.begin(), cumulativeDistribution.end(), randValue);
        return std::distance(cumulativeDistribution.begin(), it);
    }

private:
    std::list<double> cumulativeDistribution;
    std::mt19937 rng;
    std::uniform_real_distribution<> dist{ 0.0, 1.0 };
};