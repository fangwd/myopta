#include <cfloat>

#include "myopta.h"

namespace myopta {

Variable::Variable(int upper) : Variable(0, upper) {}
Variable::Variable(int lower, int upper) {
    lower_ = lower;
    upper_ = upper;
}

void Variable::Pick(Value& value, Rand& rand) const {
    auto range = upper_ - lower_;
    auto delta = rand.next(range);
    value = lower_ + (range > 0 ? delta : 0);
}

void Variable::Bound(Value& value) const {
    int range = upper_ - lower_;
    if (value < lower_) {
        value += range;
    } else if (value >= upper_) {
        value -= range;
    }
}

Problem::Problem(size_t size) {
    variables_.reserve(size);
}

Problem::~Problem() {
    for (auto v : variables_) {
        delete v;
    }
}

void Problem::Add(Variable* v) {
    variables_.push_back(v);
}

void InitSolution(const Problem& problem, Solution& solution, Rand& rand) {
    const auto& vars = problem.variables();
    for (size_t i = 0; i < vars.size(); i++) {
        vars[i]->Pick(solution.values[i], rand);
    }
    solution.fitness = INVALID_FITNESS;
}

}  // namespace myopta