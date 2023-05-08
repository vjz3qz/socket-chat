# Simple Chat Program

This is a simple chat program written in C that allows two users to communicate with each other over a network.

## Requirements

- C compiler (such as GCC)
- Linux or Unix-like operating system

## Usage

To use the program, follow these steps:

1. Compile the program using the following command:

gcc chat.c -o chat


2. Start the server by running the following command:

./chat [<port>]


If you do not specify a port number, a random port number will be used. The server will display its IP address and the port number it is listening on.

3. Start the client by running the following command:


Replace `<ip_address>` with the IP address of the server and `<port>` with the port number that the server is listening on. The client will connect to the server and display a prompt.

4. Type a message at the prompt and press Enter to send it to the other user. The message will be displayed on the other user's screen.

5. To exit the program, type the word "exit" at the prompt and press Enter.

## Limitations

- This program only supports communication between two users. It does not support group chat or broadcast messages.
- The program does not provide any encryption or authentication mechanisms, and therefore should not be used for sensitive or confidential communication.
