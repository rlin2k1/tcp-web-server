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
#include <sys/socket.h> //For the Socket()
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> //String Library
#include <stdio.h> //Standard Input/Ouput Library
#include <errno.h> //Error Handling
#include <unistd.h>
#include <thread> //For Multi-Threading - Handling Multiple Clients
#include <iostream> //Input and Output

using namespace std; //Using the Standard Namespace

#define BUFLENGTH 1024

int main(int argc, char *argv[]) //Main Function w/ Arguments from Command Line
{
  // ------------------------------------------------------------------------ //
  // Error Handling from Arguments
  // ------------------------------------------------------------------------ //
  if(argc != 4) {
    cerr << "ERROR: Need 3 Arguments: HostName, Port Number, and File Name to " 
      << "Transfer" << endl;
    exit(1);
  }

  const char* hostname = argv[1]; //The HostName/IP Address is 1st Argument
  int port_number = stoi(argv[2]); //The Port Number is the 2nd Argument
  if(port_number <= 1023){
    cerr << "ERROR: Reserved Port Number Detected. Please Specify a Port Number"
      << " Greater than 1023" << endl;
    exit(1);
  }
  string file_name = argv[3]; //Assume File Name is Always Correct - 3rd Arg

  cerr << "Hostname to Be Connected to: " << hostname << endl;
  cerr << "Port Number to Be Connected to: " << port_number << endl;
  cerr << "File Name to Transfer: " << file_name << endl;

  // ------------------------------------------------------------------------ //
  // Create a Socket using TCP IP
  // ------------------------------------------------------------------------ //
  //sockfd contains the file descriptor to access the Socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serverAddr; //Get Ready to Store the Server Address
  serverAddr.sin_family = AF_INET; //Type
  serverAddr.sin_port = htons(port_number); //Define the Port Number
  //Short, Network Byte Order
  serverAddr.sin_addr.s_addr = inet_addr(hostname); //Where is the Server IP
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
  //Memset to Null Bytes

  // ------------------------------------------------------------------------ //
  // Connect to the Server
  // ------------------------------------------------------------------------ //
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
  //Attempt to Connect to the Given HostName/IP Address and Port Number
  {
    cerr << "ERROR: Connect Function Failed" << endl;
    exit(2);
  }

  struct sockaddr_in clientAddr; //Save the Client Address
  socklen_t clientAddrLen = sizeof(clientAddr); //Size of the Client Address
  if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1)
  {
    //Make Sure Client Address is OK
    cerr << "ERROR: Getsockname Function Failed" << endl;
    exit(3);
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'}; //Set String to NullByte
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  //Put Client Information in 'ipstr'
  cerr << "Set Up a Connection From: " << ipstr << ":" << \
  ntohs(clientAddr.sin_port) << endl; //State Who the Client Is

  // ------------------------------------------------------------------------ //
  // Send/Receive Data to/from Connection
  // ------------------------------------------------------------------------ //
  FILE *fs = fopen(file_name.c_str(), "r"); //Open the File for Reading
  char buf[BUFLENGTH] = {0}; //Set the Buffer as BUFLENGTH Characters

  memset(buf, '\0', sizeof(buf)); //Reset Buffer Every Time

  int fs_block_sz;
  while((fs_block_sz = fread(buf, sizeof(char), 500, fs)) > 0)
  {
    cerr << "Send: " << buf << endl; //Output the Buffer into Standard Output;
    if (send(sockfd, buf, fs_block_sz, 0) == -1) {
      //Attempt to Send the BUFFER String Over the Socket Connection
      cerr << "ERROR: Send Function Failed" << endl;
      exit(4);
    }
  }
  close(sockfd); //Finally Close the Connection

  return 0; //Exit Normally
}