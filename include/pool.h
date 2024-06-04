#ifndef MYOPTA_POOL_H_
#define MYOPTA_POOL_H_

#include <cstdlib>
#include <new>

namespace myopta {

template<typename T, typename U>
class Pool {
    struct Node {
        Node *next;
        T data;
    };

  private:
    size_t capacity_;
    size_t value_count_;
    Node *pool_;
    Node *free_list_;

  public:
    Pool(size_t capacity, size_t value_count) : capacity_(capacity), value_count_(value_count) {
        size_t node_size = sizeof(Node) + value_count * sizeof(U);
        pool_ = static_cast<Node *>(std::malloc(capacity * node_size));
        std::memset(pool_, 0, capacity * node_size);
        if (!pool_) {
            throw std::bad_alloc();
        }
        Reset();
    }

    ~Pool() {
        std::free(pool_);
    }

    T *Allocate() {
        if (!free_list_) {
            return nullptr;
        }
        Node *node = free_list_;
        free_list_ = free_list_->next;
        node->next = nullptr;
        return &node->data;
    }

    T *Copy(T* p) {
        T *t = Allocate();
        if (t) {
            std::memcpy(t, p, sizeof(T) + value_count_ * sizeof(U));
        }
        return t;
    }

    void Deallocate(T* p) {
        Node *node = reinterpret_cast<Node *>(reinterpret_cast<char *>(p) - offsetof(Node, data));
        node->next = free_list_;
        free_list_ = node;
    }

    void Reset() {
        free_list_ = nullptr;

        size_t node_size = sizeof(Node) + value_count_ * sizeof(U);
        for (size_t i = 0; i < capacity_; ++i) {
            auto addr = (char *)pool_ + node_size * i;
            auto node = new (addr) Node();
            Deallocate(&node->data);
        }
    }

    size_t CountElites() {
        size_t node_size = sizeof(Node) + value_count_ * sizeof(U);
        size_t elite_count = 0;
        for (size_t i = 0; i < capacity_; ++i) {
            auto addr = (char *)pool_ + node_size * i;
            auto node = (Node*)(addr);
            T* s = &node->data;
            if (s->elite) {
                elite_count++;
            }
        }
        return elite_count;
    }

    size_t GetSize() {

        size_t size = 0;
        Node* free_list = free_list_;
        while (free_list) {
            size++;
            free_list = free_list->next;
        }
        return size;
    }
};

}  // namespace myopta

#endif  // MYOPTA_POOL_H_
