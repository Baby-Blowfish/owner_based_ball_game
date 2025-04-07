# Ball Game

A client-server based interactive ball game that allows multiple clients to connect to a server and manipulate balls on a shared display.

## Technology Stack

### Programming Language

- **C**: Core implementation language
- **C99 Standard**: Modern C features and syntax

### System Programming

- **Linux System Calls**: For low-level system operations
- **POSIX Threads (pthread)**: For multi-threading support
- **Socket Programming**: For network communication
- **Framebuffer API**: For direct display manipulation

### Network Communication

- **TCP/IP**: For reliable client-server communication
- **Socket API**: For network socket operations
- **Select I/O Multiplexing**: For handling multiple client connections

### Build Tools

- **GCC**: GNU Compiler Collection
- **Make**: Build automation tool
- **GDB**: Debugging tool

### Development Tools

- **Doxygen**: Documentation generation
- **Git**: Version control system
- **Valgrind**: Memory leak detection and profiling

### Libraries

- **Standard C Library**: For basic I/O and data manipulation
- **POSIX Library**: For system-level operations
- **Linux Framebuffer Library**: For display operations

### Operating System

- **Linux**: Primary development and deployment platform
- **Kernel Version**: 5.15.0 or higher recommended

## System Architecture

### High-Level Architecture

```mermaid
graph TD
    subgraph "Server"
        S[Server Process]
        CLM[Client List Manager]
        LBM[Local Ball Manager]
        TQ[Task Queue]
        S --> CLM
        S --> LBM
        S --> TQ
    end

    subgraph "Client 1"
        C1[Client Process]
        SBM1[Screen Ball Manager]
        SBL1[Screen Ball List]
        C1 --> SBM1
        SBM1 --> SBL1
    end

    subgraph "Client 2"
        C2[Client Process]
        SBM2[Screen Ball Manager]
        SBL2[Screen Ball List]
        C2 --> SBM2
        SBM2 --> SBL2
    end

    subgraph "Client N"
        CN[Client Process]
        SBMN[Screen Ball Manager]
        SBLN[Screen Ball List]
        CN --> SBMN
        SBMN --> SBLN
    end

    S <-->|TCP/IP| C1
    S <-->|TCP/IP| C2
    S <-->|TCP/IP| CN
```

### Component Interaction

```mermaid
sequenceDiagram
    participant Client
    participant Server
    participant TaskQueue
    participant LocalBallManager
    participant ClientListManager

    Client->>Server: Connect
    Server->>ClientListManager: Register client
    Server->>Client: Send initial game state

    Client->>Server: Send command (add/delete ball)
    Server->>TaskQueue: Add task
    TaskQueue->>LocalBallManager: Process command
    LocalBallManager->>LocalBallManager: Update game state
    LocalBallManager->>ClientListManager: Notify state change
    ClientListManager->>Client: Broadcast updated state
    Client->>Client: Update display
```

### Thread Model

```mermaid
graph TD
    subgraph "Client Threads"
        MT[Main Thread]
        RDT[Render Thread]
        SRT[Socket Receive Thread]
        SST[Socket Send Thread]

        MT -->|Create| RDT
        MT -->|Create| SRT
        MT -->|Create| SST

        RDT -->|30 FPS| FB[Framebuffer]
        SRT -->|Receive| SB[Screen Ball List]
        SST -->|Send| SV[Server]

        UI[User Input] -->|Commands| SST
    end

    subgraph "Server Threads"
        ST[Main Thread]
        WT1[Worker Thread 1]
        WT2[Worker Thread 2]
        WTN[Worker Thread N]
        CBT[Cycle Broadcast Thread]

        ST -->|Create| WT1
        ST -->|Create| WT2
        ST -->|Create| WTN
        ST -->|Create| CBT

        ST -->|epoll_wait| EP[Epoll Events]
        EP -->|Accept| ST
        EP -->|EPOLLIN| ST

        ST -->|Task| TQ[Task Queue]
        TQ -->|Dequeue| WT1
        TQ -->|Dequeue| WT2
        TQ -->|Dequeue| WTN

        CBT -->|33 FPS| LBM[Local Ball Manager]
        CBT -->|Broadcast| CLM[Client List Manager]
    end
```

