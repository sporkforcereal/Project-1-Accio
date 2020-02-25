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
#include <string>
#include <sys/stat.h>
#define LENGTH 1024
#include <thread>


//METHODS
//checks if the file exists
bool fileExists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str() + 1, &buffer) == 0);
}


int main(int argc, char *argv[])//argv[1] is for port argv[2] is for file-dir we're trying to save at
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



  //read/write data from/into the connection
  bool isEnd = false;
  char buff[LENGTH];

  //we have to make a directory from argv[2] first, /save
  std::string dirname = argv[2]; //takes in the /save

  //makes a directory name save, not /save
  mkdir(dirname.c_str() + 1, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  int i = 1;
  bool stop = true;

  std::string name = std::to_string(i); // which will be the number

  //starts with 1.file
  std::string halffile = name + ".file";
  //    fullfile =          /save   /       1     .file
  std::string fullfile = dirname + "/" + name + ".file"; //     /save/1.file

  //std::ofstream writef;
  while (stop){
    if (!fileExists(fullfile)){//FOR SOME REASON IT DOESNT SEE THE EXIST
      perror("IT DOESNT EXISTS");
      //std::ofstream writef(fullfile.c_str() + 1, std::ios::binary); //creats the file
      break;
    }
    else if (fileExists(fullfile)){//IF IT EXISTS ALREADY, INCREMENT THE COUNT AND RECHECK
        perror("in da while loop!");
        i++;
        name = std::to_string(i);
        fullfile = dirname + "/" + name + ".file";
    }
  }
  i++;

  //makes the file to write our binary data into, writef
  std::ofstream writef(fullfile.c_str() + 1, std::ios::binary);


  //char buf[LENGTH];

  while (!isEnd) {

    memset(buff, '\0', sizeof(buff));



    int result = recv(clientSockfd, buff, LENGTH, 0);

    //char buff[result];
    //recv(clientSockfd, buff, sizeof(buff), 0);


    if (result == -1) {
      perror("if result is -1");
      return 5;
    }

    else if (result == 0){
      perror("if result is 0");
      break;
    }

    //std::cout << result;

    else{
      writef.write(buff, sizeof(buff)); //this actually writes it out to the file
    }//so after we get here, we have to keep it open and listen for new commands

  }//END OF WHILE LOOP

  //if 10 seconds passed after the last transfer, then close
  close(clientSockfd);

  return 0;
}
