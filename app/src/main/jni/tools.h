//
// Created by ziv on 17-6-28.
//
#ifndef TOOLS_H
#define TOOLS_H

#include "com_ziv_socket_ServerActivity.h"

// NULL
#include <stdio.h>
// va_list,vsnprintf
// errno
#include <errno.h>
// strerror_r, memset
#include <string.h>

// socket, bind, getsockname, listen, accept, recv, send, connect
#include <sys/socket.h>

// socketaddr_un

// htons, socketaddr_in
#include <netinet/in.h>
// #include <sys/endian.h>
// inet_ntop
#include <arpa/inet.h>

// close, unlink
#include <unistd.h>
// offsetof
#include <stddef.h>

#define MAX_LOG_MESSAGE_LENGTH 256
#define MAX_BUFFER_SIZE 80

/**
 * 信息格式化并调用logMessage方法输出
 */
void LogMessage(JNIEnv *env, jobject obj, const char *format, ...);

/**
 * 抛出异常方法
 */
void ThrowException(JNIEnv *env, const char *className, const char *message);

/**
 * 抛出包含errnum的异常
 */
void ThrowErrnoException(JNIEnv *env, const char *className, int errnum);

/**
 * 构造新的TCP Socket
 */
int NewTcpSocket(JNIEnv *env, jobject obj);

/**
 * 将socket绑定到某一端口
 */
void BindSocketToPort(JNIEnv *env, jobject obj, int sd, unsigned short port);

/**
 * 获取当前绑定的端口号
 */
unsigned short GetSocketPort(JNIEnv *env, jobject obj, int sd);

/**
 * 监听进入的链接:将链接放入队列等待应用程序显式的accept
 */
void ListenOnSocket(JNIEnv *env, jobject obj, int sd, int backlog);

/**
 * 打印输出给定地址的IP地址和端口号
 */
void LogAddress(JNIEnv *env, jobject obj, const char *message, const struct sockaddr_in *address);

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

#endif // TOOLS_H