### Data Flow

```mermaid
flowchart LR
    subgraph "Client"
        UI[User Input]
        SB[Screen Ball Manager]
        FB[Framebuffer]
        UI -->|Commands| SB
        SB -->|Render| FB
    end

    subgraph "Network"
        TCP[TCP/IP Socket]
    end

    subgraph "Server"
        CM[Command Parser]
        LBM[Local Ball Manager]
        CLM[Client List Manager]
        CM -->|Process| LBM
        LBM -->|Update| CLM
        CLM -->|Broadcast| TCP
    end

    UI -->|Send| TCP
    TCP -->|Receive| CM
    TCP -->|Receive| SB
```

## Project Overview

Ball Game is a networked application that consists of a server and multiple clients. The server manages the game state and coordinates between clients, while each client connects to the server and displays the game on a framebuffer device.

## Architecture

The project is organized into three main components:

### Server Component

- **Server**: Manages client connections and game state
- **Client List Manager**: Handles client connections and disconnections
- **Local Ball Manager**: Manages the ball objects on the server side
- **Task Queue**: Processes client commands asynchronously

### Client Component

- **Client**: Connects to the server and receives game state updates
- **Screen Ball Manager**: Manages the display of balls on the client's screen
- **Screen Ball List**: Maintains a list of balls to be displayed

### Shared Component

- **Framebuffer Drawing**: Provides functions for drawing on the framebuffer
- **Logging**: Handles logging of events and errors
- **Console Color**: Provides colored output for console messages

## Features

- **Multi-client Support**: Multiple clients can connect to the server simultaneously
- **Real-time Updates**: Changes to the game state are broadcast to all connected clients
- **Interactive Ball Manipulation**: Clients can add, delete, and modify balls
- **Framebuffer Display**: Game is displayed directly on the framebuffer device
- **Threaded Architecture**: Uses multiple threads for rendering, sending, and receiving data

## Directory Structure

```
Ball_Game/
├── include/
│   ├── client/
│   │   ├── client.h
│   │   ├── screenball.h
│   │   ├── screenball_list.h
│   │   └── screenballmanager.h
│   ├── server/
│   │   ├── client_list_manager.h
│   │   ├── localball.h
│   │   ├── localball_list.h
│   │   ├── localballmanager.h
│   │   ├── server.h
│   │   └── task.h
│   └── shared/
│       ├── console_color.h
│       ├── fbDraw.h
│       └── log.h
└── src/
    ├── client/
    │   ├── client.c
    │   ├── main.c
    │   ├── screenball.c
    │   ├── screenball_list.c
    │   └── screenballmanager.c
    ├── server/
    │   ├── client_list_manager.c
    │   ├── localballmanager.c
    │   ├── localball_list.c
    │   ├── loaclball.c
    │   ├── main.c
    │   ├── server.c
    │   └── task.c
    └── shared/
        ├── fbDraw.c
        └── log.c
```

## Key Components

### Server

- Manages client connections using sockets
- Processes client commands and updates the game state
- Broadcasts state changes to all connected clients
- Uses a task queue for asynchronous command processing

### Client

- Connects to the server using sockets
- Receives game state updates from the server
- Renders the game state on the framebuffer
- Sends user commands to the server

### Ball Management

- **Server-side**: LocalBallManager maintains the authoritative game state
- **Client-side**: ScreenBallManager displays the game state
- Balls have properties such as position, radius, and color

### Framebuffer Drawing

- Provides low-level drawing functions for the framebuffer
- Supports drawing shapes, lines, and text
- Handles color management and screen updates

## Building and Running

### Prerequisites

- Linux operating system with framebuffer support
- GCC compiler
- Make build system
- pthread library

### Build Instructions

1. Clone the repository
2. Navigate to the project directory
3. Run `make` to build both server and client

### Running the Application

1. Start the server: `./server`
2. Start one or more clients: `./client`

## Author

Kim Hyo Jin

## License

This project is proprietary and confidential.
