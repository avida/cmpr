#include <iostream>
#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using boost::asio::ip::tcp;
using namespace std::placeholders;


typedef std::shared_ptr<tcp::socket> SocketPtr;

void acceptConnection(const boost::system::error_code &error,
                      tcp::acceptor &acceptor, asio::io_service &io_service, 
                      SocketPtr socket){
  auto console = spdlog::get("console");
  console->info("connection accepted");
  auto new_socket = std::make_shared<tcp::socket>(io_service);
  acceptor.async_accept(*new_socket, std::bind(&acceptConnection, _1,
                        std::ref(acceptor), std::ref(io_service),
                        socket));
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
  auto socket = std::make_shared<tcp::socket>(io_service);
  acceptor.async_accept(*socket, std::bind(&acceptConnection, _1,
                        std::ref(acceptor), std::ref(io_service),
                        socket) );
  while(true) {
    io_service.run();
  }
  console->error("BB");

}
