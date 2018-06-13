#pragma once

#include <concurrentqueue.h>
#include <spdlog/spdlog.h>
#include <boost/thread/thread.hpp>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include "server/utils.hpp"

namespace server {

typedef std::function<void()> Job;
typedef std::unique_ptr<std::thread> ThreadUPtr;

class WorkerThread {
 public:
  WorkerThread() : stopped(false), drain(false) {
    workerThread = utils::make_unique<std::thread>(([this]() {
      auto logger = spdlog::get("console");
      logger->info("Thread started");
      while (!stopped) {
        if (q.size_approx() == 0) {
          logger->info("Sleeping");
          std::unique_lock<std::mutex> lk(m);
          awake_cond.wait(lk);
          logger->info("Time to wake up");
        }
        Job nextJob;
        if (q.try_dequeue(nextJob)) {
          nextJob();
        }
      }
      if (drain) {
        Job nextJob;
        while (q.try_dequeue(nextJob)) {
          nextJob();
        }
      }
    }));
  }

  void AddJob(Job job) {
    q.enqueue(job);
    if (q.size_approx() == 1) {
      awake_cond.notify_one();
    }
  }

  void Stop(bool do_drain = false) {
    stopped = true;
    drain = do_drain;
    awake_cond.notify_one();
    workerThread->join();
  }

 private:
  ThreadUPtr workerThread;
  moodycamel::ConcurrentQueue<Job> q;
  bool stopped;
  bool drain;
  std::mutex m;
  std::condition_variable awake_cond;
};
}
