#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#define LENGTH 1024

//using namespace std;
int
main(int argc, char *argv[])
{
  int portnum = atoi(argv[1]);
  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // allow others to reuse the address
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    return 1;
  }

  // bind address to socket
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(portnum);     // short, network byte order
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind");
    return 2;
  }

  // set socket to listen status
  if (listen(sockfd, 1) == -1) {
    perror("listen");
    return 3;
  }

  // accept a new connection
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


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  //read/write data from/into the connection
  bool isEnd = false;
  char buff[LENGTH];
  std::ofstream writef("receive.txt", std::ios::binary);

  while (!isEnd) {
    //bzero(buf, LENGTH);
    memset(buff, '\0', sizeof(buff)); //OVER HERE! IT KEEPS PUTTING \0 AFTER WE'RE DONE.
                                    //SO WE HAVE TO SOMEHOW STOP IT FROM HAPPENING

    if (recv(clientSockfd, buff, LENGTH, 0) == -1) {
      perror("recv");
      return 5;
    }

    //i made this line below to match the client and it somehow works?
    //but the issue is that,
    writef.write(buff, sizeof(buff));
    //writef << buf << std::endl; //WRITES IT TO THE RECEIVE.TXT

    //IF I COMMENT THIS OUT IT KEEPS LOOPING AND PUTS \0 AT THE END
    if (send(clientSockfd, buff, LENGTH, 0) == -1) {
      perror("send");
      return 6;
    }

  }//END OF WHILE LOOP

  close(clientSockfd);

  return 0;
}
