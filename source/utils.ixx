#include <atomic>
#include <time.h>

export module utils;

export class spinlock {
  public:
    spinlock() noexcept : lock_{false} {}
    auto lock() noexcept -> void
    {
        while (lock_.exchange(true, std::memory_order_acquire)) {
        }
    }
    auto unlock() noexcept -> void
    {
        lock_.store(false, std::memory_order_release);
    }

  private:
    std::atomic<bool> lock_;
};