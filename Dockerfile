FROM debian:9
ADD . /cmpl
EXPOSE 4000
RUN apt-get update
RUN apt-get install -y apt-utils
RUN apt-get install -y sudo
WORKDIR /cmpl
RUN ./build.sh deploy
ENTRYPOINT ./run.sh
