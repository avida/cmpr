#pragma once

#include <memory>
#include <vector>

namespace server {

typedef std::shared_ptr<std::string> StringPtr;

class Compressor {
 public:
  typedef enum { Ok, UnexpectedChar } CompressorError;
  Compressor(size_t bufferSize) { buffer_.resize(bufferSize); }
  CompressorError Compress();
  std::string &Buffer() { return buffer_; }

 private:
  std::string buffer_;
};
typedef std::unique_ptr<Compressor> CompressorUPtr;
}
