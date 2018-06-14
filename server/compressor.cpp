#include "compressor.hpp"

#include <spdlog/spdlog.h>
#include <sstream>

namespace server {

Compressor::CompressorError Compressor::Compress() {
  auto logger = spdlog::get("console");
  auto unCompressedPos = 0;
  auto compressedPos = 0;
  while (unCompressedPos < buffer_.size()) {
    auto currentChar = buffer_[unCompressedPos];
    if (currentChar < 'a' || currentChar > 'z') {
      logger->error("Wrong char {0}", currentChar);
      return UnexpectedChar;
    }
    auto nextPos = buffer_.find_first_not_of(currentChar, unCompressedPos);
    auto consequtiveChars = nextPos == std::string::npos
                                ? (buffer_.size() - unCompressedPos)
                                : (nextPos - unCompressedPos);
    if (consequtiveChars > 2) {
      auto compressedString = std::to_string(consequtiveChars) + currentChar;
      std::memcpy(&buffer_[compressedPos], &compressedString[0],
                  compressedString.size());
      compressedPos += compressedString.size();
    } else {
      std::memcpy(&buffer_[compressedPos], &buffer_[unCompressedPos],
                  consequtiveChars);
      compressedPos += consequtiveChars;
    }
    unCompressedPos += consequtiveChars;
  }
  buffer_.resize(compressedPos);

  return Ok;
}
}
