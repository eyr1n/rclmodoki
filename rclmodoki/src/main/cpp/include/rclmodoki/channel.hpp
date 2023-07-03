#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace rclmodoki {

template <class T> class Channel {
public:
  void produce(T &&x) {
    std::lock_guard lock{mtx_};
    if (!closed_) {
      queue_.push(std::forward<T>(x));
      cv_.notify_all();
    }
  }

  bool consume(T &x) {
    std::unique_lock lock{mtx_};
    cv_.wait(lock, [this] { return !queue_.empty() || closed_; });
    if (closed_) {
      return false;
    }
    x = std::move(queue_.front());
    queue_.pop();
    cv_.notify_all();
    return true;
  }

  void close() {
    std::lock_guard lock{mtx_};
    closed_ = true;
    cv_.notify_all();
  }

private:
  std::queue<T> queue_;
  bool closed_ = false;
  std::mutex mtx_;
  std::condition_variable cv_;
};

} // namespace rclmodoki
