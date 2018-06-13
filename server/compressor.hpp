#pragma once

#include <memory>
#include <vector>

namespace server {

typedef enum { Ok, UnexpectedChar } CompressorError;

typedef std::shared_ptr<std::string> StringPtr;

class Compressor {
 public:
  Compressor() : currentChar_(0), currentCharCount_(0) {}
  CompressorError Compress(StringPtr buffer);
  const std::string &Finish();

 private:
  void DumpChar();
  std::string outBuffer_;
  char currentChar_;
  int currentCharCount_;
};
}
