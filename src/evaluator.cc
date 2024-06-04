#include "myopta.h"

namespace myopta {

ParallelEvaluator::ParallelEvaluator(EvaluatorFactory& factory, size_t thread_count)
    : population_(nullptr), next_index_(0), stopping_(false) {
    threads_.reserve(thread_count);
    evaluators_.reserve(thread_count);
    for (uint32_t i = 0; i < thread_count; i++) {
        evaluators_.push_back(factory.CreateEvaluator());
        threads_.emplace_back(EvaluatorWorker, this, i);
    }
}

ParallelEvaluator::~ParallelEvaluator() {
    Stop();
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ParallelEvaluator::EvaluatorWorker(ParallelEvaluator* parallel, size_t index) {
    while (!parallel->stopping_) {
        Solution* solution = nullptr;
        {
            std::unique_lock<std::mutex> lock(parallel->worker_mutex_);
            parallel->worker_cv_.wait(lock, [parallel]() {
                return parallel->unblocked();
            });
            solution = parallel->GetSolution();
        }
        if (solution != nullptr) {
            parallel->evaluators_[index]->Evaluate(*solution);
            parallel->master_cv_.notify_one();
        }
    }
}

void ParallelEvaluator::Evaluate(Population& population) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        population_ = &population;
        next_index_ = 0;
    }
    worker_cv_.notify_all();
    std::unique_lock<std::mutex> lock(mutex_);
    master_cv_.wait(lock, [&]() {
        return next_index_ >= population.size();
    });
    population_ = nullptr;
}

void ParallelEvaluator::Stop() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stopping_ = true;
    }
    worker_cv_.notify_all();
}

}  // namespace myopta