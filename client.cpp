/* client.cpp
The client connects to the server and as soon as connection established, sends
the content of a file to the server.

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

#include <getopt.h>

using namespace std;
int
main(int argc, char *argv[])
{
  // ------------------------------------------------------------------------ //
  // Error Handling from Arguments
  // ------------------------------------------------------------------------ //
  if(argc != 4) {
    cerr << "Need 3 Arguments: HostName, Port Number, and File Name to Transfer" << endl;
    exit(1);
  }
  const char* hostname = argv[1];
  int port_number = stoi(argv[2]);
  if(port_number <= 1023){
    cerr << "Reserved Port Number Detected. Please Specify a Port Number Greater than 1023" << endl;
    exit(1);
  }
  string file_name = argv[3]; //Assume File Name is Always Correct

  cout << "Hostname to Be Connected to: " << hostname << endl;
  cout << "Port Number to Be Connected to: " << port_number << endl;
  cout << "File Name to Transfer: " << file_name << endl;

  // ------------------------------------------------------------------------ //
  // Create a Socket using TCP IP
  // ------------------------------------------------------------------------ //
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // struct sockaddr_in addr;
  // addr.sin_family = AF_INET;
  // addr.sin_port = htons(40001);     // short, network byte order
  // addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
  // if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
  //   perror("bind");
  //   return 1;
  // }
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port_number);         // Short, Network Byte Order
  serverAddr.sin_addr.s_addr = inet_addr(hostname);
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  // ------------------------------------------------------------------------ //
  // Connect to the Server
  // ------------------------------------------------------------------------ //
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    perror("connect");
    return 2;
  }

  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
    perror("getsockname");
    return 3;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Set up a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;


  // -------------------------------------------------------------------------- //
  // Send/Receive Data to/from Connection
  // -------------------------------------------------------------------------- //
  bool isEnd = false;
  std::string input;
  char buf[20] = {0};
  std::stringstream ss;

  while (!isEnd) {
    memset(buf, '\0', sizeof(buf));

    std::cout << "send: ";
    std::cin >> input;
    if (send(sockfd, input.c_str(), input.size(), 0) == -1) {
      perror("send");
      return 4;
    }


    if (recv(sockfd, buf, 20, 0) == -1) {
      perror("recv");
      return 5;
    }
    ss << buf << std::endl;
    std::cout << "echo: ";
    std::cout << buf << std::endl;

    if (ss.str() == "close\n")
      break;

    ss.str("");
  }

  close(sockfd);

  return 0;
}