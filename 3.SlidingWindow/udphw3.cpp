// Created by Kieran Coito on 11/7/19.
#include "UdpSocket.h"
#include "Timer.h"


// functions for stop and wait implementation

// Stop-and-wait test implements the "stop-and-wait" algorithm, i.e., a client writes a message sequence number in
// message[0], sends message[] and waits until it receives an integer acknowledgment of that sequence number from a
// server, while the server receives message[], copies the sequence number from message[0] to an acknowledgment

/**
 *
 * @param sock
 * @param max
 * @param message
 * @return
 */
int clientStopWait( UdpSocket &sock, const int max, int message[] ){
    // sends message[] and receives an acknowledgment from the server max (=20,000) times using the sock object. If the
    // client cannot receive an acknowledgment immediately, it should start a Timer. If a timeout occurs (i.e., no
    // response after 1500 usec), the client must resend the same message. The function must count the number of
    // messages retransmitted and return it to the main function as its return value.

    Timer* timeout = new Timer();
    int retransmitNum = 0;

    for(int i = 0; i < max; i++){

        //send message





    }


}

/**
 *
 * @param sock
 * @param max
 * @param message
 */
void serverReliable( UdpSocket &sock, const int max, int message[] ){
    // receiving message[] and sending an acknowledgment at a server side max (=20,000) times using the sock object.

}

// functions for sliding window implementation
//
// Sliding window implements the "sliding window" algorithm, using the first element in the message[] for storing
// sequence numbers (in messages sent by client) and acknowledgement numbers (in messages sent by server).
//
// A client keeps writing a message sequence number in message[0] and sending message[] as long as the number of
// in-transit messages is less than a given windowSize.
//
// The server receives message[], saves the message's sequence number (stored in message[0]) in a local buffer array
// and returns a cumulative acknowledgement, i.e., the last received message in order.

/**
 *
 * @param sock
 * @param max
 * @param message
 * @param windowSize
 * @return
 */
int clientSlidingWindow( UdpSocket &sock, const int max, int message[], int windowSize ){
    // sends message[] and receiving an acknowledgment from a server max (=20,000) times using the sock object. As
    // described above, the client can continuously send a new message[] and increasing the sequence number as long as
    // the number of in-transit messages (i.e., # of unacknowledged messages) is less than "windowSize." That number
    // should be decremented every time the client receives an acknowledgment. If the number of unacknowledged messages
    // reaches "windowSize," the client should start a Timer. If a timeout occurs (i.e., no response after 1500 usec),
    // it must resend the message with the minimum sequence number among those which have not yet been acknowledged.
    // The function must count the number of messages (not bytes) re-transmitted and return it to the main function as
    // its return value.
    // NOTE: the windowSize in this assignment refers to the number of messages, not the number of bytes
}

/**
 *
 * @param sock
 * @param max
 * @param message
 * @param windowSize
 */
void serverEarlyRetrans( UdpSocket &sock, const int max, int message[], int windowSize ){
    // receives message[] and sends an acknowledgment to the client max (=20,000) times using the sock object. Every
    // time the server receives a new message[], it must save the message's sequence number in an array and return a
    // cumulative acknowledgment, i.e., the last received message in order.

}
