**UDP Chat Application (C)**
**Overview**

This project is a simple UDP-based chat application written in C.
It consists of a server and multiple clients that can join a chat room, send messages, and leave gracefully.

The server maintains a list of connected clients and broadcasts messages to all active clients.

**Features**

1. UDP-based client–server communication

2. Multiple client support

3. Client registration using JOIN <name>

4. Message broadcasting using MSG <text>

5. Graceful client exit using LEAVE

6. Server-side client tracking using IP + port

7. Uses select() for non-blocking I/O

**Message Protocol**

The client communicates with the server using simple text commands:

JOIN <name> – Register a client

MSG <message> – Send a chat message

LEAVE – Leave the chat

**Build**

Compile using gcc:

gcc server.c -o server
gcc client.c -o client

**Run**

Start the server first:

./server


Start one or more clients in separate terminals:

./client

**Usage**

Client is prompted to enter a name

Client joins the chat automatically

Type messages and press Enter to send

Type quit to leave the chat

**Notes**

This project uses UDP, so delivery is not guaranteed.

Client identity is tracked using (IP address + port).

Designed as a learning project and will be extended step by step.
