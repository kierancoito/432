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
#include <sys/time.h>
#include <pthread.h>
using namespace std;
/**
 * Created by Kieran Coito
 * CSS 432
 *
 */



int port, repetition, serverSD, newSD;
const int bufSize = 1500;
const int connection = 5;
int uniCount = 0;

//struct to hold thread data for pthread
struct thread_data{
    int sd;
};

/*
 * Verify port number and arguments from user
 */
bool verifyArgs(int argumentNum, char *argument[]){
    if (argumentNum != 2){
        cerr << "Invalid number of arguments. " << argumentNum << " was how many you entered. This requires 3" << endl;
        return false;
    }
    try{
        port = stoi(argument[1]);

    } catch(invalid_argument& iaException){
        cerr << iaException.what() << endl;
        return false;

    } catch(exception e){
        cerr << e.what() << endl;
        return false;

    }
    if (port != 80 && (port < 1024 || port > 49152)){
        cerr << "Bad port number" << endl;
        return false;
    }

    return true;
}

/*
 * Method used to execute the different type scenarios in a p thread
 *
 * Parameter
 *      threadData - the data about socket connection and repetitions
 */
void * thread_server(void *threadData){
    //increment count everytime this function is called
    //while the instance of the server is running
    uniCount++;
    //create data buffer
    char databuf[bufSize];
    //extract data about connection and repetition
    struct thread_data *data;
    data = (struct thread_data *) threadData;
    int count = 0;


    //go through all data
    for ( int i = 0; i <= repetition; i++ ){
        for ( int numberRead = 0; (numberRead += read(data->sd , databuf , bufSize - numberRead)) < bufSize; ++count );
        count++;
    }

    //write back to client
    write(data->sd , &count , sizeof(count));

    // finish with this client. close it
    close(data->sd);
}

/*
 * Main function that will handle the majority of the server application work
 */
int main(int argumentNum, char *argument[]){
    //verify all input from the user is valid

    //
    // NEEDS ONE ARG PORT NUM
    //
    if (!verifyArgs(argumentNum, argument)){
        cout << "Arguments were invalid." << endl;
        return -1;
    }

    //create data structures for socket connection as specified in lecture slides
    sockaddr_in acceptSocketAddr;
    bzero((char *)& acceptSocketAddr, sizeof(acceptSocketAddr));
    acceptSocketAddr.sin_family = AF_INET;
    acceptSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    acceptSocketAddr.sin_port = htons(port);

    // Open socket and bind
    serverSD = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM = TCP, 0 = IP
    const int on = 1;
    setsockopt(serverSD, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int));
    bind(serverSD, (sockaddr*)&acceptSocketAddr, sizeof(acceptSocketAddr));

    // Listen and accept
    listen(serverSD, connection);
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);

    //run forever
    while(true){
        newSD = accept(serverSD, (sockaddr *)&newSockAddr, &newSockAddrSize);

        //create new thread and run thread_server on that thread with the information received from the client
        pthread_t new_thread;
        struct thread_data *data;

        //save data into thread_data object from socket that will be used in thread_server
        data = new thread_data();
        data->sd = newSD;

        int iret1 = pthread_create( &new_thread, NULL, thread_server, (void*) data );
    }
}
