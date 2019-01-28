/* server.cpp
The server listens for TCP connections and saves all the received data from
the client in a file.

Notes: Mainly use select() for 15 Second Timer!

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
#include <sstream> //String Stream

#include <signal.h> //For Signal Processing and KILL() Command
#include <fcntl.h> //For Socket Non Blocking
#include <chrono> //For Timing

#include <stdlib.h> //For Standard Library
#include <sys/wait.h>

#include <fstream>
#include <sys/select.h> //For the Select()

#include <sys/stat.h> //For MakeDir()
#include <netdb.h> //For GetHostByName()

using namespace std; //Using the Standard Namespace

#define BUFLENGTH 1025 //One Bigger than the Client Buffer Length

struct arg_struct { 
    int socket;          // Socket File Descriptor
    int num;             // File Number - Order Connected to the Socket
};

string file_directory = "";

//No Mutex Because No Race Conditions When Reading from Socket or Writing File

void *socketThread(void *arg)
{
  int newSocket = ((struct arg_struct *)arg)->socket;
  int num = ((struct arg_struct *)arg)->num;

  fd_set active_fd_set;
  fd_set working_fd_set;
  FD_ZERO (&active_fd_set); //Zero Out the Active File Descriptor Set
  FD_SET (newSocket, &active_fd_set); //Add the newSocket to the Working Set

  string file_path = file_directory + "/" + to_string(num) + ".file"; //FileName
  FILE *fs = fopen(file_path.c_str(), "wb"); //Open the File for Modification

  char error[6] = "ERROR";
  char buf[BUFLENGTH] = {0}; //Set the Buffer as BUFLENGTH Characters

  while(true)
  { //When We are Not Over
    memset(buf, '\0', sizeof(buf)); //Reset Buffer Every Time

    working_fd_set = active_fd_set; //Determine the Working Set
    struct timeval tv = {15, 0};   //Set the Timeout Interval as 15 Seconds!

    int rc = select(newSocket + 1, &working_fd_set, NULL, NULL, &tv);
    if (rc < 0)
    {
        cerr << "ERROR: SELECT() FAILED" << endl;
        close(newSocket); //Finally Close the Connection
        pthread_exit(NULL);
        exit(3);
    }
    if (rc == 0)
    {
        FILE *fd = freopen(file_path.c_str(), "w", fs); //Rewrite the File!
        fwrite(error, 1, 6, fd); //Write the Error Message into the File
        fflush(fd); //Make Sure Everything is Written to File!
        cerr << "ERROR: Select Timed Out!" << endl;
        close(newSocket); //Finally Close the Connection
        pthread_exit(NULL);
        exit(3);
    }
    int fr_block_sz = 0; //We Got Something!
    while((fr_block_sz = recv(newSocket, buf, BUFLENGTH, 0)) > 0)
    {
        cerr << buf << endl; //Output the Buffer into Standard Output
        int write_sz = fwrite(buf, sizeof(char), fr_block_sz, fs); //Write File
        fflush(fs); //Make Sure Everything is Written to File!
        if(write_sz < fr_block_sz)
        {
          cerr << "ERROR: Write Operation Failed on the Server" << endl;
        }
        memset(buf, '\0', sizeof(buf)); //Reset Buffer Every Time
        if (fr_block_sz == 0 || fr_block_sz != BUFLENGTH) 
        {
          break; //Done Reading!
        }
    }
    if(fr_block_sz < 0)
    {
        if (errno == EAGAIN)
        {
          cerr << "ERROR: RECV() TIMED OUT" << endl;
        }
        else
        {
          cerr << "ERROR: RECV() FAILED" << endl;
          close(newSocket); //Finally Close the Connection
          pthread_exit(NULL);
          exit(3);
        }
    }
    
  }
  close(newSocket); //Finally Close the Connection
  pthread_exit(NULL);
}

void sigquit_handler(int signum) {
  cerr << "ERROR: Received SIGQUIT Signal" << endl;
  exit(0);
}

void sigterm_handler(int signum) {
  cerr << "ERROR: Received SIGTERM Signal" << endl;
  exit(0);
}

int main(int argc, char *argv[]) //Main Function w/ Arguments from Command Line
{
  // ------------------------------------------------------------------------ //
  // Signal Handling
  // ------------------------------------------------------------------------ //
  signal(SIGQUIT, sigquit_handler);
  signal(SIGTERM, sigterm_handler);
  int port_number = -1; //Sentinel
  // ------------------------------------------------------------------------ //
  // Error Handling from Arguments
  // ------------------------------------------------------------------------ //
  if(argc != 3) {
    cerr << "ERROR: Need 2 Arguments: Port Number and File Directory" << endl;
    exit(3);
  }
  try{
    port_number = stoi(argv[1]); //The Port Number is the First Argument
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
  file_directory = argv[2]; //Assume Directory is Always Correct-2nd Arg
  mkdir(file_directory.c_str(), 0777); //Will Always Have Permissions

  cerr << "Port Number to Be Connected to: " << port_number << endl;
  cerr << "Directory to Save Transferred File in: " << file_directory << endl;
  
  // ------------------------------------------------------------------------ //
  // Create a Socket using TCP IP
  // ------------------------------------------------------------------------ //
  int sockfd; //Socket File Descriptor
  struct addrinfo hints, *servinfo, *p; //For getaddrinfo()
  int rv; //Check Return Value of getaddrinfo()

  memset(&hints, 0, sizeof hints); //Initialize Hints
  hints.ai_family = AF_INET; // AF_INET for IPv4
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // Use my IP address

  if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) { //GetAddrInfo
    cerr << "ERROR: Get Address Info Failed" << endl;
    exit(3);
  }

  // Loop through all results and try to bind
  for(p = servinfo; p != NULL; p = p->ai_next) {
      //sockfd contains the file descriptor to access the Socket
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
      { //Create Socket for the Result
          cerr << "ERROR: Failed to Create Socket" << endl;
          continue; //On to the Next Result
      }
      // -------------------------------------------------------------------- //
      // Allow Others to Reuse the Address - Error Handling
      // -------------------------------------------------------------------- //
      int yes = 1;
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
      {
        cerr << "ERROR: Set Socket Options Failed" << endl;
      }
      fcntl(sockfd, F_SETFL, O_NONBLOCK);
      // -------------------------------------------------------------------- //
      // Bind Address to Socket
      // -------------------------------------------------------------------- //
      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          cerr << "ERROR: Error Binding the Port and the HostName/IP Address" <<
          "Together" << endl;
          close(sockfd); //Finally Close the Connection
          continue;
      }

      break; // Must have connected successfully
  }

  if (p == NULL) {
      // End of List with No Successful Bind
      cerr << "ERROR: Failed to Bind Socket" << endl;
      exit(3);
  }

  freeaddrinfo(servinfo); // Deallocate Structure
  //sockfd contains the file descriptor to access the Socket
  //int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // ------------------------------------------------------------------------ //
  // Allow Others to Reuse the Address - Error Handling
  // ------------------------------------------------------------------------ //
  // int yes = 1;
  // if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
  // {
  //   cerr << "ERROR: Set Socket Options Failed" << endl;
  //   close(sockfd); //Finally Close the Connection
  //   exit(3);
  // }
  //
  // fcntl(sockfd, F_SETFL, O_NONBLOCK);
  // if (fcntl(sockfd, F_SETFL, O_NONBLOCK)  == -1) {
  //   perror("fcntyl");
  //   return 1;
  // }

  // ------------------------------------------------------------------------ //
  // Bind Address to Socket
  // ------------------------------------------------------------------------ //
  // struct sockaddr_in addr;

  // addr.sin_family = AF_INET; //Type
  // addr.sin_port = htons(port_number); 
  //DefinePortNumber - ShortNetworkByteOrder

  // addr.sin_addr.s_addr = *(in_addr_t *) hostinfo->h_addr; 
  //Where Hosting From
  // memset(addr.sin_zero, '\0', sizeof(addr.sin_zero)); 
  //Memset to Null Bytes

  // if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
  //   //Bind the Port and Hostname/IP Address Together
  //   cerr << "ERROR: Error Binding the Port and the HostName/IP Address" << 
  //   "Together" << endl;
  //   close(sockfd); //Finally Close the Connection
  //   exit(3);
  // }

  // ------------------------------------------------------------------------ //
  // Set Socket to Listen Status
  // ------------------------------------------------------------------------ //
  if (listen(sockfd, 100) == -1) {
    //Start Listening to the Connections
    cerr << "ERROR: Listen() Failed" << endl;
    close(sockfd); //Finally Close the Connection
    exit(3);
  }
  // ------------------------------------------------------------------------ //
  // Select Function Setup - Do We Really Need This? Since We Are Only Using 1SD
  // ------------------------------------------------------------------------ //
  fd_set active_fd_set;
  fd_set working_fd_set;

  FD_ZERO (&active_fd_set); //Zero Out the Active File Descriptor Set
  FD_SET (sockfd, &active_fd_set); //Add the newSocket to the Working Set

  //----------------------------------------------------------------------------
  int newSocket;
  pthread_t tid[100];
  int i = 1;
  while(1)
  {
      struct sockaddr_in clientAddr; //Place to Store the Client Address
      socklen_t clientAddrSize = sizeof(clientAddr); //Store Client Address Size
      //Accept Call Creates a New Socket for the Incoming Connection

      working_fd_set = active_fd_set; //Determine the Working Set

      int rc = select(sockfd + 1, &working_fd_set, NULL, NULL, NULL);
      if (rc < 0)
      {
          cerr << "ERROR: SELECT() FAILED" << endl;
          close(sockfd); //Finally Close the Connection
          exit(3);
      }

      newSocket = accept(sockfd, (struct sockaddr *) &clientAddr,
       &clientAddrSize);
      //For each Client Request Creates a Thread and Assign the Client Request 
      //to it to Process so the Main Thread can Entertain Next Request
      if (newSocket == -1) { //Error Handling for Bad Connection
        cerr << "ERROR: ACCEPT() Failed" << endl;
        close(newSocket);
        close(sockfd); //Finally Close the Connection
        exit(3);
      }

      char ipstr[INET_ADDRSTRLEN] = {'\0'}; //Set String to NullByte
      inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr,
       sizeof(ipstr));
      //Put Client Information in 'ipstr'
      cerr << "Accept a Connection From: " << ipstr << ":" << \
      ntohs(clientAddr.sin_port) << endl; //State Who Was Accepted

      struct arg_struct arguments;
      arguments.socket = newSocket;
      arguments.num = i;

      if(pthread_create(&tid[i], NULL, socketThread, (void*) &arguments) != 0 ){
        cerr << "Failed to Create Thread!" << endl;
        close(newSocket); //Finally Close the Connection
        close(sockfd); //Finally Close the Connection
      }
      i = i + 1;
      if( i >= 100)
      {
        i = 0;
        while(i < 100)
        {
          pthread_join(tid[i++],NULL);
        }
        i = 0;
      }
  }
  close(sockfd);

  return 0;
}