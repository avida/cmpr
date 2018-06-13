#pragma once

#include <memory>
#include <vector>

namespace server {

typedef enum { Ok, UnexpectedChar } CompressorError;

typedef std::shared_ptr<std::string> StringPtr;

class Compressor {
 public:
  Compressor(size_t bufferSize) { buffer_.resize(bufferSize); }
  CompressorError Compress();
  std::string &Buffer() { return buffer_; }

 private:
  std::string buffer_;
};
}
