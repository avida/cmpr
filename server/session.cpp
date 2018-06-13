#include "session.hpp"

#include <spdlog/spdlog.h>

namespace server {
void Session::Read() {
  auto thisPtr = shared_from_this();
  asio::async_read(sock_, asio::buffer(&buffer_[0], buffer_.size()),
                   [thisPtr](const boost::system::error_code &ec,
                             std::size_t bytes_transferred) {
                     auto logger = spdlog::get("console");
                     if (ec) {
                       logger->info("error: {0}", ec.message());
                       return;
                     }
                     logger->info("Async read: {0}", bytes_transferred);
                     logger->info("data: {0}", thisPtr->buffer_);
                     if (bytes_transferred > 0) {
                       thisPtr->Read();
                       thisPtr->outBuffer_ = std::string(1000000, 'a');

                       logger->info("sending");

                       thisPtr->Write(thisPtr->outBuffer_);
                     }

                   });
}
void Session::Write(const std::string &buf) {
  auto thisPtr = shared_from_this();
  asio::async_write(sock_, asio::buffer(&buf[0], buf.size()),
                    [thisPtr](const boost::system::error_code &ec,
                              std::size_t bytes_transferred) {
                      auto logger = spdlog::get("console");
                      logger->info("sent {0}", bytes_transferred);
                      if (ec) {
                        logger->info("error sending {0}", ec.message());
                      }

                    });
}

Session::~Session() { spdlog::get("console")->info("~Session"); }
}
