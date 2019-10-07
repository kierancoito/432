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
     * repetition: the repetition of client's data transmission activities. This value should be the same as the "repetition" value used by the client. It means the number of iterations a server shall perform on "read" from the socket. If a client performs "repetition of 100" data transmissions, then the server shall perform at least "repetition of 100" reads from the socket to completely receive all data sent by the client.
     * Therefore, the value "repetition" shall be the same for both client and server to use.
     *
     */
    if(argumentNum != 2){
        cout << "Incorrect number of arguments, please try again with the proper number, which is 6" << endl;
    }

    int port = (int) *argument[0];
    int repetition = (int) *argument[1];

    int serverSocket = socket(AF_INET ,SOCK_STREAM, 0);


}
