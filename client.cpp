#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <ctype.h>
#include <fstream>

#include <ctime>


#include <netdb.h>

using namespace std;

#define LENGTH 1024


//METHODS
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int
main(int argc, char *argv[]){

  struct hostent* server = gethostbyname(argv[1]);

  std::clock_t start;
  double duration;
  start = std::clock(); //guessing this starts the timer?

  int portnum = atoi(argv[2]);


  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // BIND ADDRESS TO SOCKET
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(portnum);     // short, network byte order
  serverAddr.sin_addr.s_addr = *(long*)(server->h_addr_list[0]);
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  // connect to the server
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  bool isEnd = false;
  char buff[LENGTH];

  std::ifstream readf(argv[3], std::ios::binary); //reads it as binary into readf

  //so this while loop has to


  while (!isEnd) {
    //make buffer size of the file
    //char buff[sizeof(readf)];
    //std::cout << buff;
    memset(buff, '\0', sizeof(buff));//resets the buffer to null
    //
    readf.read(buff, sizeof(buff));

    //IF NOTHING HAVE BEEN READ IN, THEN WE BREAK OUT OF LOOP
    if (readf.gcount() == 0){
      break;
    }

    //this actually sends the data, the contents in line is getting sent


    if (send(sockfd, buff, sizeof(buff), 0) == -1) {
      perror("send");
      return 4;
    }
  }//end of while loop


  //sets duration in seconds
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<<"printf: "<< duration <<'\n';

  close(sockfd);

  return 0;
}
