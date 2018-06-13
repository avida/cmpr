#pragma once

#include <boost/asio.hpp>
#include "server/utils.hpp"

using boost::asio::ip::tcp;

namespace server {
class Session {
 public:
  Session(boost::asio::io_service &io_service) : sock(io_service) {}
  tcp::socket &socket() { return sock; }

 private:
  tcp::socket sock;
};
typedef std::shared_ptr<Session> SessionPtr;
}
