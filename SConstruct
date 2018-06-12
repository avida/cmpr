srv_files = [File("server/compressor.cpp")]

glob_env = Environment(CPPPATH=[Dir("3rdParty/Catch2/single_include"),
                                Dir("3rdParty/spdlog/include"),
                                Dir(".")],
                       CCFLAGS="-std=c++0x",
                       LIBS="pthread")
Export ("glob_env", "srv_files")

SConscript("server/SConscript")
SConscript("test/SConscript")
