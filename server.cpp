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

//METHODS
bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}


//using namespace std;
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


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  //read/write data from/into the connection
  bool isEnd = false;
  char buff[LENGTH];

//we have to make a directory from argv[2] first
  std::string dirname = argv[2];

  mkdir(dirname.c_str() + 1, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //makes a directory from the command line

//check if file exists or not, so start with 1, 2, .... if it dont exist, then make it
//so check if it exists first
  int i = 1;
  bool stop = true;
  std::string name = std::to_string(i); // which will be the number

  std::string fullfile = dirname + "/" + name + ".file"; //makes it 1.file 2.file......

  //std::ofstream *writef = nullptr;
  while (stop){
    if (!fileExists(fullfile)){//if the file doesnt exist, make that file, now we have to put that into the while loop
      std::ofstream writef(fullfile.c_str() + 1, std::ios::binary);
      stop = false;
    }
    else{
        i++;
        name = std::to_string(i);
        fullfile = name + ".file";
    }
  }
  i = 0;

//how we have to put the directory from of the receive file.
  std::ofstream writef(fullfile.c_str() + 1, std::ios::binary);



  while (!isEnd) {
    //bzero(buf, LENGTH);
    memset(buff, '\0', sizeof(buff)); //OVER HERE! IT KEEPS PUTTING \0 AFTER WE'RE DONE.
                                    //SO WE HAVE TO SOMEHOW STOP IT FROM HAPPENING
                                    //returns 0 if clientj's socket closes
    int result = recv(clientSockfd, buff, LENGTH, 0);
    if (result == -1) {
      perror("recv");
      return 5;
    }
    if (result == 0){
      break;
    }

    writef.write(buff, sizeof(buff)); //seems like it works

/*
    //IF I COMMENT THIS OUT IT KEEPS LOOPING AND PUTS \0 AT THE END
    if (send(clientSockfd, buff, LENGTH, 0) == -1) {
      perror("send");
      return 6;
    }
*/

  }//END OF WHILE LOOP
/*
  (*writef).close();
  delete writef;
*/
  close(clientSockfd);

  return 0;
}
