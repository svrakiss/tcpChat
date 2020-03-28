  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <pthread.h>
#include <netdb.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>


  #define PORT 4567
  #define BUF_SIZE 256
  #define CLADDR_LEN 100
using boost::asio::ip::tcp;
class Chatter{

    void error(char *msg)
    {
        perror(msg);
        exit(1);
    }
    
    void * receiveMessage(void * socket) {
        int  sockfd,ret;
        char buffer[BUF_SIZE];
        sockfd = (int) socket;

        memset(buffer, 0, BUF_SIZE);
        if (write(sockfd,"I'm waiting for message",23) < 0)
            error("ERROR writing to socket");

        while ((ret = read(sockfd, buffer, BUF_SIZE)) > 0) {
            printf("client: %s", buffer);
        }
        if (ret < 0)
            printf("Error receiving data!\n");
        else
            printf("Closing connection\n");
        close(sockfd);
    }


    

};
