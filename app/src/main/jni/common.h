#ifndef COMMON_H
#define COMMON_H

#include <jni.h>

// NULL
#include <stdio.h>
// va_list,vsnprintf
#include <stdarg.h>
// errno
#include <errno.h>
// strerror_r, memset
#include <string.h>
// socket, bind, getsockname, listen, accept, recv, send, connect
#include <sys/types.h>
#include <sys/socket.h>
// inet_ntop
#include <arpa/inet.h>
// socketaddr_un
#include <sys/un.h>
// htons, socketaddr_in
#include <netinet/in.h>
// #include <sys/endian.h>

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
 * 打印输出给定地址的IP地址和端口号
 */
void LogAddress(JNIEnv *env, jobject obj, const char *message, const struct sockaddr_in *address);

/**
 * 将socket绑定到某一端口
 */
void BindSocketToPort(JNIEnv *env, jobject obj, int sd, unsigned short port);

/**
 * 获取当前绑定的端口号
 */
unsigned short GetSocketPort(JNIEnv *env, jobject obj, int sd);

int throwNewException(JNIEnv *env, const char *msg);

#endif // COMMON_H