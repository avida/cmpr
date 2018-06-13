from plumbum import local, ProcessExecutionError
import sys

srv_main = File("server/compress.cpp")
srv_files = [Glob("server/*.cpp", exclude=str(srv_main))]
test_files = [Glob("test/*pp")]

if "clang" in sys.argv:
  clang_cmd = local["clang-format"]
  clang_args = "-i style=google"
  try:
     clang_cmd()
  except ProcessExecutionError:
    print("Cannot find clang-format cmd")
    sys.exit(1)

  for file in Glob("*/*pp"):
    clang_cmd[clang_args](file)
  sys.exit(0)

glob_env = Environment(CPPPATH=[Dir("3rdParty/Catch2/single_include"),
                                Dir("3rdParty/spdlog/include"),
                                Dir("3rdParty/CLI11/include"),
                                Dir("3rdParty/concurrentqueue"),
                                Dir(".")],
                       CCFLAGS="-std=c++0x",
                       LIBS=["pthread", "boost_system"])
Export ("glob_env", "srv_files", "srv_main", "test_files")

SConscript("server/SConscript")
SConscript("test/SConscript")
