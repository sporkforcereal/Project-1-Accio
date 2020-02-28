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
#include <pthread.h>
#define LENGTH 1024

//METHODS
//checks if the file exists
bool fileExists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str() + 1, &buffer) == 0);
}




pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
char buffy[1024];

//METHOD WE ARE MAKING
void * socketThread(void *arg){//have it accept argv[2]
  int newSocket = *((int *)arg);
  recv(newSocket, buffy, 1024, 0);

  pthread_mutex_lock(&lock);







  pthread_mutex_unlock(&lock);
  close(newSocket);
  pthread_exit(NULL);
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
  addr.sin_addr.s_addr = INADDR_ANY;         //inet_addr("127.0.0.1");
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind");
    return 2;
  }

  // set socket to listen status
  startlisten:
  if (listen(sockfd, 10) == -1) {
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

  //we have to make a directory from argv[2] first, /save
  std::string dirname = argv[2]; //takes in the /save

  //makes a directory name save, not /save
  mkdir(dirname.c_str() + 1, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  int i = 1;
  std::string name = std::to_string(i); // which will be the number
  //starts with 1.file
  std::string halffile = name + ".file";
  //    fullfile =          /save   /       1     .file
  std::string fullfile = dirname + "/" + name + ".file"; //     /save/1.file


  //checks if the file exists and name the next #.file name
  while (1){
    if (!fileExists(fullfile)){
      break;
    }
    else if (fileExists(fullfile)){//IF IT EXISTS ALREADY, INCREMENT THE COUNT AND RECHECK
        i++;
        name = std::to_string(i);
        fullfile = dirname + "/" + name + ".file";
    }
  }
  i++;

  //create the #.file
  std::ofstream writef(fullfile.c_str() + 1, std::ios::binary);

  //this will write out to the file
  while (!isEnd) {
    memset(buff, '\0', sizeof(buff));
    int result = recv(clientSockfd, buff, LENGTH, 0);

    //error checking
    if (result == -1) {
      perror("if result is -1");
      return 5;
    }
    else if (result == 0){
      perror("if result is 0");
      break;
    }

    //checks if buff has null
    bool contains = false;
    int count = 0;
    for (int i = 0; i < 1024; i++){
      if (buff[i] == '\0'){
        std::cout << "THE NULL CHARACTER IS IN HERE!";
        count = i;  //counts has the index where the null starts
        contains = true;
        break;
      }
    }

    //if it contains null, then we write special amount instead
    if (contains){
      char wbuff[count];
      int i = 0;
      for (i = 0; i < count; i++){
        wbuff[i] = buff[i];
        std::cout << wbuff[i];
      }
      writef.write(wbuff, sizeof(wbuff)); //this actually writes it out to the file
    }
    //if it doesnt contain null, we know that we're not end of the file, so we
    //write the full buffer into the file
    else if (!contains){
      writef.write(buff, sizeof(buff));
    }
  }//END OF WHILE LOOP
  goto startlisten;

  close(clientSockfd);
  return 0;
}
