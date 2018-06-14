#include "session.hpp"

#include <spdlog/spdlog.h>
#include "utils.hpp"

namespace server {
void Session::ReadHeader() {
  auto thisPtr = shared_from_this();
  auto buffer = asio::buffer(&headerBuffer_.rawData, sizeof(PacketHeader));
  asio::async_read(sock_, buffer, [thisPtr](const boost::system::error_code &ec,
                                            size_t bytesTransferred) {
    auto logger = spdlog::get("console");
    if (ec) {
      logger->info("error: {0}", ec.message());
      return;
    }
    logger->info("Async read: {0}", bytesTransferred);
    thisPtr->headerBuffer_.hdr.ToHost();
    auto &hdr = thisPtr->headerBuffer_.hdr;
    logger->info("magic: {0}, pl: {1}, code: {2}", hdr.magicNumber,
                 hdr.payloadLength, hdr.code);
    if (hdr.magicNumber != kMagicNumber) {
      thisPtr->SendErrorResponse(MagicNumberDontMatch);
      return;
    }
    switch (hdr.code) {
      case ResetStats:
      case Ping:
        thisPtr->SendErrorResponse(Ok);
        break;
      case GetStats:
        thisPtr->SendStats();
        break;
      case Compress:
        if (hdr.payloadLength > kMaxPayload) {
          thisPtr->SendErrorResponse(MessageTooLarge);
          break;
        }
        thisPtr->ReadPayload();
        break;
      default:
        thisPtr->SendErrorResponse(UnsupportedRequestType);
    }
  });
}

void Session::ReadPayload() {
  auto thisPtr = shared_from_this();
  compressor_ = utils::make_unique<Compressor>(headerBuffer_.hdr.payloadLength);
  auto &buf = compressor_->Buffer();

  auto buffer = asio::buffer(&buf[0], buf.size());
  asio::async_read(sock_, buffer, [thisPtr](const boost::system::error_code &ec,
                                            size_t bytesTransferred) {
    auto logger = spdlog::get("console");
    logger->info("payload received, {0}", bytesTransferred);
    thisPtr->worker_.AddJob([thisPtr]() {
      if (auto res = thisPtr->compressor_->Compress() != Compressor::Ok) {
        thisPtr->sock_.get_io_service().post(
            [thisPtr]() { thisPtr->SendErrorResponse(UnexpectedCharactes); });
      } else {
        thisPtr->sock_.get_io_service().post([thisPtr]() {
          thisPtr->SendErrorResponse(Ok, thisPtr->compressor_->Buffer().size());
          thisPtr->SendPayload();
        });
      }
    });
  });
}

void Session::SendStats() {
  auto thisPtr = shared_from_this();
  headerBuffer_.stats.bytesReceived = 20;
  headerBuffer_.stats.bytesSent = 24;
  headerBuffer_.stats.compressRatio = 20;
  headerBuffer_.stats.ToNetwork();
  asio::async_write(
      sock_, asio::buffer(&headerBuffer_.rawData, sizeof(StatsReport)),
      [thisPtr](const boost::system::error_code &ec, size_t bytesTransferred) {
        auto logger = spdlog::get("console");
        if (ec) {
          logger->info("error: {0}", ec.message());
        }
        logger->info("Stats sent {0}", bytesTransferred);
      });
}

void Session::SendErrorResponse(ResponceStatus errorCode, size_t payloadSize) {
  auto thisPtr = shared_from_this();
  auto &header = thisPtr->headerBuffer_.hdr;
  header.code = errorCode;
  header.payloadLength = payloadSize;
  header.ToNetwork();

  asio::async_write(
      sock_, asio::buffer(&headerBuffer_.rawData, sizeof(PacketHeader)),
      [thisPtr](const boost::system::error_code &ec, size_t bytesTransferred) {
        auto logger = spdlog::get("console");
        logger->info("Resp sent {0}", bytesTransferred);
      });
}

void Session::SendPayload() {
  auto thisPtr = shared_from_this();
  const auto &output = compressor_->Buffer();

  asio::async_write(
      sock_, asio::buffer(&output[0], output.size()),
      [thisPtr](const boost::system::error_code &ec, size_t bytesTransferred) {
        auto logger = spdlog::get("console");
        logger->info("sent {0}", bytesTransferred);
        if (ec) {
          logger->info("error sending {0}", ec.message());
        }
      });
}

Session::~Session() { spdlog::get("console")->info("~Session"); }
}
