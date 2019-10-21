#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <string.h>       // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR

#include <stdlib.h>       // EXIT_FAILURE
#include <sys/uio.h>      // writev
#include <sys/time.h>     // timeval
#include <string>
#include <iostream>
#include <exception>
using namespace std;

/**
 * Created by Kieran Coito
 * CSS 432
 * October 11th 2019
 *
 *
 */



int port, repetition, nbufs, bufsize, type;
char *serverIp;

/*
 * Takes in the user arguments and verifies that they are all valid
 */
bool verifyArgs(int argumentNum, char *argument[]) {

    if (argumentNum != 7) {
        cout << "Need 7 args. Instead got " << argumentNum << " args." << endl;
        return false;
    }

    try {
        port = stoi(argument[1]);
        repetition = stoi(argument[2]);
        nbufs = stoi(argument[3]);
        bufsize = stoi(argument[4]);
        serverIp = argument[5];
        type = stoi(argument[6]);

    } catch (invalid_argument ) {
        cout << "One of your arguments was incorrect please check over and try again" << endl;
        return false;
    } catch (exception ) {
        cout << "One of your arguments was incorrect please check over and try again" << endl;
        return false;
    }

    //ensure valid port
    if (port > 49152 || port < 1024) {
        cout << "Port number invalid: " << port << endl;
        return false;
    }

    //ensure total buffer doesn't exceed 1500
    if (nbufs * bufsize != 1500) {
        cout << "Number of buffers * buffer size != 1500" << endl;
        cout << "Number of buffers: " << nbufs << ", ";
        cout << "Buffer size: " << bufsize << "." << endl;
        return false;
    }
    //ensure valid number of repitions
    if (repetition < 0) {
        cout << "Number of times repeated is negative!" << endl;
        return false;
    }
    //ensure tpe is correct
    if (type != 1 && type != 2 && type != 3) {
        cout << " Type must be 1, 2, or 3. You entered " << type  << endl;
        return false;
    }
    return true;
}

int main(int argumentNum, char *argument[]) {

    //
    // ARGS NEEDED Server name (website name), file path(extension) and port number
    //

    if (!verifyArgs(argumentNum, argument)) {
        cout << "Arguments were invalid." << endl;
        return -1;
    }

    char databuf[nbufs][bufsize];

    // set up data structure and connect to port
    // as specified in lecture slides
    struct hostent* host = gethostbyname(serverIp);
    sockaddr_in sendSockAddr;
    bzero( (char *)&sendSockAddr, sizeof(sendSockAddr));

    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);

    int clientSD = socket(AF_INET, SOCK_STREAM, 0);

    int connectStatus = connect( clientSD, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
    if (connectStatus < 0) {
        cerr << "Connection failed!" << endl;
        close(clientSD);
        return -1;
    }

    int numberOfRead;

    //read back from server to confirm writing was finished
    read(clientSD, &numberOfRead, sizeof(numberOfRead));

    close(clientSD);
    return 0;

}
