#ifndef MYOPTA_COMBINER_H_
#define MYOPTA_COMBINER_H_

#include <functional>
#include <memory>

#include "myopta.h"

namespace myopta {

enum class CrossoverMethod {
    OnePoint,
    TwoPoint,
    Uniform,
};

struct CrossoverConfig {
    CrossoverMethod method;
    float alpha;

    CrossoverConfig(CrossoverMethod method, float alpha = 0.5) : method(method), alpha(alpha) {}
};

class CrossoverOperator {
  public:
    virtual ~CrossoverOperator() {}
    virtual void Perform(Solution&, Solution&) = 0;
};

std::unique_ptr<CrossoverOperator> CreateCrossoverOperator(const Problem&, const CrossoverConfig&, Rand&);

}  // namespace myopta

#endif  // MYOPTA_COMBINER_H_