// Created by Kieran Coito on 11/7/19.
#include "UdpSocket.h"
#include "Timer.h"


// functions for stop and wait implementation

/**
 *
 * @param sock
 * @param max
 * @param message
 * @return
 */
int clientStopWait( UdpSocket &sock, const int max, int message[] ){

}

/**
 *
 * @param sock
 * @param max
 * @param message
 */
void serverReliable( UdpSocket &sock, const int max, int message[] ){

}

//functions for sliding window implementation

/**
 *
 * @param sock
 * @param max
 * @param message
 * @param windowSize
 * @return
 */
int clientSlidingWindow( UdpSocket &sock, const int max, int message[], int windowSize ){

}

/**
 *
 * @param sock
 * @param max
 * @param message
 * @param windowSize
 */
void serverEarlyRetrans( UdpSocket &sock, const int max, int message[], int windowSize ){

}
