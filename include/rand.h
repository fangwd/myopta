#ifndef MYOPTA_RAND_H_
#define MYOPTA_RAND_H_

namespace myopta {

class Rand {
  public:
    virtual ~Rand() {}
    virtual int next(int) = 0;
    virtual double next_double() = 0;
};

class Random : public Rand {
  private:
    static const long multiplier = 0x5DEECE66DL;
    static const long addend = 0xBL;
    static const long mask = (1L << 48) - 1;

    long seed;

    int _next(int bits) {
        seed = (seed * multiplier + addend) & mask;
        return (int)(seed >> (48 - bits));
    }

  public:
    Random(long seed) {
        this->seed = (seed ^ multiplier) & mask;
    }

    int next() {
        return _next(32);
    }

    int next(int n) override {
        if ((n & -n) == n)  // n is a power of 2
            return (int)((n * (long)_next(31)) >> 31);
        int bits, val;
        do {
            bits = _next(31);
            val = bits % n;
        } while (bits - val + (n - 1) < 0);
        return val;
    }

    long next_long() {
        return ((long)(_next(32)) << 32) + _next(32);
    }

    double next_double() override {
        return (((long)(_next(26)) << 27) + _next(27)) / (double)(1L << 53);
    }
};

}  // namespace myopta

#endif  // MYOPTA_RAND_H_
