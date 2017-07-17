//
// Created by ziv on 2017/7/10.
//

#ifndef NATIVESOCKET_UDPTOOLS_H
#define NATIVESOCKET_UDPTOOLS_H

#include "common.h"

int NewUdpSocket(JNIEnv *env, jobject obj);

ssize_t ReceiveDatagramFromSocket(JNIEnv *env, jobject obj, int sd, struct sockaddr_in *address,
                                  char *buffer, size_t bufferSize);

ssize_t SendDatagramToSocket(JNIEnv *env, jobject obj, int sd, const struct sockaddr_in *address,
                             const char *buffer, size_t bufferSize);

#endif //NATIVESOCKET_UDPTOOLS_H
