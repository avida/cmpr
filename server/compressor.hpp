
#include <memory>
#include <vector>

namespace server {

typedef enum {
  Ok,
  Finished,
  UnexpectedChar
} CompressorError;

typedef std::shared_ptr<std::string> StringPtr;
typedef std::function< void(const std::string&, CompressorError) > CompressedCallback;

class Compressor {
public:
  Compressor(CompressedCallback compressed_cb): cb(compressed_cb),  
                                                currentChar(0),
                                                currentCharCount(0) {}
  void Compress(StringPtr buffer);
  void Finish();
private:
  void DumpChar();
  CompressedCallback cb;
  std::string out;
  char currentChar;
  int currentCharCount;
};
}

