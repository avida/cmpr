#include <catch.hpp>
#include <spdlog/spdlog.h>
#include <server/worker_thread.hpp>
#include <chrono>
#include <thread>

TEST_CASE("WorkerThreadTest", "[worker]") {
  auto logger = spdlog::get("console");
  logger->info("Test started");
  server::WorkerThread thr;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto someNumber = 0;
  thr.AddJob([&logger, &someNumber](){
    logger->info("Job is being performed");
    someNumber = 13;
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  thr.Stop();
  REQUIRE(someNumber==13);
}

TEST_CASE("WorkerThreadALotOfJobs", "[worker]") {
  server::WorkerThread thr;
  const auto targetNumber = 10000042;
  auto someNumber = 0;
  std::thread jobsAdderThread([&thr, &someNumber, &targetNumber](){
    auto logger = spdlog::get("console");
    for (auto i = 0; i< targetNumber; i++){
      thr.AddJob([&someNumber](){
        someNumber++;
      });
    }
  });
  jobsAdderThread.join();
  thr.Stop(true/*drain qob queue*/);
  REQUIRE(someNumber == targetNumber);
}

TEST_CASE("WorkerThreadALotOfSleep", "[worker]") {
  server::WorkerThread thr;
  const auto targetNumber = 500;
  auto someNumber = 0;
  std::thread jobsAdderThread([&thr, &someNumber, &targetNumber](){
    auto logger = spdlog::get("console");
    for (auto i = 0; i< targetNumber; i++){
      thr.AddJob([&someNumber](){
        someNumber++;
      });
      // Give some time for worker thread to go in sleep.
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });
  jobsAdderThread.join();
  thr.Stop(true/*drain qob queue*/);
  REQUIRE(someNumber == targetNumber);
}
