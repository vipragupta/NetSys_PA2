#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>



#define MAXLINE 4096 /*max text line length*/
//#define SERV_PORT 8884/*port*/
#define LISTENQ 8 /*maximum number of client connections*/


FILE *getFilePointer(char filename[])
{
    FILE *file = NULL;
    //char filePath[100];
    //memset(filePath, '\0', sizeof(filePath));
    //strcat(filePath, "./www/");
    //strcat(filePath, filename)  ;

    file = fopen(filename, "r");
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
        char *tok = strtok(data, "\"");
        if (tok != NULL) {
          char *temp = strtok(NULL, "\"");
          strcpy(root, temp);
          root[strlen(temp)] = '\0';
          break;
        }
      }
    }
  }
  fclose(file);
}

void getUrl(char *url, char *request) {
  int i = 0;

  char *tok = strtok(request, " ");
  if (tok != NULL) {
    char *temp = strtok(NULL, " ");
    printf("tok URL:%s\n", tok);
    printf("temp URL:%s\n", temp);
    strcpy(url, temp);
    url[strlen(temp)] = '\0';
  }
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
          char *temp = strtok(NULL, "\n");
          //printf("i:%d    ext:%s   type:%s\n", i, tok, temp);
          if (i == 8) {
            strcpy(address, tok);
          } else {
            strcat(address, tok);
          }
          strcat(address, ":");
          strcat(address, temp);
          strcat(address, ",");
        }
      }
    }
    address[strlen(address)] = '\0';
  }
  
  fclose(file); 
}

void getFiveOOneResponse(char *responseBuffer, char *requestUrl) {
  bzero(responseBuffer, sizeof(responseBuffer));
  strcpy(responseBuffer, "HTTP/1.1 501 Not Implemented\r\n");
  strcat(responseBuffer, "Content-Type: text/html \r\n");
  strcat(responseBuffer,"Content-Length:100");
  strcat(responseBuffer, "\r\n\r\n");
  strcat(responseBuffer,"<html><body>501 Not Implemented  <br/><b>Url: ");
  strcat(responseBuffer, requestUrl);
  strcat(responseBuffer, "</b></body></html>");
}

void getFourOFourResponse(char *responseBuffer, char *requestUrl) {
  bzero(responseBuffer, sizeof(responseBuffer));
  strcpy(responseBuffer, "HTTP/1.1 404 Not Found\r\n");
  strcat(responseBuffer, "Content-Type: text/html \r\n");
  strcat(responseBuffer,"Content-Length:100");
  strcat(responseBuffer, "\r\n\r\n");
  strcat(responseBuffer,"<html><body>404 Not Found Reason URL doesn not exist<br/><b>Url: ");
  strcat(responseBuffer, requestUrl);
  strcat(responseBuffer, "</b></body></html>");
}

void getFiveHundreadResponse(char *responseBuffer, char *requestUrl) {
  bzero(responseBuffer, sizeof(responseBuffer));
  strcpy(responseBuffer, "HTTP/1.1 500 Internal Server Error\r\n");
  strcat(responseBuffer, "Content-Type: text/html \r\n");
  strcat(responseBuffer,"Content-Length:100");
  strcat(responseBuffer, "\r\n\r\n");
  strcat(responseBuffer,"<html><body>500 Internal Server Error: cannot allocate memory<br/><b>Url: ");
  strcat(responseBuffer, requestUrl);
  strcat(responseBuffer, "</b></body></html>");
}

int isGetFile(char *url) {
  char *tok = strrchr(url, '.');
  if (tok == NULL) {
    return 0;
  }
  return 1;
}

int isValidFileType(char *requestUrl, char *types) {
  printf("types: %s\n", types);
  printf("requestUrl: %s\n", requestUrl);
  char *fileType = strrchr(requestUrl, '.');
  //printf("fileType:\n", fileType);
  char *typePresent = strstr(types, fileType);
  
  if (typePresent != NULL) {
    printf("RETURN 1 fileType:%s    typePresent: %s\n", fileType, typePresent);
    return 1;
  } else {
    printf("RETURN 0 fileType:%s    typePresent: %s\n", fileType, typePresent);
    return 0;
  }
}

int isImageTypeFile(char *requestUrl) {
  printf("requestUrl: %s\n", requestUrl);
  char *fileType = strrchr(requestUrl, '.');
  if (fileType != NULL && ((strcmp(fileType, ".txt") == 0) || (strcmp(fileType, ".html") == 0) || (strcmp(fileType, ".htm") == 0))) {
    return 0;
  } else {
    return 1;
  }
}


