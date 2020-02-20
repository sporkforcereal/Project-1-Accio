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
//SAMEPLE CODE
// send/receive data to/from connection
  bool isEnd = false;
  std::string input;
  char buf[LENGTH] = {0};
  std::stringstream ss;
  std::ifstream read(argv[3]);
  string line;


  while (!isEnd) {
    memset(buf, '\0', sizeof(buf));

  //  std::cout << "send: ";
  //  std::cin >> input;

    getline(read, line); //read is the argv, and puts it the the line



    //this actually sends the data, the contents in line is getting sent
    if (send(sockfd, line.c_str(), line.size(), 0) == -1) {
      perror("send");
      return 4;
    }
    if (recv(sockfd, buf, LENGTH, 0) == -1) {
      perror("recv");
      return 5;
    }

    //ss << buf << std::endl;
    //std::cout << "echo: ";
    //std::cout << buf << std::endl;

    if (ss.str() == "close\n")
      break;

    ss.str("");
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


  //close the socket
  close(sockfd);

  return 0;
}
