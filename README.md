# NetSys_PA2
To create a HTTP-based web server that handles multiple simultaneous requests from users.

The server is called webserver.c
Running the server:
```
gcc webserver.c -o webserver.o
./webserver.o
```


## Functionalities
1. If only the server ip and port number is given, e.g. 127.0.0.1:8080, then the index.html file is rendered.
2. If a particular file is given in the url, then the path is checked, if its present, then the file is rendered.

#### ws.conf file is used for all he configurations like port number of server, root directory, default file and type of files server handles.

#### Server handles requests with Request method - GET and POST, Request Version - HTTP/1.1 and HTTP/1.0

## Error Codes

400: Request message is in wrong format like Http version is not supported, url is in wrong format.
404: The requested url doesn't exist.
501: Requested file type is not supported or Request method is not supported.
500: All other Server side errors like error during file read etc.

## Sample request 

```
GET /images/welcome.png HTTP/1.1
Host: 127.0.0.1:8880
Connection: keep-alive
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36
Accept: image/webp,image/apng,image/*,*/*;q=0.8
DNT: 1
Referer: http://127.0.0.1:8880/
Accept-Encoding: gzip, deflate, br
Accept-Language: en-US,en;q=0.8
```

## Sample Response

```
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length:41

This is a text File. Welcome to My Server
```