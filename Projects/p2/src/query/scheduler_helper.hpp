#ifndef SRC_QUERY_SCHEDULER_HELPER_HPP_
#define SRC_QUERY_SCHEDULER_HELPER_HPP_

#include <atomic>
#include <cctype>
#include <condition_variable>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Query.h"
#include "QueryBuilders.h"
#include "QueryParser.h"
#include "QueryResult.h"

#define DEBUG_SCHEDULER_LOG (0)

using Seq = std::size_t;

static auto extractQueryString(std::istream &istream) -> std::string {
  std::string buf;
  while (true) {
    const int chapter = istream.get();
    if (chapter == ';') {
      return buf;
    }
    if (chapter == EOF) {
      throw std::ios_base::failure("End of input");
    }
    buf.push_back(static_cast<char>(chapter));
  }
}

struct Task {
  Seq seq{};
  Query::Ptr query;
  std::string table;
  bool isRead{false};
  bool isLoad{false};
  bool isDbLevel{false};
  std::vector<Seq> waitAllSeqs;
};

// NOLINTBEGIN
static auto startPrinterThread(std::mutex &bufferMutex,
                               std::map<Seq, std::string> &resultBuffer,
                               std::atomic<Seq> &flushUpTo,
                               std::atomic<Seq> &finalSeq, std::mutex &depMutex,
                               std::condition_variable &depCv,
                               std::unordered_set<Seq> &doneSeqs)
    // NOLINTEND
    -> std::thread {
  return std::thread([&]() -> void {
    Seq nextToPrint = 1;
    for (;;) {
      std::unique_lock<std::mutex> lock(depMutex);
      depCv.wait(lock, [&]() -> bool {
        return (nextToPrint <= flushUpTo.load() &&
                doneSeqs.find(nextToPrint) != doneSeqs.end()) ||
               (finalSeq.load() != 0 && nextToPrint > finalSeq.load());
      });
      if (finalSeq.load() != 0 && nextToPrint > finalSeq.load()) {
        break;
      }
      while (nextToPrint <= flushUpTo.load()) {
        if (doneSeqs.find(nextToPrint) == doneSeqs.end()) {
          break;
        }
        std::string str;
        {
          const std::scoped_lock bufferlock(bufferMutex);
          if (auto iter = resultBuffer.find(nextToPrint);
              iter != resultBuffer.end()) {
            str = std::move(iter->second);
            resultBuffer.erase(iter);
          }
        }
        lock.unlock();
        if (!str.empty()) {
          std::cout << str;
          std::cout.flush();
        }
        ++nextToPrint;
        lock.lock();
      }
    }
  });
}

// NOLINTBEGIN
static auto
makeWorkerFunc(std::mutex &coutMutex, std::map<Seq, std::string> &resultBuffer,
               std::mutex &depMutex, std::condition_variable &depCv,
               std::unordered_set<Seq> &doneSeqs, std::mutex &bufferMutex)
    // NOLINTEND
    -> std::function<void(Task)> {
  return [&](Task task) -> void {
    try {
      if (!task.waitAllSeqs.empty()) {
#if DEBUG_SCHEDULER_LOG
        // Debug: log what this task is waiting on
        std::cerr << "[WORKER] Task " << task.seq << " waiting on [";
        for (size_t i = 0; i < task.waitAllSeqs.size(); ++i) {
          if (i)
            std::cerr << ",";
          std::cerr << task.waitAllSeqs[i];
        }
        std::cerr << "] (table='" << task.table << "')\n";
#endif

        std::unique_lock<std::mutex> lock(depMutex);
        depCv.wait(lock, [&]() -> bool {
          for (auto seq : task.waitAllSeqs) {
            if (doneSeqs.find(seq) == doneSeqs.end()) {
              return false;
            }
          }
          return true;
        });

#if DEBUG_SCHEDULER_LOG
        // Debug: woke up
        std::cerr << "[WORKER] Task " << task.seq << " woke\n";
#endif
      }

#if DEBUG_SCHEDULER_LOG
      // Debug: mark start of execution for this task
      try {
        std::cerr << "[EXEC] Task " << task.seq
                  << " start: " << task.query->toString() << "\n";
      } catch (...) {
        std::cerr << "[EXEC] Task " << task.seq
                  << " start: <toString() failed>\n";
      }
#endif
      QueryResult::Ptr result = task.query->execute();
#if DEBUG_SCHEDULER_LOG
      // Debug: mark end of execution for this task
      try {
        std::cerr << "[EXEC] Task " << task.seq
                  << " end: " << task.query->toString() << "\n";
      } catch (...) {
        std::cerr << "[EXEC] Task " << task.seq
                  << " end: <toString() failed>\n";
      }
#endif
      std::ostringstream out;
      out << task.seq << "\n";
      if (result->success()) {
        out << *result;
      } else {
        out << "QUERY FAILED:\n\t" << *result;
      }
      {
        const std::scoped_lock lockk(bufferMutex);
        resultBuffer[task.seq] = std::move(out).str();
      }
      {
        const std::scoped_lock locking(depMutex);
        doneSeqs.insert(task.seq);
#if DEBUG_SCHEDULER_LOG
        std::cerr << "[WORKER] Task " << task.seq
                  << " marked done (thread=" << std::this_thread::get_id()
                  << ")\n";
#endif
      }
      depCv.notify_all();
    } catch (const std::exception &e) {
      const std::scoped_lock locker(coutMutex);
      std::cerr << "Worker exception: " << e.what() << "\n";
      {
        const std::scoped_lock lockers(depMutex);
        doneSeqs.insert(task.seq);
      }
      depCv.notify_all();
    }
  };
}

static void
registerQueryBuilders(QueryParser &parser) {  // NOLINT(runtime/references)
  parser.registerQueryBuilder(
      std::make_unique<typename QueryBuilderType<DebugTag>::type>());
  parser.registerQueryBuilder(
      std::make_unique<typename QueryBuilderType<ManageTableTag>::type>());
  parser.registerQueryBuilder(
      std::make_unique<typename QueryBuilderType<ComplexTag>::type>());
}

static void collectAllPendingReads(
    const std::unordered_map<std::string, std::unordered_set<Seq>>
        &pendingReadsBeforeWrite,
    std::vector<Seq> &waitAll) {  // NOLINT(runtime/references)
  for (const auto &key_value : pendingReadsBeforeWrite) {
    waitAll.insert(waitAll.end(), key_value.second.begin(),
                   key_value.second.end());
  }
}

static void
addLastGlobalWrite(Seq lastGlobalWriteSeq,       // NOLINT(runtime/references)
                   std::vector<Seq> &waitAll) {  // NOLINT(runtime/references)
  if (lastGlobalWriteSeq != 0) {
    waitAll.push_back(lastGlobalWriteSeq);
  }
}

static void collectReadDependencies(
    const Task &task, const std::unordered_map<std::string, Seq> &lastWriteSeq,
    Seq lastGlobalWriteSeq,
    std::vector<Seq> &waitAll) {  // NOLINT(runtime/references)
  auto itW = lastWriteSeq.find(task.table);
  if (itW != lastWriteSeq.end()) {
    waitAll.push_back(itW->second);
  }
  addLastGlobalWrite(lastGlobalWriteSeq, waitAll);
}

#endif  // SRC_QUERY_SCHEDULER_HELPER_HPP_
