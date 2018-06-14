#pragma once

#include <boost/asio.hpp>

#include "compressor.hpp"
#include "server/utils.hpp"
#include "stat_mgr.hpp"
#include "worker_thread.hpp"

namespace asio = boost::asio;
using boost::asio::ip::tcp;

const uint32_t BufferSize = 10;

namespace server {

const int kMagicNumber = 0x53545259;
const int kMaxPayload = 200;

struct PacketHeader {
  uint32_t magicNumber;
  uint16_t payloadLength;
  uint16_t code;
  void ToHost() {
    magicNumber = ntohl(magicNumber);
    payloadLength = ntohs(payloadLength);
    code = ntohs(code);
  };
  void ToNetwork() {
    magicNumber = htonl(magicNumber);
    payloadLength = htons(payloadLength);
    code = htons(code);
  }
};

#pragma pack(push, 1)
struct StatsReport {
  uint32_t bytesReceived;
  uint32_t bytesSent;
  char compressRatio;
  void ToNetwork() {
    bytesReceived = htonl(bytesReceived);
    bytesSent = htonl(bytesSent);
  }
};
#pragma pack(pop)

typedef union {
  char *rawData;
  PacketHeader hdr;
  StatsReport stats;
} PacketBuffer;

typedef enum {
  Ping = 1,
  GetStats,
  ResetStats,
  Compress,
  UnexpectedPacketsZone
} PacketType;

typedef enum {
  Ok,
  UnknownError,
  MessageTooLarge,
  UnsupportedRequestType,
  AddCustomStatusBelowThisLine = 33,
  MagicNumberDontMatch,
  UnexpectedCharactes,
} ResponceStatus;

class Session : public std::enable_shared_from_this<Session> {
 public:
  Session(asio::io_service &io_service, WorkerThread &worker,
          StatsManager &stats)
      : sock_(io_service), worker_(worker), stats_(stats) {}
  tcp::socket &socket() { return sock_; }
  void ReadHeader();
  void ReadPayload();
  void SendPayload();
  void SendErrorResponse(ResponceStatus errorCode, size_t payloadSize = 0);
  void SendStats();

  ~Session();

 private:
  tcp::socket sock_;
  PacketBuffer headerBuffer_;
  WorkerThread &worker_;
  CompressorUPtr compressor_;
  StatsManager &stats_;
};
typedef std::shared_ptr<Session> SessionPtr;
}
