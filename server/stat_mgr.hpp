#pragma once

#include <thread>

namespace server {
class StatsManager {
 public:
  StatsManager() : threadId_(std::this_thread::get_id()) { Reset(); }
  void IncrementBytesReceived(size_t bytes) {
    IncrementInternal(bytesReceived_, bytes);
  }
  void IncrementBytesSent(size_t bytes) {
    IncrementInternal(bytesSent_, bytes);
  }
  void IncrementPayloadReceived(size_t bytes) {
    IncrementInternal(payloadReceived_, bytes);
  }
  void IncrementPayloadSent(size_t bytes) {
    IncrementInternal(payloadSent_, bytes);
  }
  short CalculateCompressionRatio() {
    return payloadReceived_
               ? 100 * ((double)(payloadReceived_ - payloadSent_) /
                        payloadReceived_)
               : 0;
  }
  void Reset() {
    if (std::this_thread::get_id() != threadId_) {
      throw std::runtime_error(
          "Method should be run on thread it have been created");
    }
    bytesReceived_ = bytesSent_ = payloadReceived_ = payloadSent_ = 0;
  };
  uint32_t BytesReceived() { return bytesReceived_; }
  uint32_t BytesSent() { return bytesSent_; }

 private:
  void IncrementInternal(uint32_t &value, size_t amount) {
    if (std::this_thread::get_id() != threadId_) {
      throw std::runtime_error(
          "Method should be run on thread it have been created");
    }
    value += amount;
  }
  uint32_t bytesReceived_;
  uint32_t bytesSent_;
  uint32_t payloadReceived_;
  uint32_t payloadSent_;
  std::thread::id threadId_;
};
}
