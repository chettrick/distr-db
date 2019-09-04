## FROM amytabb/docker_ubuntu16_essentials
FROM ubuntu:latest
WORKDIR /wl_generator/
RUN apt-get update -y && apt-get install gcc-8 g++-8 cmake -y && ln -s /usr/bin/g++-8 /usr/bin/g++ && apt-get install libsfml-dev -y
# used to install libcurl, used by the workload generator to make http post requests
RUN apt-get install libcurl4-openssl-dev -y
COPY wl_generator /wl_generator
RUN rm -rf bin && cmake -H. -Bbin && cmake --build bin
COPY start.sh /
RUN chmod +x /start.sh
CMD ["/start.sh"]
