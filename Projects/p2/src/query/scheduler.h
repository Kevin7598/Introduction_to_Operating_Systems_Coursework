//
// Created by kaiqizhu on 11/6/25.
//

#ifndef SRC_QUERY_SCHEDULER_H_
#define SRC_QUERY_SCHEDULER_H_

#include <atomic>
#include <condition_variable>  // NOLINT(build/c++11)
#include <cstddef>
#include <functional>
#include <istream>
#include <mutex>  // NOLINT(build/c++11)
#include <optional>
#include <queue>
#include <string>
#include <thread>  // NOLINT(build/c++11)
#include <utility>
#include <vector>

template <typename T> class ConcurrentQueue {
private:
  std::queue<T> q{};
  mutable std::mutex mutexes;
  std::condition_variable c_var;
  bool closed = false;

public:
  ConcurrentQueue() = default;
  void push(T item) {
    {
      std::scoped_lock lock(mutexes);  // NOLINT(misc-const-correctness)
      if (closed) {
        throw std::runtime_error("push to closed queue");
      }
      q.push(std::move(item));
    }
    c_var.notify_one();
  }

  // pop waits until an item is available or queue is closed
  auto pop() -> std::optional<T> {
    std::unique_lock<std::mutex> locking(mutexes);
    c_var.wait(locking, [&]() -> auto { return !q.empty() || closed; });
    if (q.empty()) {
      return std::nullopt;
    }
    T item = std::move(q.front());
    q.pop();
    return item;
  }

  // close the queue, notify all waiting threads
  void close() {
    {
      std::scoped_lock locker(mutexes);  // NOLINT(misc-const-correctness)
      closed = true;
    }
    c_var.notify_all();
  }
};

class WorkerPool {
private:
  std::vector<std::thread> workers;
  std::atomic<bool> running{true};

public:
  template <typename QueueT, typename Func>
  WorkerPool(size_t n, QueueT &queue,  // NOLINT(runtime/references)
             const Func &workerFunc)
      : workers(), running(true) {  // NOLINT(readability-redundant-member-init)
    workers.reserve(n);
    for (size_t i = 0; i < n; ++i) {
      workers.emplace_back([&queue, workerFunc]() -> auto {
        while (true) {
          auto opt = queue.pop();
          if (!opt.has_value()) {
            break;
          }
          workerFunc(std::move(opt.value()));
        }
      });
    }
  }

  void joinAll() {
    for (auto &threads : workers) {
      if (threads.joinable()) {
        threads.join();
      }
    }
  }
};

// High-level query scheduler
class QueryScheduler {
private:
  std::size_t nthreads_{};

public:
  explicit QueryScheduler(std::size_t nthreads) : nthreads_(nthreads) {}
  // Execute all queries from input stream with internal worker pool and
  // streaming printer. Returns 0 on success.

  auto execute(std::istream &istream) const -> int;
};

#endif  // SRC_QUERY_SCHEDULER_H_
