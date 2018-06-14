#include <spdlog/spdlog.h>
#include <catch.hpp>
#include "server/stat_mgr.hpp"
#include "server/worker_thread.hpp"

TEST_CASE("StatManager", "") {
  auto logger = spdlog::get("console");
  server::StatsManager mgr;
  mgr.IncrementPayloadReceived(100);
  mgr.IncrementPayloadSent(98);
  REQUIRE(mgr.CalculateCompressionRatio() == 2);
  mgr.Reset();
  REQUIRE(mgr.CalculateCompressionRatio() == 0);
  mgr.IncrementPayloadReceived(std::numeric_limits<uint32_t>::max());
  mgr.IncrementPayloadSent(std::numeric_limits<uint32_t>::max() - 1);
  // Very pure comprassion ratio, close to 0
  REQUIRE(mgr.CalculateCompressionRatio() == 0);
  mgr.Reset();
  mgr.IncrementPayloadReceived(std::numeric_limits<uint32_t>::max());
  mgr.IncrementPayloadSent(1);
  // Super effective comperssion ratio
  REQUIRE(mgr.CalculateCompressionRatio() == 99);
  mgr.Reset();
  mgr.IncrementPayloadReceived(100);
  mgr.IncrementPayloadSent(100);
  // This is still not the worst compression algorythm.
  REQUIRE(mgr.CalculateCompressionRatio() == 0);
}

TEST_CASE("StatManagerPerfectCompression", "[!mayfail]") {
  server::StatsManager mgr;
  mgr.IncrementPayloadReceived(100);
  mgr.IncrementPayloadSent(100);
  REQUIRE(mgr.CalculateCompressionRatio() == 100);
}

TEST_CASE("StatManagerOtherThread", "") {
  server::StatsManager mgr;
  server::WorkerThread thr;
  bool exceptionOccured = false;
  thr.AddJob([&]() {
    try {
      mgr.IncrementPayloadReceived(1);
    } catch (...) {
      exceptionOccured = true;
    }
  });
  thr.Stop(true /*drain*/);
  REQUIRE(exceptionOccured);
}
