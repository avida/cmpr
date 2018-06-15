# Small compression service
## Description
This is a application for compressing incoming data using simple prefix encoding compression scheme.
All character sequences with lenght more than 2 are replaced with string denoting the sequnce range and character itself (e.g. aaaaa would be 5a).
Uncompressed data should have be more than 32KiB of length and contain only lowercase ASCII characters (a-z). Application is able to serve multiple simultaneous connections.
All the compressing are executed on background thread without affecting on service response time.

## Environment
Application was tested on latest debian 10, codename buster, but it should run on 9 as well. Compiled with gcc 6.3.0 and python 2.7 for build system and test client.

## How to run
To build application from sources use attached scripts:
> ./build.sh && ./run.sh

if you want build script to install required packages use:
> ./build.sh deploy

It shoud be run on system with apt package manager and by the user.
If you have troubles building source on your maching Ive created a docker file for you:
> docker build . -t cmpr

> docker run cmpr -p 0.0.0.0:4000:4000

## Code structure

- *server/server.cpp*: 
Entry point for application. It initializes all data and sets up listening port, construct and run io_service. Code for accepting new connection and initiating reading request headers.
- *server/session.{hpp,cpp}*: 
Contains class for reading and processing requests, sending responses and initiating background data compressing.
- *server/worker_thred.hpp*:
Contains class for executing job on background thread. It has lockfree queue for storing incoming jobs. When background thread runs out of task it would fall into a sleep untill new task arrive.
- *server/compressor.{hpp,cpp}*:
Containg class for storing data buffer and executing compressing algorithm. Assuming that compressed data would be less or equal to the uncompressed it performs all the work in place i.e. reusing same buffer for output data.
- *server/stat_mgr.hpp*:
Contains class for accumulating data that have been sent/received and calculate compression ratio. To get compression ratio I used formula CompressionRatio = (PayloadSent / PayloadReceived) * 100%
So it required  additional variables to store payload without request/ response headers. I don't like mutexes so all the manipulation should be performed on the same thread. Otherwise std::runtime_exception
would be thrown.
- *client/tcp_client.py*:
Simple test client written in Python.
- *test/*.cpp*:
Set of unit tests for most of basic classes.

## 3rdParty libraries
* Catch2 - Modern c++ headers only unittest library. Supports test tags, allows to specify single case or case set.
* spdlog - high performance and tunable log library. Supports log level, multiple log sinks (with separate log level for each target), Able to do write log to
     syslog, file and to the console with support of terminal color schemes. Beats boost::log on benchmarks.
* CLI11 -  Lightweight command line parsing tool. Doesnt require additional variables.
     concurrentqueue - Lockfree queue implementation. Very fast, slightly better than std::queue + mutexes according to the benchmarks its author have provided.

## Assumptions
I haven't implemented full automative tests for this application cause it would take a lot of time and it not a part of this task.
But I assume this application to have a good performance due to its async multithread design. Anyway there is a some room for improvements (see next section).
Application supports streams up to 32KiB and, cause of API desing, all data should be stored in memory for sending correct reponse erorr.
So it assumes that system should have at least 32 KiB of heap memory available and 4 KiB for additional variable resulting into 36 KiB.

## Improvements

- First of all it should considered to redesign API not to send response with payload length and error code in the header.
Instead it should be some stream based format like the http chunked encoding or null terminated stream with payload lenght/crc for verifying data at the end.
It would allow not to read all the data and store it in memory before proceeding to compressing but perform all the job on the fly.

- Second improvement is to perform compressing on several threads for taking an advantage of multiprocessor system which is most of the modern hardware.
For current implementation there is already one background thread performing all the calculations. Adding more threads likely wouldn't show some performance improvements
because for this simple compressing algorithm all the data are processed much faster than it being read out from the network. But for some more efficient and cpu heavy
algorithms adding some thread pool should increase performance unless its not exceed number of physicall processors/threads.
- Cache line allignment. Beside the network bandwidth weak point of this algo is the RAM access time. To fully utilize the L3 cache caps we need to make sure nothing would perform data storing command on the
same cache line leading to cache miss and invalidating the cache. So we have to align all data buffers on the cache  line beginning and allocate memory of size multiple to the size of cache line.
