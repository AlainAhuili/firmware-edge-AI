#pragma once

#include <cstddef>

// Fixed-size FIFO window of sensor samples for TinyML inference.
// No heap allocation, no exceptions: safe for ISR-free use on the MCU.
// Not thread-safe: push and pop from the same task, or guard it externally.
template <typename T, std::size_t N>
class RingBuffer {
  static_assert(N > 0, "RingBuffer capacity must be at least 1");

 public:
  // Returns false (and drops the value) when the window is full.
  bool push(const T& value) {
    if (count_ >= N) {
      return false;
    }
    data_[head_] = value;
    head_ = (head_ + 1) % N;
    ++count_;
    return true;
  }

  bool pop(T& out) {
    if (count_ == 0) {
      return false;
    }
    out = data_[tail_];
    tail_ = (tail_ + 1) % N;
    --count_;
    return true;
  }

  // i = 0 is the oldest sample. Caller must ensure i < size().
  const T& at(std::size_t i) const { return data_[(tail_ + i) % N]; }

  std::size_t size() const { return count_; }
  static constexpr std::size_t capacity() { return N; }
  bool empty() const { return count_ == 0; }
  bool full() const { return count_ == N; }

 private:
  T data_[N]{};
  std::size_t head_ = 0;
  std::size_t tail_ = 0;
  std::size_t count_ = 0;
};

// Mean of the current window, a simple feature for the model. 0 when empty.
template <typename T, std::size_t N>
float window_mean(const RingBuffer<T, N>& rb) {
  if (rb.empty()) {
    return 0.0f;
  }
  float sum = 0.0f;
  for (std::size_t i = 0; i < rb.size(); ++i) {
    sum += static_cast<float>(rb.at(i));
  }
  return sum / static_cast<float>(rb.size());
}
