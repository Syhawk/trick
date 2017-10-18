I. Target
1. Process relationship communication.

II. UDP Server
1. Father process communicate with subprocess.

IV. Interface
1. For solving socket(TCP)'s sticky package problem, use special suffix('\n').
2. For solving problem buffer insufficient, implement new method by old method(write, read -> writen, readn).
3. Give new method readline, recv_peek to process suffix('\n').
4. For I/O conflict, use select and thread_cond to solve.
5. Use gethostname get hostname, use hostname get local ip.
6. Give new timeout function to solve all kinds of timeout action.
7. Errno EINTR error check.

V. Common
1. Give some protocol and message format about user chat.
