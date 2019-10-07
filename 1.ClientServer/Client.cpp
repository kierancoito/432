// Created by Kieran Coito on 2019-10-05.
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <iostream>

using namespace std;

int main(int argumentNum, char *argument[]){
    /*
     * port: server's port number
     * repetition: the number of iterations a client performs on data transmission using "single write," "writev" or "multiple writes."
     * nbufs: the number of data buffers
     * bufsize: the size of each data buffer (in bytes)
     * serverIp: server's IP address
     * type: the type of transfer scenario: 1, 2, or 3 (see below)
     */

    if(argumentNum != 6){
        cout << "Incorrect number of arguments, please try again with the proper number, which is 6" << endl;
    }

    int port = (int) *argument[0];
    int repetition = (int) *argument[1];
    int nbufs = (int) *argument[2];
    int bufsize = (int) *argument[3];
    int serverIp = (int) *argument[4];
    int type = (int) *argument[5];

    if(nbufs * bufsize != 1500){
        cout << "Either nbufs or bufsize are the wrong size. nbufs * bufize must equal 1500" << endl;
    }
    char databuf[nbufs][bufsize];

    //2050 for port
    int clientSocker = socket(AF_INET ,SOCK_STREAM, 0);


}
