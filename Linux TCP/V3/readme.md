1. TCP server connect with TCP client.
2. Server broadcast to some clients.
3. Server use thread for each client.
4. Client use thread to write or read message.
5. For solving socket(TCP)'s sticky package problem, use special suffix('\n').
6. For solving problem buffer insufficient, implement new method by old method(write, read -> writen, readn).
7. Give new method readline, recv_peek to process suffix('\n').
8. For I/O conflict, use select and thread_cond to solve.
9. Use gethostname get hostname, use hostname get local ip.
10. Use setsockopt resume system port.
