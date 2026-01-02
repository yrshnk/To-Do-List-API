FROM ubuntu:22.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential cmake git wget pkg-config \
    libsqlite3-dev libhiredis-dev

WORKDIR /app

COPY . /app

RUN mkdir build && cd build && cmake .. && cmake --build .

EXPOSE 8080

CMD ["./build/todo_server"]
