# CS118 Project 1
Name: Roy Lin<br>
UCLA ID: 704-767-891<br>

## High Level Design of Server
My high level design of the Server was based off of the Sample Code server.cpp provided by the Hints Section of the Project Specification and the Server Code from TA ZHEHUI ZANG's Discussion Section Slides. Basically, the Server was designed to take inputs from the User for PORT Number, and directory to save transferred files in. Server was designed such that it can accept multiple clients at once - allocating 100 Threads so that 1 Thread can handle 1 Connection - thereby making up for 100 Connections possible to the Server. The Server Socket is first created and Set to Non-Blocking Mode as to accept multiple Clients and Not Block if a single Client fails when connecting. Then the Server waits for incoming Client Connections as passes a newly created unique Socket to one of the 100 Threads to handle that incoming connection. That thread reads from the socket into a buffer and writes directly to a new file created in the User's specified directory. Since each thread is writing into a different file, no race conditions are present.

## High Level Design of Client
My high level design of the Client was based off of the Sample Code client.cpp provided by the Hints Section of the Project Specification and the Client Code from TA ZHEHUI ZANG's Discussion Section Slides. Basically, the Client was designed to take inputs from the User for PORT Number, Hostname of Server (IP Address), and the File to be transferred. The Client then binds the Hostname and PORT Number given to open a Socket TCP Connection to the Server. The Client attempts to open the User requested file for transfer and reads from the file into a buffer and sends that buffer over the Socket.

## Problems I Ran Into
This CS 118 Project 1 was very similar to the CS 111 Project 1B in Spring of 2018 - creating Clients and Servers to connect to each other through Socket TCP Connection. The only difference was that I did not know how to create a Multithreaded Server - and that was the problem I ran into as when one Client was connected, another Client could not connect at the same time. I ran into issues on Select not timing out - or timing out and not writing an error message into the specified file. I ran into errors while making Buffers - not everything would be sent from the Client Side to the Host Side. I ran into problems trying discard already written text into the Server side file when the Socket would time out. I even ran into problems trying to pass multiple arguments to the function the Thread would run on - eventually learning to put the arguments into a struct. Although I ran into a lot of problems, these were common pitfalls when working with network operations. Many of the problems were met by other people on Google and the solutions to problems could readily be found online.

## List of Additional Libraries Used
I had quite a bit of included libraries:<br>
#include <sys/types.h><br>
#include <sys/socket.h> //For the Socket()<br>
#include <netinet/in.h><br>
#include <arpa/inet.h><br>
#include <string.h> //String Library<br>
#include <stdio.h> //Standard Input/Ouput Library<br>
#include <errno.h> //Error Handling<br>
#include <unistd.h><br>
#include \<thread\> //For Multi-Threading - Handling Multiple Clients<br>
#include \<iostream\> //Input and Output<br>
#include <signal.h> //For Signal Processing and KILL() Command<br>
#include <fcntl.h> //For Socket Non Blocking<br>
#include \<chrono\> //For Timing<br>
#include <stdlib.h> //For Standard Library<br>
#include <sys/wait.h><br>
#include \<fstream\><br>
#include <sys/select.h> //For the Select()<br>

## Online Tutorials or Code Examples
Code Examples:
- http://web.cs.ucla.edu/classes/spring17/cs118/hints/server.cpp
- http://web.cs.ucla.edu/classes/spring17/cs118/hints/client.cpp
- http://web.cs.ucla.edu/classes/spring17/cs118/hints/multi-thread.cpp

Online Tutorials:
- https://en.wikipedia.org/wiki/Berkeley_sockets
- http://beej.us/guide/bgnet/
- https://www.cs.dartmouth.edu/~campbell/cs60/socketprogramming.html
- https://dzone.com/articles/parallel-tcpip-socket-server-with-multi-threading
- http://man7.org/linux/man-pages/man2/select.2.html
- https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/

## Makefile

This provides a couple make targets for things.
By default (all target), it makes the `server` and `client` executables.

It provides a `clean` target, and `tarball` target to create the submission file as well.

You will need to modify the `Makefile` to add your userid for the `.tar.gz` turn-in at the top of the file.

## Academic Integrity Note

You are encouraged to host your code in private repositories on [GitHub](https://github.com/), [GitLab](https://gitlab.com), or other places.  At the same time, you are PROHIBITED to make your code for the class project public during the class or any time after the class.  If you do so, you will be violating academic honestly policy that you have signed, as well as the student code of conduct and be subject to serious sanctions.

## Provided Files

`server.cpp` and `client.cpp` are the entry points for the server and client part of the project.
