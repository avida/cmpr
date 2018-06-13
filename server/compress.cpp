#include <iostream>

#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>
#include <boost/asio.hpp>

#include "session.hpp"

namespace asio = boost::asio;
using boost::asio::ip::tcp;
using namespace std::placeholders;
using namespace server;


void acceptConnection(const boost::system::error_code &error,
                      tcp::acceptor &acceptor, asio::io_service &io_service, 
                      SessionPtr session){
  auto console = spdlog::get("console");
  console->info("connection accepted");
  auto new_session = std::make_shared<Session>(io_service);
  acceptor.async_accept(new_session->socket(), std::bind(&acceptConnection, _1,
                        std::ref(acceptor), std::ref(io_service),
                        new_session));
}

int main (int argc, char** argv){
  auto port = 4000;
  CLI::App app{"App description"};
  app.add_option("-p, --port", port, "A listening port");
  CLI11_PARSE(app, argc, argv);

  auto console = spdlog::stdout_color_mt("console");
  console->info("Hi");
  console->info("Port: {0}", port);
  asio::io_service io_service;
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
  auto session = std::make_shared<Session>(io_service);
  acceptor.async_accept(session->socket(), std::bind(&acceptConnection, _1,
                        std::ref(acceptor), std::ref(io_service),
                        session) );
  while(true) {
    io_service.run();
  }
  console->error("BB");

}
