#ifndef MYOPTA_MISC_H_
#define MYOPTA_MISC_H_

#include <algorithm>
#include <vector>

#include "myopta.h"

namespace myopta {

class EliteSet {
    std::vector<Solution*> data_;
    size_t size_;

    EliteSet( const EliteSet& ); // non construction-copyable
    EliteSet& operator=( const EliteSet& ); // non copyable

  public:
    explicit EliteSet(size_t size) : size_(size) {
        data_.reserve(size + 1);
    }

    void Add(Solution* value) {
        auto cmp = [](const Solution* lhs, const Solution* rhs) {
            return lhs->fitness > rhs->fitness;
        };
        auto pos = std::lower_bound(data_.begin(), data_.end(), value, cmp);
        data_.insert(pos, value);
        value->elite = true;
        if (data_.size() > size_) {
            auto back = data_.back();
            back->elite = false;
            data_.pop_back();
        }
    }

    inline std::vector<Solution*>& data() {
        return data_;
    }
};

}  // namespace myopta

#endif  // MYOPTA_MISC_H_
