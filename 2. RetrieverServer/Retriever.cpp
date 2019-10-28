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
#include <fstream>

using namespace std;

/**
 * Created by Kieran Coito
 * CSS 432
 * October 11th 2019
 *
 * This program can make file requests over the internet and can request pages from websites
 * If it requests a valid page that has content it will then save the page for future viewing
 *
 *
 *
 */

string requestType;
int port;
char *serverName, *fileName;

string NAME = "savedPage.txt";

/**
 * This method will verify that all parameters passed into the application are valid
 * @param argumentNum number of parameters that were passed into the application
 * @param argument char array of the parameters that user passed into the application
 * @return whether all parameters are valid
 */
bool verifyArgs(int argumentNum, char *argument[]) {

    if (argumentNum != 5) {
        cout << "Need 4 args. Instead got " << argumentNum << " args." << endl;
        return false;
    }

    char * tempServer;
    try {

        requestType = argument[1];
        port = stoi(argument[4]);
        tempServer = argument[2];
        fileName = argument[3];

    } catch ( exception ) {
        cout << "One of your arguments was incorrect please check over and try again" << endl;
        return false;
    }

    //ensure valid port
    if (port != 80 && (port > 49152 || port < 1024)) {
        cout << "Port number invalid: " << port << endl;
        return false;
    }

    //fix server name if needed by removing info before the front of the address
    string server = tempServer;
    if(server.substr(0,7).compare("http://") == 0){
        server = server.substr(7, server.length()-7);

    }
    if(server.substr(0,8).compare("https://") == 0){
        server = server.substr(8 ,server.length()-7);

    }

    serverName = new char[server.length()];
    strcpy(serverName, server.c_str());
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
 * This method will process the GET response from the server it sent a request to
 *
 * @param SD the socket the response is being sent over
 */
void receiveHtml(int SD){

    int length = 0;
    int statusCode;
    bool stillInGet = true;

    //pull each line of the get request and parse it for needed information
    //stillInGet will act as flag to stay in loop
    while ( stillInGet == true ){

        //get current line of the get request
        string responseLine = processGet(SD);

        //get the length of the file being request from the get response
        if ( responseLine.substr(0 , 15).compare("Content-Length:") == 0 ){
            length = atoi(responseLine.substr(16 , responseLine.length()).c_str());
        }

        //get the status code of the page from the get response
        if(responseLine.substr(0, 8).compare("HTTP/1.0 ") == 0){
            statusCode = stoi(responseLine.substr(9, 3));
        }
        //end of header so set flag to false to exit loop
        if ( responseLine == "" ){
            stillInGet = false;
        }
    }

    //if there is no body to the response than tell the user and close the socket and exit the program
    if(length == 0){
        cout << "This request returned no body to display or save" << endl;
        close(SD);
        return;
    }
    //open file to save website
    ofstream output;

    //get body of file being request
    char buffer[length];
    recv(SD , &buffer , length , 0);

    //convert char array to a single string
    string html = "";
    for ( int i = 0; i < length; i++ ){
        html += buffer[i];
    }

    //save html to file and display it to the user if status code is 200
    cout << html << endl;
    if(statusCode == 200){
        output.open(NAME);
        output << html;
    }

    //close file and socket
    close(SD);
    if(statusCode == 200){
        output.close();
    }
    return;
}

/**
 * This method will control the retriever
 *
 * @param argumentNum number of arguments from the user
 * @param argument request type, servername, filename, and portnumber as received as command line arguments
 * @return int signifing how the main was terminated
 */
int main(int argumentNum, char *argument[]) {

    //verify all arguments are valid
    if (!verifyArgs(argumentNum, argument)) {
        cout << "Arguments were invalid." << endl;
        return -1;
    }

    // set up data structure and connect to port
    // as specified in lecture slides
    struct hostent* host = gethostbyname(serverName);
    sockaddr_in sendSockAddr;
    bzero( (char *)&sendSockAddr, sizeof(sendSockAddr));

    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr( inet_ntoa(*(struct in_addr*)*host->h_addr_list) );
    sendSockAddr.sin_port = htons(port);
    int clientSD = socket(AF_INET, SOCK_STREAM, 0);

    cout << "attempting connection" << endl;
    int connectStatus = connect( clientSD, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
    if (connectStatus < 0) {
        cerr << "Connection failed!" << endl;
        close(clientSD);
        return -1;
    }
    //create get request
    string getRequest = requestType + " " + string(fileName) + " HTTP/1.0\r\n"
                               + "Host: " + string(serverName) + "\r\n" + "\r\n";
    //send get request to host
    cout << "sending request" << endl;
    int result = send(clientSD, getRequest.c_str(), strlen(getRequest.c_str()), 0);
    if ( result <= 0 ){
        cout << "Unable to send the request";
        return -1;
    }
    //receive back from server
    receiveHtml(clientSD);
    return 0;
}
