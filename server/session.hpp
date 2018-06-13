#pragma once

#include <boost/asio.hpp>
#include "server/utils.hpp"

namespace asio = boost::asio;
using boost::asio::ip::tcp;

const uint32_t BufferSize = 10;

namespace server {

const int kMagicNumber = 0x53545259;

typedef enum { Ping = 1, GetStats, ResetStats, Compress } PacketTypes;

typedef enum {
  Ok,
  UnknownError,
  MessageTooLarge,
  UnsupportedRequestType,
  AddCustomStatusBelowThisLine = 33
} ResponceStatus;

class Session : public std::enable_shared_from_this<Session> {
 public:
  Session(asio::io_service &io_service)
      : sock_(io_service), buffer_(BufferSize, 0) {}
  tcp::socket &socket() { return sock_; }
  void Read();
  void Write(const std::string &buf);

  ~Session();

 private:
  tcp::socket sock_;
  std::string buffer_;
  std::string outBuffer_;
};
typedef std::shared_ptr<Session> SessionPtr;
}
