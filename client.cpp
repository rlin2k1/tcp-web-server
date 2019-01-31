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
#include <fcntl.h> //For Socket Non Blocking
#include <netdb.h> //For GetHostByName()

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
    exit(3);
  }

  const char* hostname = argv[1]; //The HostName/IP Address is First Argument
  int port_number = -1; //Sentinel
  try{
    port_number = stoi(argv[2]); //The Port Number is the Second Argument
  }
  catch(std::invalid_argument& e) {
    cerr << "ERROR: Invalid Port. Please Enter Valid Port NUMBER" << endl;
    exit(3);
  }
  if(port_number <= 1023){
    cerr << "ERROR: Reserved Port Number Detected. Please Specify a Port Number"
      << " Greater than 1023" << endl;
    exit(3);
  }
  string file_name = argv[3]; //Assume File Name is Always Correct - 3rd Arg
  FILE *fs = fopen(file_name.c_str(), "r"); //Open the File for Reading
  if(fs == nullptr){
    cerr << "ERROR: File Does NOT Exist!" << endl;
    exit(3);
  }

  cerr << "Hostname to Be Connected to: " << hostname << endl;
  cerr << "Port Number to Be Connected to: " << port_number << endl;
  cerr << "File Name to Transfer: " << file_name << endl;

  // ------------------------------------------------------------------------ //
  // Create a Socket using TCP IP
  // ------------------------------------------------------------------------ //
  int sockfd; //Socket File Descriptor
  struct addrinfo hints, *servinfo, *p; //For getaddrinfo()
  int rv; //Check Return Value of getaddrinfo()

  memset(&hints, 0, sizeof hints); //Initialize Hints
  hints.ai_family = AF_INET; // AF_INET for IPv4
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(hostname, argv[2], &hints, &servinfo)) != 0)//GetAddrInfo
  {
    cerr << "ERROR: Get Address Info Failed" << endl;
    exit(3);
  }

  // Loop through all results and try to connect
  for(p = servinfo; p != NULL; p = p->ai_next) {
      //sockfd contains the file descriptor to access the Socket
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
      { //Create Socket for the Result
          cerr << "ERROR: Failed to Create Socket" << endl;
          continue; //On to the Next Result
      }

      // -------------------------------------------------------------------- //
      // Connect to the Server
      // -------------------------------------------------------------------- //
      int flags = fcntl(sockfd, F_GETFL); //Get Flags from the Socket
      flags |= O_NONBLOCK; //Add the Non Blocking Flag to the Socket Flags Set
      fcntl(sockfd, F_SETFL, flags); //Set The Socket to NONBLOCKING
      socklen_t slen; //Socket Length
      int errorCheck; //Error Code

      int res = connect(sockfd, p->ai_addr, p->ai_addrlen);
      if (res < 0) { //Possible Error
      if(errno == EINPROGRESS) {
        fd_set active_fd_set;
        fd_set working_fd_set;

        FD_ZERO (&active_fd_set); //Zero Out the Active File Descriptor Set
        FD_SET (sockfd, &active_fd_set); //Add the newSocket to the Working Set

        working_fd_set = active_fd_set; //Determine the Working Set

        struct timeval tv = {15, 0};   //Set the Timeout Interval as 15 Seconds!
        if (select(sockfd + 1, NULL, &working_fd_set, NULL, &tv) > 0) 
        { 
          slen = sizeof(int); 
          getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&errorCheck), &slen); 
          if (errorCheck) { 
            cerr << "ERROR: Connection Error" << endl;
            exit(3);
          } 
        } 
        else 
        { 
          cerr << "ERROR: Connection Timed Out Error" << endl;
          exit(3);
        }
      } 
      else { 
        cerr << "ERROR: Connection Error" << endl;
        exit(3); 
      } 
    } 
    //Make the Socket Block Again
    flags = fcntl(sockfd, F_GETFL); 
    flags &= ~O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);

    break; // Must have connected successfully
  }

  if (p == NULL) {
    // End of List with No Successful Bind
    cerr << "ERROR: Failed to Bind Socket" << endl;
    exit(3);
  }

  freeaddrinfo(servinfo); // Deallocate Structure
  //sockfd contains the file descriptor to access the Socket
  // int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // struct sockaddr_in serverAddr; //Get Ready to Store the Server Address
  // serverAddr.sin_family = AF_INET; //Type
  // serverAddr.sin_port = htons(port_number); //Define the Port Number
  // //Short, Network Byte Order
  // serverAddr.sin_addr.s_addr = inet_addr(hostname); //Where is the Server IP
  // memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
  //Memset to Null Bytes

  // ------------------------------------------------------------------------ //
  // Connect to the Server
  // ------------------------------------------------------------------------ //
  // int flags = fcntl(sockfd, F_GETFL); //Get Flags from the Socket
  // flags |= O_NONBLOCK; //Add the Non Blocking Flag to the Socket Flags Set
  // fcntl(sockfd, F_SETFL, flags); //Set The Socket to NONBLOCKING
  // socklen_t slen; //Socket Length
  // int errorCheck; //Error Code

//int res = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  // if (res < 0) { //Possible Error
  //   if(errno == EINPROGRESS) {
  //     fd_set active_fd_set;
  //     fd_set working_fd_set;

  //     FD_ZERO (&active_fd_set); //Zero Out the Active File Descriptor Set
  //     FD_SET (sockfd, &active_fd_set); //Add the newSocket to the Working Set

  //     working_fd_set = active_fd_set; //Determine the Working Set

  //     struct timeval tv = {15, 0};  //Set the Timeout Interval as 15 Seconds!
  //     if (select(sockfd + 1, NULL, &working_fd_set, NULL, &tv) > 0) 
  //     { 
  //       slen = sizeof(int); 
//       getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&errorCheck), &slen); 
  //       if (errorCheck) { 
  //         cerr << "ERROR: Connection Error" << endl;
  //         exit(3);
  //       } 
  //     } 
  //     else 
  //     { 
  //       cerr << "ERROR: Connection Timed Out Error" << endl;
  //       exit(3);
  //     }
  //    } 
  //    else { 
  //     cerr << "ERROR: Connection Error" << endl;
  //     exit(3); 
  //    } 
  // } 
  // //Make the Socket Block Again
  // flags = fcntl(sockfd, F_GETFL); 
  // flags &= ~O_NONBLOCK;
  // fcntl(sockfd, F_SETFL, flags);

  struct sockaddr_in clientAddr; //Save the Client Address
  socklen_t clientAddrLen = sizeof(clientAddr); //Size of the Client Address
  if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1)
  {
    //Make Sure Client Address is OK
    cerr << "ERROR: Getsockname Function Failed" << endl;
    close(sockfd); //Finally Close the Connection
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
  char buf[BUFLENGTH] = {0}; //Set the Buffer as BUFLENGTH Characters

  memset(buf, '\0', sizeof(buf)); //Reset Buffer Every Time

  int fs_block_sz;
  while((fs_block_sz = fread(buf, sizeof(char), BUFLENGTH, fs)) > 0)
  {
    cerr << "Send: " << buf << endl; //Output the Buffer into Standard Output;
    if (send(sockfd, buf, fs_block_sz, MSG_NOSIGNAL) == -1) {
      //Attempt to Send the BUFFER String Over the Socket Connection
      cerr << "ERROR: Send Function Failed" << endl;
      close(sockfd); //Finally Close the Connection
      exit(3);
    }
  }
  close(sockfd); //Finally Close the Connection
  return 0; //Exit Normally
}