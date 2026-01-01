# libiperf C++ Wrapper (Client–Server)

## Overview

This project is a C++ wrapper around the libiperf library, demonstrating how to run an iperf server and client using modern C++ practices such as RAII, `std::thread`, and `std::atomic`.

It was built as a learning project to understand:

* libiperf API usage
* Client–server networking
* Safe resource management

---

## Features

* C++ wrapper classes for iperf server and client
* Server runs in a dedicated thread
* JSON output capture from client
* RAII-based resource management
* Thread-safe design

---

## Project Structure

```
.
├── include/
│   ├── common.h
│   ├── server.h
│   └── client.h
├── src/
│   ├── main.cc
│   ├── server.cc
│   └── client.cc
```

---

## Build

Requires libiperf and a C++17-compatible compiler.

```bash
g++ -std=c++17 -Iinclude src/*.cc -o iperf_wrapper -liperf -pthread
```

---

## Run

```bash
./iperf_wrapper
```

The client runs an iperf test and writes JSON output to:

```
output.txt
```

---

## Notes

* This project is intended for learning, not production use.

---