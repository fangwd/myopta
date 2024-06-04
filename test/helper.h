#ifndef MYOPTA_TEST_HELPER_H_
#define MYOPTA_TEST_HELPER_H_

#include "rand.h"
#include <vector>

namespace myopta {

class DeterministicRand : public Rand {
  private:
    std::vector<int> i_vals_{0};
    std::vector<double> d_vals_{0.0};
    int i_idx_{0};
    int d_idx_{0};

  public:
    int next(int n) override {
        return i_vals_[i_idx_++ % i_vals_.size()];
    }
    double next_double() override {
        return d_vals_[d_idx_++ % d_vals_.size()];
    }

    void SetValues(const std::vector<int>& values) {
        i_vals_ = values;
        i_idx_ = 0;
    }

    void SetValues(const std::vector<double>& values) {
        d_vals_ = values;
        d_idx_ = 0;
    }
};
}

#endif // MYOPTA_TEST_HELPER_H_