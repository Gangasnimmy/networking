# TCP File Transfer (Client–Server in C)

A simple POSIX-compliant TCP client–server application written in C that transfers a file from a client to a server over a TCP connection.

This project demonstrates low-level networking fundamentals, including sockets, file I/O, and reliable data transfer using TCP streams.

## 📌 Project Name

**tcp-file-transfer-c**

## 📖 Overview

The client reads a local file in fixed-size chunks and sends it to the server using a TCP socket.
The server receives the incoming byte stream and writes the data to an output file.

Key characteristics:

* Written purely in C (POSIX APIs)
* Uses blocking TCP sockets
* Handles partial reads and writes correctly
* Works even when file size is unknown in advance

---

## 🛠️ Technologies Used

* Language: C
* OS APIs: POSIX
* Networking: TCP sockets
* File I/O: `open`, `read`, `write`, `close`

---

## 📂 Project Structure

```
.
├── client.c        # TCP client (sends file)
├── server.c        # TCP server (receives file)
├── data.txt        # Sample input file (client side)
├── received.txt    # Output file (server side)
└── README.md
```

---

## ⚙️ Build Instructions

Compile using `gcc` with warnings enabled:

### Client

```bash
gcc -Wall -Wextra -std=c11 client.c -o client
```

### Server

```bash
gcc -Wall -Wextra -std=c11 server.c -o server
```

## ▶️ How to Run

### 1️⃣ Start the server

```bash
./server
```

The server listens on port **8000** and waits for incoming connections.

### 2️⃣ Run the client

```bash
./client
```

The client connects to `127.0.0.1:8000` and sends the contents of `data.txt`.

### 3️⃣ Result

* The server writes received data into `received.txt`
* File contents are preserved exactly

---

## 🧠 Implementation Highlights

### Client Side

* Reads file using `read()` in 1024-byte chunks
* Handles **partial TCP sends** using a send loop
* Uses `shutdown(socket, SHUT_WR)` to signal end-of-file

### Server Side

* Uses `recv()` in a loop
* Detects end-of-data when `recv()` returns `0`
* Writes received bytes directly to file

---

## 🚫 Common Pitfalls Handled

* Partial `send()` and `recv()` handling
* Proper `sockaddr_in` initialization
* Correct use of network byte order
* Clean resource cleanup (`close()`)

---

## 🎯 Learning Outcomes

* Understanding TCP as a **byte-stream protocol**
* Practical usage of POSIX file APIs
* Client–server communication fundamentals
* Debugging common socket programming issues

---

## 📌 Notes

* This project uses **localhost (127.0.0.1)** by default
* No encryption (TLS) is used
* Designed for educational purposes

---

## 🧑‍💻 Author

Gangadevi

---

## 📄 License

This project is open for learning and personal use.
