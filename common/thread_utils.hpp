#pragma once

#include <chrono>
#include <iostream>
#include <atomic>
#include <thread>
//#include <unistd.h>
//#include <pthread.h>

#include <windows.h>


namespace Common {
  /// Set affinity for current thread to be pinned to the provided core_id.
  //inline auto setThreadCore(int core_id) noexcept {
    //return setThreadAffinity(t, 1); 
  //}

  /// Creates a thread instance, sets affinity on it, assigns it a name and
  /// passes the function to be run on that thread as well as the arguments to the function.
  template<typename T, typename... A>
  inline auto createAndStartThread(const std::string &name, T &&func, A &&... args) noexcept {
    std::atomic<bool> running(false), failed(false);
    auto t = new std::thread([&]() {
      std::forward<T>(func)((std::forward<A>(args))...);
    });

    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(1ms);

    return t;
  }
}