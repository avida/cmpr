#define CATCH_CONFIG_RUNNER

#include <spdlog/spdlog.h>
#include <catch.hpp>

int main(int argc, char *argv[]) {
  spdlog::stdout_color_mt("console");
  return Catch::Session().run(argc, argv);
}
