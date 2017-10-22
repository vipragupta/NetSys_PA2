#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>



#define MAXLINE 4096 /*max text line length*/
//#define SERV_PORT 8884/*port*/
#define LISTENQ 8 /*maximum number of client connections*/


FILE *getFilePointer(char filename[])
{
    FILE *file = NULL;
    char filePath[50];
    memset(filePath, '\0', sizeof(filePath));
    strcat(filePath, "./www/");
    strcat(filePath, filename)  ;

    file = fopen(filePath, "r");
    if (file) {
        printf("File %s opened \n\n", filename);
        return file;
    }
    return NULL; // error
}


size_t getFileSize(FILE *file) {
  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  return file_size;
}

int getPortNumber() {
  FILE *file;
  file = fopen("ws.conf", "r");
  char data[1048576];
  int i = 0;
  char port[4];
  if (file) {
    while(fgets(data, sizeof(data), file)) {
      i++;
      if (i == 2) {
        int j = 0;
        while (data[j] != ' ') {
          j++;
        }
        j++;
        int index = 0;
        while (data[j] != '\n') {
          port[index++] = data[j++];
        }
        break;
      }
    }
  }
  
  fclose(file); 
  return atoi(port);
}

void getRootDirectory(char *root) {
  FILE *file;
  file = fopen("ws.conf", "r");
  char data[1048576];
  int i = 0;
  
  if (file) {
    while(fgets(data, sizeof(data), file)) {
      i++;
      
      if (i == 4) {
        char *tok = strtok(data, " ");
        if (tok != NULL) {
          char *temp = strtok(NULL, " ");
          strcpy(root, temp);
          root[strlen(temp)-1] = '\0';
        }
        break;
      }
    }
  }
  
  fclose(file); 
}

void getDefaultFileName(char *address) {
  FILE *file;
  file = fopen("ws.conf", "r");
  char data[1048576];
  int i = 0;

  if (file) {
    while(fgets(data, sizeof(data), file)) {
      i++;
      
      if (i == 6) {
        char *tok = strtok(data, " ");
        if (tok != NULL) {
          char *temp = strtok(NULL, "\n");
          strcpy(address, temp);
          address[strlen(temp)] = '\0';
        }
        break;
      }
    }
  }
  fclose(file); 
}

void getHandledContentType(char *address) {
  FILE *file;
  file = fopen("ws.conf", "r");
  char data[1048576];
  int i = 0;

  if (file) {
    while(fgets(data, sizeof(data), file)) {
      i++;
      
      if (i > 7) {
        char *tok = strtok(data, " ");
        if (tok != NULL) {
          char *temp = strtok(data, " ");
          //printf("tok:%s     temp:%s\n", tok, temp);
          if (i == 8) {
            strcpy(address, temp);
          } else {
            strcat(address, temp);
          }
          strcat(address, ",");
        }
      }
    }
    address[strlen(address)] = '\0';
  }
  
  fclose(file); 
}


int main (int argc, char **argv)
{

  int listenfd, connfd, n;
  pid_t childpid;
  socklen_t clientLength;
  char request[MAXLINE];
  struct sockaddr_in clientSocket, serverSocket;

  int port = getPortNumber();
  
  char rootAddress[150];
  getRootDirectory(&rootAddress);
  
  char defaultFileName[10];
  getDefaultFileName(&defaultFileName);
  
  char fileType[100];
  getHandledContentType(&fileType);

  printf("Port:%d:\n", port);
  printf("rootAddress:%s:\n", rootAddress);
  printf("defaultFileName:%s:\n", defaultFileName);
  printf("Type:%s:\n\n", fileType);

  //Create a socket for the soclet
  //If sockfd<0 there was an error in the creation of the socket
  if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
    perror("Problem in creating the socket");
    exit(2);
  }

  //preparation of the socket address
  serverSocket.sin_family = AF_INET;
  serverSocket.sin_addr.s_addr = htonl(INADDR_ANY);
  serverSocket.sin_port = htons(port);

  //bind the socket
  bind (listenfd, (struct sockaddr *) &serverSocket, sizeof(serverSocket));

  //listen to the socket by creating a connection queue, then wait for clients
  listen (listenfd, LISTENQ);

  printf("Server running...waiting for connections at port:%d\n", port);

  while (1) {

    clientLength = sizeof(clientSocket);
    //accept a connection
    connfd = accept (listenfd, (struct sockaddr *) &clientSocket, &clientLength);

    printf("\n%s\n","Received request...");

    if ((childpid = fork ()) == 0 ) {//if it’s 0, it’s child process

      printf ("%s\n","Child created for dealing with client requests");

      //close listening socket
      close(listenfd);

      while ((n = recv(connfd, request, MAXLINE,0)) > 0)  {
        printf("%s\n\n","String received from and resent to the client: ");
        puts(request);
        char *httpRequest;
        char requestMethod[10];
        char requestUrl[20];
        char requestVersion[10];

        httpRequest = strtok(request, "\n");
        printf("%s\n", httpRequest);
        int i = 0 ;
        int j = 0;
        int arg = 0;
        char temp[10];

        while (httpRequest[i] != '\0') {
          if (httpRequest[i] == ' ') {
            if (arg == 0) {
              bzero(requestMethod, sizeof(requestMethod));
              strcpy(requestMethod, temp);
            } else if (arg == 1) {
              strcpy(requestUrl, temp);
            }
            bzero(temp, sizeof(temp));
            j = 0;
            arg++;
          } else {
            temp[j] = httpRequest[i];
            j++;
          }
          i++;
        }
        bzero(requestVersion, sizeof(requestVersion));
        memset(requestVersion, '\0', sizeof(requestVersion));
        strcpy(requestVersion, temp);

        char responseBuffer[1048576];
        int responseIndex = 0;
        char fileBuffer[1048576];

        FILE *file;
        char filename[50];
        memset(filename, '\0', sizeof(filename));
        strcpy(filename, "text.txt");
        file = getFilePointer(filename);
        bzero(responseBuffer, sizeof(responseBuffer));

        strcpy(responseBuffer, "HTTP/1.1 200 OK\r\n");
        strcat(responseBuffer, "Content-Type: text/plain \r\n");  //TODO
        strcat(responseBuffer,"Content-Length:");

        if(file == NULL)
        {
           printf("file does not exist\n");
        } else {

          size_t file_size = getFileSize(file);     //Tells the file size in bytes.
          //printf("file_size: %lu\n", file_size);
          fseek(file, 0, SEEK_SET);
          int byte_read = fread(fileBuffer, 1, file_size, file);
          //printf("fileBuffer:\n%s\n", fileBuffer);
          
          if(byte_read <= 0)
          {
            printf("unable to copy file into buffer\n");
            exit(0);
          }
          
          char byteStr[5];
          sprintf(byteStr, "%d", byte_read);
          strcat(responseBuffer, byteStr);
          strcat(responseBuffer, "\r\n\r\n");
          strcat(responseBuffer, fileBuffer);
          strcat(responseBuffer, "\r\n");
          printf("\nResponse:\n%s\n\n", responseBuffer);
          //write(connfd, responseBuffer, sizeof(responseBuffer) -1);
          send(connfd, responseBuffer, sizeof(responseBuffer), 0);
          close(connfd);
        }
        
      }

      if (n < 0)
      {
        printf("%s\n", "Read error");
        printf("%d\n", n);
        printf("%s\n", request);

      }
      close(connfd);
      exit(0);
    }
    //close socket of the server
    close(connfd);
  }
}