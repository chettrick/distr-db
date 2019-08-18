## FROM amytabb/docker_ubuntu16_essentials
FROM ubuntu:latest
 COPY wl_generator /wl_generator
WORKDIR /wl_generator/
## RUN g++ -std=c++11 -pthread -o NewWorld newworld.cpp
RUN apt-get update -y && apt-get install gcc-8 g++-8 cmake -y && ln -s /usr/bin/g++-8 /usr/bin/g++
RUN rm -rf bin && cmake -H. -Bbin && cmake --build bin
CMD ["bin/wl_generator"]
