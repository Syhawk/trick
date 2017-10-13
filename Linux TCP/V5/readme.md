I. Target
1. TCP server connect with TCP client.

II. TCP Server
1. Server broadcast to some clients.
2. Server use poll for each client.
3. Server use setsockopt resume system port.

III. TCP Client
1. Client use thread to write or read message.
2. Client use shutdown to close connect.
3. Client will close if server close.

IV. Interface
1. For solving socket(TCP)'s sticky package problem, use special suffix('\n').
2. For solving problem buffer insufficient, implement new method by old method(write, read -> writen, readn).
3. Give new method readline, recv_peek to process suffix('\n').
4. For I/O conflict, use select and thread_cond to solve.
5. Use gethostname get hostname, use hostname get local ip.
6. Give new timeout function to solve all kinds of timeout action.
