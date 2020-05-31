# TCP Web Server
By: Roy Lin<br>

## High Level Design of Server
My high level design of the Server was based off of the Sample Code server.cpp provided by the Hints Section of the Project Specification and the Server Code from Section Slides. Basically, the Server was designed to take inputs from the User for PORT Number, and directory to save transferred files in. Server was designed such that it can accept multiple clients at once - allocating 100 Threads so that 1 Thread can handle 1 Connection - thereby making up for 100 Connections possible to the Server. The Server Socket is first created and Set to Non-Blocking Mode as to accept multiple Clients and Not Block if a single Client fails when connecting. Then the Server waits for incoming Client Connections as passes a newly created unique Socket to one of the 100 Threads to handle that incoming connection. That thread reads from the socket into a buffer and writes directly to a new file created in the User's specified directory. Since each thread is writing into a different file, no race conditions are present.

## High Level Design of Client
My high level design of the Client was based off of the Sample Code client.cpp provided by the Hints Section of the Project Specification and the Client Code from Section Slides. Basically, the Client was designed to take inputs from the User for PORT Number, Hostname of Server (IP Address), and the File to be transferred. The Client then binds the Hostname and PORT Number given to open a Socket TCP Connection to the Server. The Client attempts to open the User requested file for transfer and reads from the file into a buffer and sends that buffer over the Socket.

## Problems I Ran Into
This project was very similar to the CS 111 Project 1B in Spring of 2018 - creating Clients and Servers to connect to each other through Socket TCP Connection. The only difference was that I did not know how to create a Multithreaded Server - and that was the problem I ran into as when one Client was connected, another Client could not connect at the same time. I solved this issue by taking a look at the Sample Multithread Code and the DZONE Article detailing Server Multithreading.<br>
I ran into issues on Select not timing out - or timing out and not writing an error message into the specified file. I solved this issue by looking in the MAN Section of the Unix Terminal to see the Select() Specification.<br>
I ran into errors while making Buffers - not everything would be sent from the Client Side to the Host Side. I solved this issue by taking a close look at the code and realizing that the Buffer size in the Server did not add up to each other - was basically using different Buffer lengths. So, I created one Global Macro detailing the buffer length in both the Server and Client Programs to solve this issue.<br>
I ran into problems trying discard already written text into the Server side file when the Socket would time out. I solved this issue by looking how to reopen a File for writing at the very beginning. -> freopen()<br>
I even ran into problems trying to pass multiple arguments to the function the Thread would run on - eventually learning to put the arguments into a struct.<br>
Although I ran into a lot of problems, these were common pitfalls when working with network operations. Many of the problems were met by other people on Google and the solutions to problems could readily be found online.

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
#include <netdb.h> //For getaddrinfo()

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
- https://stackoverflow.com/questions/1352749/multiple-arguments-to-function-called-by-pthread-create
- http://www.cplusplus.com/reference/cstdio/freopen/
- https://stackoverflow.com/questions/5269683/rewindfile-pointer-and-then-fwrite-wont-fwrite-overwrite-the-file-contents
- http://www.cplusplus.com/reference/cstdio/fopen/
- http://developerweb.net/viewtopic.php?id=3196
- https://en.wikibooks.org/wiki/C_Programming/POSIX_Reference/netdb.h/getaddrinfo#Example
- https://stackoverflow.com/questions/16372700/how-to-use-getaddrinfo-to-connect-to-a-server-using-the-external-ip
- https://docs.microsoft.com/en-us/windows/desktop/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
- https://www.dreamincode.net/forums/topic/109330-how-to-use-getaddrinfo/

## Makefile

This provides a couple make targets for things.
By default (all target), it makes the `server` and `client` executables.

It provides a `clean` target, and `tarball` target to create the submission file as well.

You will need to modify the `Makefile` to add your userid for the `.tar.gz` turn-in at the top of the file.

## Academic Integrity Note

You are encouraged to host your code in private repositories on [GitHub](https://github.com/), [GitLab](https://gitlab.com), or other places.  At the same time, you are PROHIBITED to make your code for the class project public during the class or any time after the class.  If you do so, you will be violating academic honestly policy that you have signed, as well as the student code of conduct and be subject to serious sanctions.

## Provided Files

`server.cpp` and `client.cpp` are the entry points for the server and client part of the project.
