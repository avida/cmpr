#include "compressor.hpp"

#include <spdlog/spdlog.h>
#include <sstream>

namespace server {

void Compressor::DumpChar() {
  switch (currentCharCount) {
    case 0:
      break;
    case 2:
      out += currentChar;
    case 1:
      out += currentChar;
      break;
    default:
      auto append = std::to_string(currentCharCount) + currentChar;
      out += append;
  }
}

void Compressor::Compress(StringPtr buffer){
  auto logger = spdlog::get("console");
  std::string vec;
  for (auto it = buffer->cbegin(); it != buffer->end(); it++) {
    if (*it < 'a' || *it >'z'){
      logger->error("Wrong char {0}", *it);
      cb("", UnexpectedChar);
      return;
    }
    if (currentChar == 0) {
      currentChar = *it;
      currentCharCount = 1;
      continue;
    }
    if (*it == currentChar){
      currentCharCount++;
    } else {
      DumpChar();
      currentChar = *it;
      currentCharCount = 1;
    }
  }
  cb(out, Ok);
  out.clear();
}

void Compressor::Finish(){
  DumpChar();
  cb(out, Finished);
  out.clear();
}
}
