//
// Created by ziv on 17-6-28.
//
#ifndef NATIVESOCKET_TCPTOOLS_H
#define NATIVESOCKET_TCPTOOLS_H

#include "common.h"

/**
 * 构造新的TCP Socket
 */
int NewTcpSocket(JNIEnv *env, jobject obj);

/**
 * 监听进入的链接:将链接放入队列等待应用程序显式的accept
 */
void ListenOnSocket(JNIEnv *env, jobject obj, int sd, int backlog);

/**
 * 在给定的socket上阻塞和等待客户链接
 */
int AcceptOnSocket(JNIEnv *env, jobject obj, int sd);

/**
 * 阻塞并接受来自Socket的数据放到缓冲区
 */
ssize_t ReceiveFromSocket(JNIEnv *env, jobject obj, int sd, char *buffer, size_t bufferSize);

/**
 * 将数据缓冲区发送到Socket
 */
ssize_t SendToSocket(JNIEnv *env, jobject obj, int sd, const char *buffer, size_t bufferSize);

/**
 * 连接到给定的IP地址和给定的端口号
 */
void ConnectToAddress(JNIEnv *env, jobject obj, int sd, const char *ip, unsigned short port);

#endif // NATIVESOCKET_TCPTOOLS_H