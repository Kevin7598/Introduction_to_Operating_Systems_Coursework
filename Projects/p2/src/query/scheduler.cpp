#include "scheduler.h"  // NOLINT

#include <algorithm>
#include <atomic>
#include <cctype>
#include <condition_variable>  // NOLINT(build/c++11)
#include <cstddef>
#include <cstdio>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>  // NOLINT(build/c++11)
#include <sstream>
#include <string>
#include <thread>  // NOLINT(build/c++11)
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Query.h"
#include "QueryParser.h"
#include "QueryResult.h"
#include "scheduler_helper.hpp"  // NOLINT

using Seq = std::size_t;

namespace {
void collectWriteDependencies(
    const Task &task,
    const std::unordered_map<std::string, std::unordered_set<Seq>>
        &pendingReadsBeforeWrite,
    const std::unordered_map<std::string, Seq> &lastWriteSeq,
    Seq lastGlobalWriteSeq,
    std::vector<Seq> &waitAll) {  // NOLINT(runtime/references)
  auto itSet = pendingReadsBeforeWrite.find(task.table);
  if (itSet != pendingReadsBeforeWrite.end()) {
    waitAll.insert(waitAll.end(), itSet->second.begin(), itSet->second.end());
  }

  auto itW = lastWriteSeq.find(task.table);
  if (itW != lastWriteSeq.end()) {
    waitAll.push_back(itW->second);
  }
  addLastGlobalWrite(lastGlobalWriteSeq, waitAll);
}
}  // namespace

namespace {
auto computeDependencies(
    const Task &task, const std::unordered_map<std::string, Seq> &lastWriteSeq,
    const std::unordered_map<std::string, std::unordered_set<Seq>>
        &pendingReadsBeforeWrite,
    Seq lastGlobalWriteSeq) -> std::vector<Seq> {
  std::vector<Seq> waitAll;
  if (task.isDbLevel) {
    collectAllPendingReads(pendingReadsBeforeWrite, waitAll);
    addLastGlobalWrite(lastGlobalWriteSeq, waitAll);
  } else if (!task.table.empty()) {
    if (task.isRead) {
      collectReadDependencies(task, lastWriteSeq, lastGlobalWriteSeq, waitAll);
    } else {
      collectWriteDependencies(task, pendingReadsBeforeWrite, lastWriteSeq,
                               lastGlobalWriteSeq, waitAll);
    }
  }
  return waitAll;
}
}  // namespace

namespace {
// NOLINTBEGIN
void updateDependencyState(
    std::unordered_map<std::string, Seq> &lastLoadSeq, const Task &task,
    std::unordered_map<std::string, Seq> &lastWriteSeq,
    std::unordered_map<std::string, std::unordered_set<Seq>>
        &pendingReadsBeforeWrite,
    Seq &lastGlobalWriteSeq) {
  // NOLINTEND
  if (task.isDbLevel) {
    for (auto &key_value : pendingReadsBeforeWrite) {
      key_value.second.clear();
    }
    lastGlobalWriteSeq = task.seq;
  } else if (!task.table.empty()) {
    if (task.isRead) {
      pendingReadsBeforeWrite[task.table].insert(task.seq);
    } else {
      pendingReadsBeforeWrite[task.table].clear();
      lastWriteSeq[task.table] = task.seq;
      lastGlobalWriteSeq = task.seq;
      if (task.isLoad) {
        lastLoadSeq[task.table] = task.seq;
      }
    }
  }
}
}  // namespace

namespace {
void handleBarrierAndNotify(
    const Task &task,
    std::atomic<Seq> &flushUpTo,       // NOLINT(runtime/references)
    std::condition_variable &depCv) {  // NOLINT(runtime/references)
  if (!task.isRead) {
    const Seq barrier = task.seq > 0 ? (task.seq - 1) : 0;
    if (barrier > flushUpTo.load()) {
      flushUpTo.store(barrier);
    }
    depCv.notify_all();
  }
}
}  // namespace

