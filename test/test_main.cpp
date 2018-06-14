#define CATCH_CONFIG_RUNNER

#include <spdlog/spdlog.h>
#include <catch.hpp>
#include <server/compressor.hpp>

int main(int argc, char *argv[]) {
  spdlog::stdout_color_mt("console");
  return Catch::Session().run(argc, argv);
}

std::string TestFullCorrectString(const std::string &input) {
  std::string output;
  server::Compressor cmp(input.size());
  auto &buf = cmp.Buffer();
  buf = input;
  REQUIRE(cmp.Compress() == server::Compressor::Ok);
  return buf;
}

void CheckString(const std::string &in, const std::string &expected) {
  auto out = TestFullCorrectString(in);
  REQUIRE(out == expected);
}

TEST_CASE("Simple test case", "[simple]") { REQUIRE(1 == 1); }

TEST_CASE("CompressorTest", "[cmpr]") {
  CheckString("aaabbcc", "3abbcc");
  CheckString("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqlllllllllllllloooooo",
              "31q14l6o");
  CheckString("a", "a");
  CheckString("aa", "aa");
  CheckString("aaa", "3a");
  CheckString("aaaaabbb", "5a3b");
  CheckString("aaaaabbbbbbaaabb", "5a6b3abb");
  CheckString("abcdefg", "abcdefg");
  CheckString("aaaccddddhhhhi", "3acc4d4hi");
  CheckString(
      "kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk"
      "kkkkkkkkkkkkkkkkkkkkkkkka",
      "96ka");
  CheckString("", "");
}
