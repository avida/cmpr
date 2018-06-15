FROM debian:9
ADD . /cmpl
RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends apt-utils
RUN apt-get install -y sudo
WORKDIR /cmpl
RUN ./build.sh
ENTRYPOINT ./run.sh