int main (int argc, char **argv)
{

  int listenfd, connfd, n;
  pid_t childpid;
  socklen_t clientLength;
  char request[MAXLINE];
  struct sockaddr_in clientSocket, serverSocket; 
  char rootAddress[200];
  char defaultFileName[10];
  char fileType[150];
  
  int port = getPortNumber();
  getDefaultFileName(defaultFileName);  
  getHandledContentType(fileType);
  getRootDirectory(rootAddress);

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
        printf("*****************************************************************\n");
        printf("Request received from client\n\n");
        puts(request);
        char *httpRequest;
        char requestMethod[10];
        char requestUrl[70];
        char requestVersion[10];
        char urlByToken[70];

        httpRequest = strtok(request, "\n");
        //printf("%s\n", httpRequest);
        int i = 0 ;
        int j = 0;
        int arg = 0;
        char temp[30];

        while (httpRequest[i] != '\0') {
          if (httpRequest[i] == ' ') {
            if (arg == 0) {
              bzero(requestMethod, sizeof(requestMethod));
              strcpy(requestMethod, temp);
            } else if (arg == 1) {
              printf("temp:%s    len:%lu\n", temp, strlen(temp));
              bzero(requestUrl, sizeof(requestUrl));
              strcpy(requestUrl, temp);
              requestUrl[strlen(temp)] = '\0';
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

        getUrl(urlByToken, httpRequest);

        char responseBuffer[1048576];
        int responseIndex = 0;
        char fileBuffer[1048576];

        if (strcmp(requestMethod, "GET") == 0) {
          int isSendImage = 0;
          FILE *file;
          char filename[50];
          memset(filename, '\0', sizeof(filename));

          int isFile = isGetFile(requestUrl);
          printf("requestUrl:%s\n", requestUrl);
          printf("Url:%s\n", urlByToken);
          printf("\nISFILE::::%d\n", isFile);
          if (isFile == 1) {
           
            int validFileType = isValidFileType(requestUrl, fileType);
            printf("ISVALIDFILETYPE::::%d\n", validFileType);
            if (validFileType == 1) {
              int isImageType = isImageTypeFile(requestUrl);
              printf("ISIMAGETYPE::::%d\n", isImageType);
                if (isImageType == 0) {
                  printf("rootAddress::::%sss\n", rootAddress);
                  strcpy(filename, rootAddress);
                  strcat(filename, requestUrl);
                  printf("FILENAME:::%s\n", filename);
                  file = getFilePointer(filename);

                  if(file == NULL)
                  {
                     printf("file does not exist\n");
                     bzero(responseBuffer, sizeof(responseBuffer));
                     getFourOFourResponse(responseBuffer, requestUrl);
                     printf("\nResponse:\n%s\n\n", responseBuffer);
                     send(connfd, responseBuffer, sizeof(responseBuffer), 0);                     
                  } else {
                    bzero(responseBuffer, sizeof(responseBuffer));
                    strcpy(responseBuffer, "HTTP/1.1 200 OK\r\n");
                    strcat(responseBuffer, "Content-Type: text/plain \r\n");  //TODO
                    strcat(responseBuffer,"Content-Length:");
                    size_t file_size = getFileSize(file);     //Tells the file size in bytes.
                    fseek(file, 0, SEEK_SET);
                    int byte_read = fread(fileBuffer, 1, file_size, file);
                    
                    if(byte_read <= 0)
                    {
                      printf("unable to copy file into buffer\n");
                      bzero(responseBuffer, sizeof(responseBuffer));
                      getFiveHundreadResponse(responseBuffer, requestUrl);
                      printf("\nResponse:\n%s\n\n", responseBuffer);
                      send(connfd, responseBuffer, sizeof(responseBuffer), 0);
                      continue;
                    }
                    
                    char byteStr[5];
                    sprintf(byteStr, "%d", byte_read);
                    strcat(responseBuffer, byteStr);
                    strcat(responseBuffer, "\r\n\r\n");
                    strcat(responseBuffer, fileBuffer);
                    strcat(responseBuffer, "\r\n");
                    printf("\nResponse:\n%s\n\n", responseBuffer);
                    send(connfd, responseBuffer, sizeof(responseBuffer), 0);
                  }
                } else {  // This is when we need to send a image i.e. binary data.
                  //Reference: https://stackoverflow.com/questions/28631767/sending-images-over-http-to-browser-in-c
                  printf("Inside Send Image\n");
                  struct stat filestat;
                  FILE *fp;
                  int fd;
                  char header_buff [1048576];
                  char file_buff [1048576];
                  char filesize[10];

                  strcpy(filename, rootAddress);
                  strcat(filename, requestUrl);
                  printf("FILENAME:::%s\n", filename);
                  

                  if ( ((fd = open (filename, O_RDONLY)) < -1) || (fstat(fd, &filestat) < 0) ) {
                    printf ("Error in measuring the size of the file");
                    bzero(responseBuffer, sizeof(responseBuffer));
                    getFourOFourResponse(responseBuffer, requestUrl);
                    printf("\nResponse:\n%s\n\n", responseBuffer);
                    send(connfd, responseBuffer, sizeof(responseBuffer), 0);
                  }                  
                  sprintf (filesize, "%zd", filestat.st_size);
                  fp = fopen (filename, "r");
                  
                  if (fp == NULL) {
                      printf("file does not exist\n");
                      bzero(responseBuffer, sizeof(responseBuffer));
                      getFourOFourResponse(responseBuffer, requestUrl);
                      printf("\nResponse:\n%s\n\n", responseBuffer);
                      send(connfd, responseBuffer, sizeof(responseBuffer), 0);
                  } else {
                    strcpy(responseBuffer, "HTTP/1.1 200 OK\r\n");
                    strcat(responseBuffer, "Content-Type: image/jpg \r\n");  //TODO: Make a function and remove hardcoded.
                    strcat(responseBuffer,"Content-Length:");
                    strcat(responseBuffer, filesize);
                    strcat(responseBuffer, "\r\n");
                    strcat(responseBuffer, "Connection: keep-alive\r\n\r\n");

                    //Send header content first.
                    write (connfd, responseBuffer, strlen(responseBuffer));
                    fread (file_buff, sizeof(char), filestat.st_size + 1, fp);
                    fclose(fp);
                    //Send File data.
                    write (connfd, file_buff, filestat.st_size);
                  }
                }
            } else { //Not a valid File Type. Send 501

              bzero(responseBuffer, sizeof(responseBuffer));
              getFiveOOneResponse(responseBuffer, requestUrl);
              printf("\nResponse:\n%s\n\n", responseBuffer);
              send(connfd, responseBuffer, sizeof(responseBuffer), 0);
            }

          } else { //Given url doesn't have a specified file name, so send index.html
            printf("Inside ELSE ISFILE\n");
            strcpy(filename, rootAddress);
            strcat(filename, "/index.html");
            printf("FILENAME:::%s\n", filename);
            file = getFilePointer(filename);
            bzero(responseBuffer, sizeof(responseBuffer));

            if(file == NULL)
            {
                printf("file does not exist\n");
                bzero(responseBuffer, sizeof(responseBuffer));
                getFourOFourResponse(responseBuffer, requestUrl);
                printf("\nResponse:\n%s\n\n", responseBuffer);
                send(connfd, responseBuffer, sizeof(responseBuffer), 0);
                
            } else {
              strcpy(responseBuffer, "HTTP/1.1 200 OK\r\n");
              strcat(responseBuffer, "Content-Type: text/html \r\n");  //TODO
              strcat(responseBuffer,"Content-Length:");
              size_t file_size = getFileSize(file);     //Tells the file size in bytes.
              fseek(file, 0, SEEK_SET);
              int byte_read = fread(fileBuffer, 1, file_size, file);
              
              if(byte_read <= 0)
              {
                printf("unable to copy file into buffer\n");
                bzero(responseBuffer, sizeof(responseBuffer));
                getFiveHundreadResponse(responseBuffer, requestUrl);
                printf("\nResponse:\n%s\n\n", responseBuffer);
                send(connfd, responseBuffer, sizeof(responseBuffer), 0);
                continue;
              }
              
              char byteStr[5];
              sprintf(byteStr, "%d", byte_read);
              strcat(responseBuffer, byteStr);
              strcat(responseBuffer, "\r\n\r\n");
              strcat(responseBuffer, fileBuffer);
              strcat(responseBuffer, "\r\n");
              printf("\nResponse:\n%s\n\n", responseBuffer);
              send(connfd, responseBuffer, sizeof(responseBuffer), 0);
            }
          }
        } else {
          bzero(responseBuffer, sizeof(responseBuffer));
          getFiveOOneResponse(responseBuffer, requestUrl);
          printf("\nResponse:\n%s\n\n", responseBuffer);
          send(connfd, responseBuffer, sizeof(responseBuffer), 0);
        }
        close(connfd);
        }
      close(connfd);
      exit(0);
    }
    //close socket of the server
    close(connfd);
  }
}