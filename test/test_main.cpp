#define CATCH_CONFIG_RUNNER


#include <catch.hpp>
#include <server/compressor.hpp>
#include <spdlog/spdlog.h>

int main( int argc, char* argv[] ) {
   spdlog::stdout_color_mt("console");
   return Catch::Session().run( argc, argv );
}

std::string TestFullCorrectString(const std::string &input) {
  std::string output;
  server::Compressor cmp([&output](const std::string &vec, server::CompressorError err){
    REQUIRE(err != server::UnexpectedChar);
    output += vec;
  });
  auto buf = std::make_shared<std::string>();
  *buf = input;
  cmp.Compress(buf);
  cmp.Finish();
  return output;
}

void CheckString(const std::string &in, const std::string &expected) {
  auto out = TestFullCorrectString(in);
  REQUIRE(out==expected);
}

TEST_CASE("Simple test case", "[simple]") {
  REQUIRE(1==1);
}

TEST_CASE("Compressor test", "[cmpr]") {
  CheckString("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqlllllllllllllloooooo", "31q14l6o");
  CheckString("a", "a");
  CheckString("aa", "aa");
  CheckString("aaa", "3a");
  CheckString("aaaaabbb", "5a3b");
  CheckString("aaaaabbbbbbaaabb", "5a6b3abb");
  CheckString("abcdefg", "abcdefg");
  CheckString("aaaccddddhhhhi", "3acc4d4hi");
  CheckString("kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkka", "96ka");
  CheckString("", "");

}

