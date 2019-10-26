//#include <sys/types.h>    // socket, bind
//#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
//#include <netinet/in.h>   // htonl, htons, inet_ntoa
//#include <arpa/inet.h>    // inet_ntoa
//#include <netdb.h>        // gethostbyname
//#include <unistd.h>       // read, write, close
//#include <string.h>       // bzero
//#include <netinet/tcp.h>  // SO_REUSEADDR
//#include <stdlib.h>       // EXIT_FAILURE
//#include <sys/uio.h>      // writev
//#include <sys/time.h>     // timeval
//#include <string>
//#include <iostream>
//#include <exception>
//#include <fstream>
//
//using namespace std;
//
///**
// * Created by Kieran Coito
// * CSS 432
// * October 11th 2019
// *
// *
// */
//
//int port;
//char *serverName, *fileName;
//
//string NAME = "savedPage.txt";
//
///**
// *
// * @param argumentNum
// * @param argument
// * @return
// */
//bool verifyArgs(int argumentNum, char *argument[]) {
//
//    if (argumentNum != 4) {
//        cout << "Need 4 args. Instead got " << argumentNum << " args." << endl;
//        return false;
//    }
//
//    char * tempServer;
//    try {
//        port = stoi(argument[3]);
//        tempServer = argument[1];
//        fileName = argument[2];
//
//    } catch ( exception ) {
//        cout << "One of your arguments was incorrect please check over and try again" << endl;
//        return false;
//    }
//
//    //ensure valid port
//    if (port != 80 && (port > 49152 || port < 1024)) {
//        cout << "Port number invalid: " << port << endl;
//        return false;
//    }
//
//    //fix server name if needed
//    string server = tempServer;
//    if(server.substr(0,7).compare("http://") == 0){
//        server = server.substr(7, server.length()-7);
//
//    }
//    if(server.substr(0,8).compare("https://") == 0){
//        server = server.substr(8 ,server.length()-7);
//
//    }
//
//    serverName = new char[server.length()];
//    strcpy(serverName, server.c_str());
//    return true;
//}
//
///**
// *
// * @param sd
// * @return
// */
//string processGet(int sd){
//
//    string header = "";
//    char last = 0;
//
//    while ( true ){
//
//        char current = 0;
//        recv(sd , &current , 1 , 0);
//        // For each header, it is ended with a \r\n
//        if ( current == '\n' || current == '\r' ){
//            if ( last == '\r' && current == '\n' ){
//                break;
//            }
//
//        }else{
//            header += current;
//        }
//        last = current;
//    }
//    return header;
//}
//
///**
// *
// * @param SD
// */
//void receiveHtml(int SD){
//
//    int length = 0;
//    int statusCode;
//    bool stillInGet = true;
//
//    //pull each line of the get request and parse it for needed information
//    //stillInGet will act as flag to stay in loop
//    while ( stillInGet == true ){
//
//        //get current line of the get request
//        string responseLine = processGet(SD);
//
//        //get the length of the file being request from the get response
//        if ( responseLine.substr(0 , 15).compare("Content-Length:") == 0 ){
//            length = atoi(responseLine.substr(16 , responseLine.length()).c_str());
//        }
//
//        //get the status code of the page from the get response
//        if(responseLine.substr(0, 8).compare("HTTP/1.1 ") == 0){
//            statusCode = stoi(responseLine.substr(9, 3));
//        }
//        //end of header so set flag to false to exit loop
//        if ( responseLine == "" ){
//            stillInGet = false;
//        }
//    }
//
//    if(length == 0){
//        cout << "This request returned no body to display or save" << endl;
//        close(SD);
//    }
//    //open file to save website
//    ofstream output;
//
//    //get body of file being request
//    char buffer[length];
//    recv(SD , &buffer , length , 0);
//
//    //convert char array to a single string
//    string html = "";
//    for ( int i = 0; i < length; i++ ){
//        html += buffer[i];
//    }
//
//    //save html to file and display it to the user if status code is 200
//    cout << html << endl;
//    if(statusCode == 200){
//        output.open(NAME);
//        output << html;
//
//    }
//
//    //close file and socket
//    close(SD);
//    if(statusCode == 200){
//        output.close();
//    }
//    return;
//}
//
///**
// *
// * @param argumentNum nunmber of arguments
// * @param argument servername, filename, and portnumber as received as command line arguments
// * @return
// */
//int main(int argumentNum, char *argument[]) {
//
//
//    //argumentNum = 4; //
//    //char * tempArg[] = {"", "http://google.com", "/index.html", "80"};
//    //argument = tempArg;
//    if (!verifyArgs(argumentNum, argument)) {
//        cout << "Arguments were invalid." << endl;
//        return -1;
//    }
//
//    // set up data structure and connect to port
//    // as specified in lecture slides
//    struct hostent* host = gethostbyname(serverName);
//    sockaddr_in sendSockAddr;
//    bzero( (char *)&sendSockAddr, sizeof(sendSockAddr));
//
//    sendSockAddr.sin_family = AF_INET;
//    sendSockAddr.sin_addr.s_addr = inet_addr( inet_ntoa(*(struct in_addr*)*host->h_addr_list) );
//    sendSockAddr.sin_port = htons(port);
//
//    int clientSD = socket(AF_INET, SOCK_STREAM, 0);
//
//    int connectStatus = connect( clientSD, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
//    if (connectStatus < 0) {
//        cerr << "Connection failed!" << endl;
//        close(clientSD);
//        return -1;
//    }
//    //create get request
//    string getRequest = "GET " + string(fileName) + " HTTP/1.1\r\n"
//                               + "Host: " + string(serverName) + "\r\n" + "\r\n";
//    //send get request to host
//    int result = send(clientSD, getRequest.c_str(), strlen(getRequest.c_str()), 0);
//    if ( result <= 0 ){
//        cout << "Unable to send the request";
//        return -1;
//    }
//    //receive back from server
//    receiveHtml(clientSD);
//    return 0;
//}
