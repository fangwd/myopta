#ifndef MYOPTA_H_
#define MYOPTA_H_

#include <memory>
#include <thread>
#include <vector>

#include "rand.h"
#include "pool.h"

namespace myopta {

typedef int Value;

struct Solution {
    double fitness;
    bool elite;
    Value values[];
};


typedef std::vector<Solution*> Population;
typedef Pool<Solution, Value> SolutionPool;

inline bool operator>(const Solution &lhs, const Solution &rhs) {
    return lhs.fitness > rhs.fitness;
}

class Evaluator {
  public:
    virtual ~Evaluator() {}
    virtual void Evaluate(Solution&) = 0;
};

class EvaluatorFactory {
  public:
    virtual ~EvaluatorFactory() {}
    virtual std::shared_ptr<Evaluator> CreateEvaluator() = 0;
};

// A parallel evaluator takes an evaluator factory and evaluates a population in parallel.
class ParallelEvaluator {
  private:
    std::vector<std::thread> threads_;
    std::vector<std::shared_ptr<Evaluator>> evaluators_;
    Population* population_;
    size_t next_index_;
    bool stopping_;

    std::mutex worker_mutex_;
    std::mutex mutex_;
    std::condition_variable worker_cv_;
    std::condition_variable master_cv_;

  public:
    ParallelEvaluator(EvaluatorFactory&, size_t);
    ~ParallelEvaluator();

    void Evaluate(Population&);
    void Stop();

    static void EvaluatorWorker(ParallelEvaluator*, size_t);

    bool unblocked() {
        std::lock_guard<std::mutex> lock(mutex_);
        return stopping_ || (population_ && population_->size() > next_index_);
    }

    Solution* GetSolution() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (population_ && population_->size() > next_index_) {
            return population_->at(next_index_++);
        }
        return nullptr;
    }

};

class Variable {
  private:
    Value lower_;
    Value upper_;

  public:
    Variable(int, int);
    Variable(int);
    void Pick(Value&, Rand&) const;
    void Bound(Value&) const;
};

class Problem {
  private:
    std::vector<Variable*> variables_;

    Problem(const Problem& other) = delete;
    Problem& operator=(const Problem& other) = delete;

  public:
    Problem(size_t size = 0);
    ~Problem();

    inline const std::vector<Variable*>& variables() const {
        return variables_;
    }

    inline size_t size() const {
        return variables_.size();
    }

    void Add(Variable*);
};

void InitSolution(const Problem&, Solution&, Rand&);

#define INVALID_FITNESS -1.0

}  // namespace myopta

#endif  // MYOPTA_H_
