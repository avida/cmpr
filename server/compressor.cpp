#include "compressor.hpp"

#include <spdlog/spdlog.h>
#include <sstream>

namespace server {

void Compressor::DumpChar() {
  switch (currentCharCount_) {
    case 0:
      break;
    case 2:
      outBuffer_ += currentChar_;
    case 1:
      outBuffer_ += currentChar_;
      break;
    default:
      auto append = std::to_string(currentCharCount_) + currentChar_;
      outBuffer_ += append;
  }
}

CompressorError Compressor::Compress(StringPtr buffer) {
  auto logger = spdlog::get("console");
  std::string vec;
  assert(currentChar_ != -1);
  for (auto it = buffer->cbegin(); it != buffer->end(); it++) {
    if (*it < 'a' || *it > 'z') {
      logger->error("Wrong char {0}", *it);
      return UnexpectedChar;
    }
    if (currentChar_ == 0) {
      currentChar_ = *it;
      currentCharCount_ = 1;
      continue;
    }
    if (*it == currentChar_) {
      currentCharCount_++;
    } else {
      DumpChar();
      currentChar_ = *it;
      currentCharCount_ = 1;
    }
  }
  return Ok;
}

const std::string &Compressor::Finish() {
  DumpChar();
  currentChar_ = -1;
  return outBuffer_;
}
}
