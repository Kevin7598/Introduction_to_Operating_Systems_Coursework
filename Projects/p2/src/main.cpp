//
// Created by liu on 18-10-21.
//

#include <getopt.h>  // NOLINT(misc-include-cleaner)
#include <unistd.h>  // NOLINT(misc-include-cleaner)

#include <algorithm>  // for std::max (include-cleaner)
#include <array>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "query/scheduler.h"

struct ParsedArgs {
  std::string listen;
  int64_t threads = 0;  // use fixed-width integer type
};

namespace {  // Use anonymous namespace instead of static (clang-tidy
             // suggestion)
auto parseArgs(int argc, char **argv) -> ParsedArgs {
  constexpr int kDecimalBase = 10;
  const std::array<struct option, 3> longOpts = {
      {{"listen", required_argument, nullptr, 'l'},
       {"threads", required_argument, nullptr, 't'},
       {nullptr, no_argument, nullptr, 0}}};
  const char *shortOpts = "l:t:";
  int opt = 0;
  int longIndex = 0;
  ParsedArgs args{};
  while ((opt = getopt_long(argc, argv, shortOpts, longOpts.data(),
                            &longIndex)) != -1) {
    switch (opt) {
    case 'l':
      args.listen = optarg;
      break;
    case 't':
      args.threads = std::strtoll(optarg, nullptr, kDecimalBase);
      break;
    default:
      std::cerr << "lemondb: warning: unknown argument\n";
      break;
    }
  }
  return args;
}
}  // namespace

// Parsing helper moved to scheduler.cpp

auto main(int argc, char **argv) -> int {
  try {  // Guard main to avoid bugprone-exception-escape
    std::ios_base::sync_with_stdio(false);
    const auto parsedArgs = parseArgs(argc, argv);
    std::ifstream fin;
    if (!parsedArgs.listen.empty()) {
      fin.open(parsedArgs.listen);
      if (!fin.is_open()) {
        std::cerr << "lemondb: error: " << parsedArgs.listen
                  << ": no such file or directory\n";
        return -1;
      }
    }
    std::istream input(fin.rdbuf());

#ifdef NDEBUG
    if (parsedArgs.listen.empty()) {
      std::cerr
          << "lemondb: error: --listen argument not found, not allowed in "
             "production mode\n";
      return -1;
    }
#else
    if (parsedArgs.listen.empty()) {
      std::cerr << "lemondb: warning: --listen argument not found, use stdin "
                   "instead in debug mode\n";
      input.rdbuf(std::cin.rdbuf());
    }
#endif

    const size_t nthreads =
        parsedArgs.threads > 0
            ? static_cast<size_t>(parsedArgs.threads)
            : std::max<size_t>(1, std::thread::hardware_concurrency());

    if (parsedArgs.threads < 0) {
      std::cerr << "lemondb: error: threads num can not be negative value "
                << parsedArgs.threads << "\n";
      return -1;
    }
    if (parsedArgs.threads == 0) {
      std::cerr << "lemondb: info: auto detect thread num:" << nthreads << "\n";
    } else {
      std::cerr << "lemondb: info: running in " << parsedArgs.threads
                << " threads\n";
    }

    const QueryScheduler scheduler(nthreads);
    return scheduler.execute(input);
  } catch (const std::exception &e) {
    std::cerr << "lemondb: fatal: uncaught exception: " << e.what() << "\n";
    return -2;
  } catch (...) {
    std::cerr << "lemondb: fatal: unknown uncaught exception\n";
    return -3;
  }
}
