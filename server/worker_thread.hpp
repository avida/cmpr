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
  WorkerThread() : stopped_(false), drain_(false) {
    workerThread_ = utils::make_unique<std::thread>(([this]() {
      auto logger = spdlog::get("console");
      logger->info("Thread started");
      while (!stopped_) {
        if (jobQueue_.size_approx() == 0) {
          logger->info("Sleeping");
          std::unique_lock<std::mutex> lk(m_);
          // awakeCond_.wait(lk, [this]() ->bool{ return
          // this->jobQueue_.size_approx() != 0 && !this->stopped_;});
          awakeCond_.wait(lk);
          logger->info("Time to wake up");
        }
        Job nextJob;
        if (jobQueue_.try_dequeue(nextJob)) {
          nextJob();
        }
      }
      if (drain_) {
        Job nextJob;
        while (jobQueue_.try_dequeue(nextJob)) {
          nextJob();
        }
      }
    }));
  }

  void AddJob(Job job) {
    jobQueue_.enqueue(job);
    if (jobQueue_.size_approx() == 1) {
      awakeCond_.notify_one();
    }
  }

  void Stop(bool drain = false) {
    stopped_ = true;
    drain_ = drain;
    awakeCond_.notify_one();
    workerThread_->join();
  }

 private:
  ThreadUPtr workerThread_;
  moodycamel::ConcurrentQueue<Job> jobQueue_;
  bool stopped_;
  bool drain_;
  std::mutex m_;
  std::condition_variable awakeCond_;
};
}