namespace {
auto parseAndScheduleQueries(
    std::istream &istream,
    ConcurrentQueue<Task> &queue,   /* NOLINT(runtime/references) */
    std::condition_variable &depCv, /* NOLINT(runtime/references) */
    std::atomic<Seq> &flushUpTo)    /* NOLINT(runtime/references) */
    -> std::tuple<Query::Ptr, Seq, Seq> {
  QueryParser parser;
  registerQueryBuilders(parser);

  std::unordered_map<std::string, Seq> lastLoadSeq;
  std::unordered_map<std::string, Seq> lastWriteSeq;
  std::unordered_map<std::string, std::unordered_set<Seq>>
      pendingReadsBeforeWrite;
  Seq lastGlobalWriteSeq = 0;
  Seq seq = 0;
  Seq quitSeq = 0;
  Query::Ptr quitQuery;

  while (istream) {
    try {
      std::string queryStr = extractQueryString(istream);
      if (std::all_of(
              queryStr.begin(), queryStr.end(), [](char character) -> int {
                return std::isspace(static_cast<unsigned char>(character));
              })) {
        continue;
      }

      Query::Ptr qobj = parser.parseQuery(queryStr);
      if (qobj->isQuit()) {
        quitQuery = std::move(qobj);
        quitSeq = ++seq;
        break;
      }

      Task tasks;
      tasks.seq = ++seq;
      tasks.isRead = qobj->isReadOnly();
      tasks.isLoad = qobj->isLoad();
      tasks.isDbLevel = qobj->isDbLevel();
      tasks.table = qobj->getTargetTable();
      tasks.query = std::move(qobj);
      tasks.waitAllSeqs = computeDependencies(
          tasks, lastWriteSeq, pendingReadsBeforeWrite, lastGlobalWriteSeq);
#if DEBUG_SCHEDULER_LOG
      // Debug: print scheduled task summary and its dependencies
      {
        std::cerr << "[SCHED] Scheduled task seq=" << tasks.seq << " table='"
                  << tasks.table << "' isRead=" << tasks.isRead
                  << " isLoad=" << tasks.isLoad << " waitAll=[";
        for (size_t i = 0; i < tasks.waitAllSeqs.size(); ++i) {
          if (i)
            std::cerr << ",";
          std::cerr << tasks.waitAllSeqs[i];
        }
        std::cerr << "]\n";
      }
#endif
      updateDependencyState(lastLoadSeq, tasks, lastWriteSeq,
                            pendingReadsBeforeWrite, lastGlobalWriteSeq);
      handleBarrierAndNotify(tasks, flushUpTo, depCv);
      queue.push(std::move(tasks));
    } catch (const std::ios_base::failure &) {
      break;
    }
  }
  queue.close();
  return {std::move(quitQuery), quitSeq, seq};
}
}  // namespace

namespace {
// NOLINTBEGIN
void finalizeExecution(Query::Ptr &quitQuery, Seq quitSeq, Seq seq,
                       std::mutex &bufferMutex,
                       std::map<Seq, std::string> &resultBuffer,
                       std::mutex &depMutex, std::condition_variable &depCv,
                       std::atomic<Seq> &flushUpTo, std::atomic<Seq> &finalSeq,
                       std::thread &printerThread, WorkerPool &pool,
                       std::unordered_set<Seq> &doneSeqs) {
  // NOLINTEND
  pool.joinAll();

  if (quitQuery) {
    auto result = quitQuery->execute();
    std::ostringstream out;
    if (result->success()) {
      out << *result;
    } else {
      out << "QUERY FAILED:\n\t" << *result;
    }
    {
      const std::scoped_lock lock(bufferMutex);
      resultBuffer[quitSeq] = out.str();
    }
    {
      const std::scoped_lock locker(depMutex);
      doneSeqs.insert(quitSeq);
    }

    flushUpTo.store(std::max(flushUpTo.load(), quitSeq));
    finalSeq.store(quitSeq);
  } else {
    flushUpTo.store(std::max(flushUpTo.load(), seq));
    finalSeq.store(seq);
  }
  depCv.notify_all();
  if (printerThread.joinable()) {
    printerThread.join();
  }
}
}  // namespace

auto QueryScheduler::execute(std::istream &istream) const -> int {
  ConcurrentQueue<Task> queue;
  std::mutex coutMutex;
  std::mutex bufferMutex;
  std::mutex depMutex;
  std::map<Seq, std::string> resultBuffer;
  std::atomic<Seq> flushUpTo{0};
  std::atomic<Seq> finalSeq{0};
  std::condition_variable depCv;
  std::unordered_set<Seq> doneSeqs;

  std::thread printerThread =
      startPrinterThread(bufferMutex, resultBuffer, flushUpTo, finalSeq,
                         depMutex, depCv, doneSeqs);

  auto workerFunc = makeWorkerFunc(coutMutex, resultBuffer, depMutex, depCv,
                                   doneSeqs, bufferMutex);
  WorkerPool pool(nthreads_, queue, workerFunc);

  try {
    auto [quitQuery, quitSeq, seq] =
        parseAndScheduleQueries(istream, queue, depCv, flushUpTo);

    finalizeExecution(quitQuery, quitSeq, seq, bufferMutex, resultBuffer,
                      depMutex, depCv, flushUpTo, finalSeq, printerThread, pool,
                      doneSeqs);
  } catch (const std::exception &e) {
    const std::scoped_lock locker(coutMutex);
    std::cerr << "Scheduler exception: " << e.what() << "\n";
  }

  return 0;
}
