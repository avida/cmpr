#include <iostream>

#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>
#include <boost/asio.hpp>

#include "session.hpp"
#include "worker_thread.hpp"

namespace asio = boost::asio;
using boost::asio::ip::tcp;
using namespace std::placeholders;
using namespace server;

void acceptConnection(const boost::system::error_code &error,
                      tcp::acceptor &acceptor, asio::io_service &io_service,
                      SessionPtr session, WorkerThread &wrk) {
  auto console = spdlog::get("console");
  console->info("connection accepted");
  session->ReadHeader();
  auto newSession = std::make_shared<Session>(io_service, wrk);
  acceptor.async_accept(
      newSession->socket(),
      std::bind(&acceptConnection, std::placeholders::_1, std::ref(acceptor),
                std::ref(io_service), newSession, std::ref(wrk)));
}

int main(int argc, char **argv) {
  auto port = 4000;
  CLI::App app{"App description"};
  app.add_option("-p, --port", port, "A listening port");
  CLI11_PARSE(app, argc, argv);

  auto console = spdlog::stdout_color_mt("console");
  console->info("Hi");
  console->info("Port: {0}", port);
  asio::io_service io_service;
  WorkerThread wrk;
  try {
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
    auto session = std::make_shared<Session>(io_service, wrk);
    acceptor.async_accept(
        session->socket(),
        std::bind(&acceptConnection, std::placeholders::_1, std::ref(acceptor),
                  std::ref(io_service), session, std::ref(wrk)));
    session.reset();
    io_service.run();
  } catch (...) {
    console->error("Enexpected error");
    return 1;
  }
  console->error("BB");
  return 0;
}
