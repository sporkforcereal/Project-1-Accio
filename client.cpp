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

using namespace std;

#define LENGTH 1024

int
main(int argc, char *argv[])
  //argc counts so ./client asdf will return 2
  //argv starts 0 1 2

{
  int portnum = atoi(argv[2]);
  // create a socket using TCP IP
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

  // BIND ADDRESS TO SOCKET
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(portnum);     // short, network byte order
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
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


//before it reaches here, it sets upt the connection
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// send/receive data to/from connection
  bool isEnd = false;
  char buff[LENGTH];
  std::ifstream readf(argv[3], std::ios::binary); //reads it as binary into readf

  while (!isEnd) {
    memset(buff, '\0', sizeof(buff));//resets the buffer to null

  //readf is the argv, and puts it the the line
  //instead of get line, we read in 1024 bytes in binary, and put it into the
  //buffer, then we can send the data in the buffer.


//1, load the buffer with 1024 bytes of data, and also keep track of where we are
    readf.read(buff, sizeof(buff));


//2, send the buffer here

//IF NOTHING HAVE BEEN READ IN, THEN WE BREAK OUT OF LOOP
    if (readf.gcount() == 0){
      break;
    }



    //this actually sends the data, the contents in line is getting sent
    if (send(sockfd, buff, sizeof(buff), 0) == -1) {
      perror("send");
      return 4;
    }

    if (recv(sockfd, buff, LENGTH, 0) == -1) {
      perror("recv");
      return 5;
    }

  }//END OF WHILE LOOP

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  close(sockfd);

  return 0;
}
