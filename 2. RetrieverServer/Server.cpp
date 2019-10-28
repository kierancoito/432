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
 * This method will verify that all parameters passed into the application are valid
 * @param argumentNum number of parameters that were passed into the application
 * @param argument char array of the parameters that user passed into the application
 * @return whether all parameters are valid
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
 * This method will take return the current line of the get request in the socket
 * @param sd the socket where the get request is being sent over
 * @return string that represents a line of the get request
 */
string processGet(int sd){

    string header = "";
    //keep track of the previous char so that the two consecutive escape chars
    //can be acknowledged properly
    char previous = 0;

    //continue to pull out of the socket until end of line characters are hit
    while ( true ){
        //keep track of current char
        char current = 0;
        //pull out the next char in the socket
        recv(sd , &current , 1 , 0);
        // if the previous and current chars are the two escape chars than break out of the loop
        // otherwise add the current char to the header string and set previous to the current char
        if ( current == '\n' || current == '\r' ){
            if ( previous == '\r' && current == '\n' ){
                break;
            }

        }else{
            header += current;
        }
        previous = current;
    }
    //return current line
    return header;
}

/**
 * This method will return the string message for the get response that corresponds to
 * the code it must reply with
 *
 * @param statusCode the status code for the response messsage
 * @return the first line of the response message
 */
string statusResponse(int statusCode){
    string response = "";
    switch(statusCode){
        case 200:
            response = "HTTP/1.0 200 OK\r\n";
            break;
        case 400:
            response = "HTTP/1.0 400 Bad Request\r\n";
            break;
        case 401:
            response = "HTTP/1.0 401 Unauthorized\r\n";
            break;
        case 403:
            response = "HTTP/1.0 403 Forbidden\r\n";
            break;
        case 404:
            response = "HTTP/1.0 404 Not Found\r\n";
            break;
        default:
            break;
    }
    return response;
}

/**
 * This method will construct the response message to the GET request
 *
 * @param filePath the file that the get request is looking for
 * @param statusCode the status code message that the get reponse will contain
 * @param fileContent the content of the get response which will contain either the contents of the file
 *                  being looked for, or an error message
 */
void response(string &filePath, string &statusCode, string &fileContent) {
    fileContent = "";

    // Trying to access a file that is above the directory the server is running
    if (filePath.substr(0, 2).compare("..") == 0) {
        fileContent = "Accessing that file is Forbidden";
        statusCode = statusResponse(403);

    }
    //checks if the access is to SecretFile.html which is unauthorized to the user
    else if (filePath.compare("SecretFile.html") == 0) {
        fileContent = "Access to that file is Unauthorized";
        statusCode = statusResponse(401);

    }
    //all other cases
    else {

        // append a . so that the search for the file happens in the active directory
        filePath = "." + filePath;
        FILE *file = fopen(filePath.c_str(), "r");

        // Could not open the file because it doesn't exist
        if (file == nullptr) {
            fileContent = "That file could not be found";
            statusCode = statusResponse(404);

        } else {

            // Found the file, and now copy it into a string to send
            while (!feof(file)) {

                string line;
                char c = fgetc(file);
                //skip invalid characters
                if (c < 0) {
                    continue;

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
    return;
}

/**
 * This will process the get request that was sent to the server and then respond to it
 *
 * @param threadData - the object that contains the socket info to get the request
 * @return N/A
 */
void *processGet(void *threadData) {
    //extract data about connection and repetition
    struct thread_data *data;
    data = (struct thread_data *) threadData;
    //get the message from the socket
    int sd = data->sd;
    string file = "";
    bool getting = false;

    while ( true ){
        // Read a newline-terminated string:
        string header = processGet(sd);
        if ( header == "" ){
            break;
        }
        cout << "Header: " << header << "\n";
        // Only support GET request, flag will not be set if this does not exist
        if ( header.substr(0 , 3) == "GET" ){
            // Number 13 is for for " HTTP/1.1\r\n"
            file = header.substr(4 , header.length() - 13);
            cout << "Found file: " << file << "\n";
            getting = true;
            break;
        }
    }

    string statusCode = "";
    string fileContent = "";
    //create response if there was a valid request
    if (getting == true) {
        response(file, statusCode, fileContent);

    }
    //create response for invalid request
    else {
        fileContent = "Request could not be processed. Please verify formatting";
        statusCode = statusResponse(400);
    }

    string pageLength = to_string(fileContent.size());

    //each line of response message
    string locationLine = "Location " + file + "\r\n";
    string contentLine = "Content-Length: " + pageLength + "\r\n";
    string contentType = "Content-Type: text/plain\r\n\r\n";

    //create full response message
    string responseFull = statusCode + locationLine + contentLine + contentType + fileContent;

    //send response
    int result = send(sd, responseFull.c_str(), strlen(responseFull.c_str()), 0);
    if(result < 0){
        cout << "sending did not work" << endl;

    }
    cout << "finished connection" << endl;
    //close socket
    close(sd);
    return 0;
}

/**
 * This method will control the server
 *
 * @param argumentNum number of arguments from the user
 * @param argument array of the arguments from the user, only argument needed is port number
 * @return int signifing how the main was terminated
 */
int main(int argumentNum, char *argument[]) {

    //verify all input from the user is valid
    if (!verifyArgs(argumentNum, argument)) {
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
    int listenResult = listen(serverSD, connection);
    if(listenResult < 0){
        cerr << "could not listen" << endl;
    }
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
        cout << "connection made, processing information now" << endl;
        int iret1 = pthread_create(&new_thread, NULL, processGet, (void *) data);
    }
}

