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
#include <string.h>

using namespace std;
/**
 * Created by Kieran Coito
 * CSS 432
 *
 */
int port, serverSD, newSD;
const int connection = 5;
int uniCount = 0;

//struct to hold thread data for pthread
struct thread_data {
    int sd;
};

/**
 *
 * @param argumentNum
 * @param argument
 * @return
 */
bool verifyArgs(int argumentNum, char *argument[]) {
    if (argumentNum != 2) {
        cerr << "Invalid number of arguments. " << argumentNum << " was how many you entered. This requires 3" << endl;
        return false;
    }
    try {
        port = stoi(argument[1]);

    } catch (exception) {
        cout << "One of your arguments was incorrect please check over and try again" << endl;
        return false;
    }
    if (port != 80 && (port < 1024 || port > 49152)) {
        cerr << "Bad port number" << endl;
        return false;
    }

    return true;
}

/**
 *
 * @param socketDescriptor
 * @return
 */
string processGet(int sd){

    string header = "";
    char last = 0;

    while ( true ){

        char current = 0;
        recv(sd , &current , 1 , 0);
        // For each header, it is ended with a \r\n
        if ( current == '\n' || current == '\r' ){
            if ( last == '\r' && current == '\n' ){
                break;
            }

        }else{
            header += current;
        }
        last = current;
    }
    return header;
}

/**
 *
 * @param statusCode
 * @return
 */
string statusResponse(int statusCode){
    string response = "";
    switch(statusCode){
        case 200:
            response = "HTTP/1.1 200 OK\r\n";
            break;
        case 400:
            response = "HTTP/1.1 400 Bad Request\r\n";
            break;
        case 401:
            response = "HTTP/1.1 401 Unauthorized\r\n";
            break;
        case 403:
            response = "HTTP/1.1 403 Forbidden\r\n";
            break;
        case 404:
            response = "HTTP/1.1 404 Not Found\r\n";
            break;
        default:
            break;
    }
    return response;
}

/**
 *
 * @param filePath
 * @param statusCode
 * @param fileContent
 */
void response(string &filePath, string &statusCode, string &fileContent) {
    fileContent = "";

    // Trying to access a file that is above the directory the server is running
    if (filePath.substr(0, 2).compare("..") == 0) {
        fileContent = "Accessing that file is Forbidden";
        statusCode = statusResponse(403);

    }else if (filePath.length() >= 15) {

        if (filePath.substr(filePath.length() - 15, filePath.length()).compare("SecretFile.html") == 0) {
            fileContent = "Access to that file is Unauthorized";
            statusCode = statusResponse(401);
        }

    } else {
        // All the other cases is valid
        // Need to append a . in front to know start to search from current path
        filePath = "." + filePath;
        cout << "Looking for this file " + filePath << endl;
        FILE *file = fopen(filePath.c_str(), "r");

        // Could not open the file because it doesn't exist
        if (file == nullptr) {

            fileContent = "That file could not be found";
            statusCode = statusResponse(404);

        } else {
            // Found the file
            while (!feof(file)) {

                string line;
                char c = fgetc(file);
                if (c < 0) {
                    continue;
                    // Encountered not supported character. Skip that character
                }
                // Manually append these to the string response
                if (c == '\n') {
                    fileContent += '\n';
                    continue;
                } else if (c == '\r') {
                    fileContent += "\r";
                    continue;
                }
                fileContent += c;
            }
            fclose(file);
            statusCode = statusResponse(200);
        }
    }
}

/**
 *
 * @param threadData
 * @return
 */
void *processGet(void *threadData) {
    //extract data about connection and repetition
    struct thread_data *data;
    data = (struct thread_data *) threadData;
    //get the message from the socket
    int sd = data->sd;
    string file = "";
    bool getting = false;

    while (getting == false) {
        //read each line in the get request for all needed info
        string getRequest = processGet(sd);

        cout << "	Header: " << getRequest << "\n";

        //extract file name once it is found which should be after the GET in the first line of the request
        if (getRequest.substr(0, 3) == "GET") {
            file = getRequest.substr(4, getRequest.length() - 13);

            //todo remove once done testing
            cout << "GOT file!  " << file << "\n";
            getting = true;
        }

        //this will be hit if the entire request is parsed and a file is not found
        if (getRequest == "") {
            break;
        }
    }

    string statusCode;
    string fileContent;
    if (getting == true) {
        response(file, statusCode, fileContent);

    } else {
        // Could not recognize the get request
        fileContent = "Request could not be processed. Please verify formatting";
        statusCode = statusResponse(400);
    }

    string pageLength = to_string(fileContent.size());

    //each line of response message
    string locationLine = "Location " + file;
    string contentLine = "Content-Length: " + pageLength + "\r\n";
    string contentType = "Content-Type: text/plain\r\n\r\n";

    //create full response message
    string responseFull = statusCode + locationLine + contentLine + contentType + fileContent;

    cout << "Printing out the response " + responseFull << endl;

    send(sd, &responseFull[0], responseFull.size(), 0);
    close(sd);
    return 0;
}

/**
 *
 * @param argumentNum
 * @param argument
 * @return
 */
int main(int argumentNum, char *argument[]) {

    argumentNum = 2; //
    char * tempArg[] = {"", "2050"};
    argument = tempArg;
    //verify all input from the user is valid
    if (!verifyArgs(argumentNum, argument)) {
        cout << "Arguments were invalid." << endl;
        return -1;
    }

    //create data structures for socket connection as specified in lecture slides
    sockaddr_in acceptSocketAddr;
    bzero((char *) &acceptSocketAddr, sizeof(acceptSocketAddr));
    acceptSocketAddr.sin_family = AF_INET;
    acceptSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    acceptSocketAddr.sin_port = htons(port);

    // Open socket and bind
    serverSD = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM = TCP, 0 = IP
    const int on = 1;
    setsockopt(serverSD, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int));
    bind(serverSD, (sockaddr *) &acceptSocketAddr, sizeof(acceptSocketAddr));

    // Listen and accept
    listen(serverSD, connection);
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);

    //run forever
    while (true) {
        newSD = accept(serverSD, (sockaddr *) &newSockAddr, &newSockAddrSize);

        //create new thread and run thread_server on that thread with the information received from the client
        pthread_t new_thread;
        struct thread_data *data;

        //save data into thread_data object from socket that will be used in thread_server
        data = new thread_data();
        data->sd = newSD;

        int iret1 = pthread_create(&new_thread, NULL, processGet, (void *) data);
    }
}

