/* server.cpp
The server listens for TCP connections and saves all the received data from
the client in a file.

Author(s):
  Roy Lin

Date Created:
  January 16th, 2019
*/

// -------------------------------------------------------------------------- //
// Import Statements for the Necessary Packages
// -------------------------------------------------------------------------- //
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <sstream>

using namespace std;

int
main(int argc, char *argv[])
{
  // ------------------------------------------------------------------------ //
  // Error Handling from Arguments
  // ------------------------------------------------------------------------ //
  if(argc != 3) {
    cerr << "Need 2 Arguments: Port Number and File Directory" << endl;
    exit(1);
  }

  int port_number = stoi(argv[1]);
  if(port_number <= 1023){
    cerr << "Reserved Port Number Detected. Please Specify a Port Number Greater than 1023" << endl;
    exit(1);
  }
  string file_directory = argv[2]; //Assume Directory is Always Correct

  cout << "Port Number to Be Connected to: " << port_number << endl;
  cout << "Directory to Save Transferred File in: " << file_directory << endl;
  
  // ------------------------------------------------------------------------ //
  // Create a Socket using TCP IP
  // ------------------------------------------------------------------------ //
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // ------------------------------------------------------------------------ //
  // Allow Others to Reuse the Address
  // ------------------------------------------------------------------------ //
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    return 1;
  }

  // ------------------------------------------------------------------------ //
  // Bind Address to Socket
  // ------------------------------------------------------------------------ //
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port_number);               // Short, Network Byte Order
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind");
    return 2;
  }

  // ------------------------------------------------------------------------ //
  // Set Socket to Listen Status
  // ------------------------------------------------------------------------ //
  if (listen(sockfd, 1) == -1) {
    perror("listen");
    return 3;
  }

  // ------------------------------------------------------------------------ //
  // Accept a New Connection
  // ------------------------------------------------------------------------ //
  struct sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);
  int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

  if (clientSockfd == -1) {
    perror("accept");
    return 4;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Accept a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;

  // ------------------------------------------------------------------------ //
  // Read/Write Data From/Into the Connection
  // ------------------------------------------------------------------------ //
  bool isEnd = false;
  char buf[20] = {0};
  std::stringstream ss;

  while (!isEnd) {
    memset(buf, '\0', sizeof(buf));

    if (recv(clientSockfd, buf, 20, 0) == -1) {
      perror("recv");
      return 5;
    }

    ss << buf << std::endl;
    std::cout << buf << std::endl;

    if (send(clientSockfd, buf, 20, 0) == -1) {
      perror("send");
      return 6;
    }

    if (ss.str() == "close\n")
      break;

    ss.str("");
  }

  close(clientSockfd);

  return 0;
}