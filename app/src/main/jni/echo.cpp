//
// Created by ziv on 17-6-28.
//
#include "com_ziv_socket_ServerActivity.h"

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

// socketaddr_un
#include <sys/un.h>

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
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param format message format and arguments
 */
static void LogMessage(JNIEnv *env, jobject obj, const char *format, ...) {
    static jmethodID methodID = NULL;
    if (NULL == methodID) {
        jclass clazz = env->GetObjectClass(obj);
        methodID = env->GetMethodID(clazz, "logMessage", "(Ljava/lang/String;)V");
        env->DeleteLocalRef(clazz);
    }

    if (NULL != methodID) {
        // 格式化日志信息
        char buffer[MAX_LOG_MESSAGE_LENGTH];

        va_list ap;
        va_start(ap, format);
        vsnprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, ap);
        va_end(ap);

        // 将缓冲区转换成Java字符串
        jstring message = env->NewStringUTF(buffer);

        if (NULL != message) {
            // 记录消息
            env->CallVoidMethod(obj, methodID, message);
            env->DeleteLocalRef(message);
        }
    }
}

/**
 * 抛出异常方法
 *
 * @param env JNIEnv interface.
 * @param className class name.
 * @param message exception message.
 */
static void ThrowException(JNIEnv *env, const char *className, const char *message) {
    jclass clazz = env->FindClass(className);
    if (NULL != clazz) {
        env->ThrowNew(clazz, message);
        env->DeleteLocalRef(clazz);
    }
}

/**
 * 抛出包含errnum的异常
 *
 * @param env JNIEnv interface.
 * @param className class name.
 * @param errnum error number.
 */
static void ThrowErrornoException(JNIEnv *env, const char *className, int errnum) {
    char buffer[MAX_LOG_MESSAGE_LENGTH];

    // 获取错误号消息
    if (-1 == strerror_r(errno, buffer, MAX_LOG_MESSAGE_LENGTH)) {
        strerror_r(errno, buffer, MAX_LOG_MESSAGE_LENGTH);
    }
    // 抛出异常
    ThrowException(env, className, buffer);
}

/**
 * 构造新的TCP Socket
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @return socket descriptor
 * @throws IOException
 */
static int NewTcpSocket(JNIEnv *env, jobject obj) {
    // 构造socket
    LogMessage(env, obj, "Constructing a new TCP socket...");
    int tcpSocket = socket(PF_INET, SOCK_STREAM, 0);

    // check socket
    if (-1 == tcpSocket) {
        ThrowErrornoException(env, "java/io/IOException", errno);
    }

    return tcpSocket;
}

/**
 * 将socket绑定到某一端口
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param sd socket descriptor.
 * @param port port number or zero for random port.
 * @throws IOException
 */
static void BindSocketToPort(JNIEnv *env, jobject obj, int sd, unsigned short port) {
    struct sockaddr_in address;
    // 绑定Socket地址
    memset(&address, 0, sizeof(address));
    address.sin_family = PF_INET;

    // 绑定所有地址
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // 将端口转换为网络字节顺序
    address.sin_port = htons(port);

    // 绑定Socket
    LogMessage(env, obj, "Binding to port %hu.", port);
    if (-1 == bind(sd, (const sockaddr *) &address, sizeof(address))) {
        ThrowErrornoException(env, "java/io/IOException", errno);
    }
}

/**
 * 获取当前绑定的端口号
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket descriptor
 * @return port number
 * @throws IOException
 */
static unsigned short GetSocketPort(JNIEnv *env, jobject obj, int sd) {
    unsigned short port = 0;
    struct sockaddr_in address;
    socklen_t addressLength = sizeof(address);

    // 获取Socket地址
    if (-1 == getsockname(sd, (sockaddr *) &address, &addressLength)) {
        ThrowErrornoException(env, "java/io/IOException", errno);
    } else {
        // 将端口转换为主机字节顺序
        port = ntohs(address.sin_port);
        LogMessage(env, obj, "Binded to random port %hu.", port);
    }
    return port;
}

/**
 * 监听进入的链接:将链接放入队列等待应用程序显式的accept
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket descriptor
 * @param backlog backlog size
 * @throws IOException
 */
static void ListenOnSocket(JNIEnv *env, jobject obj, int sd, int backlog) {
    // 监听给定backlog的socket
    LogMessage(env, obj, "Listening on socket with a backlog of %d pending connections.", backlog);
    if (-1 == listen(sd, backlog)) {
        ThrowErrornoException(env, "java/io/IOException", errno);
    }
}

/**
 * 打印输出给定地址的IP地址和端口号
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @param message message text
 * @param address address instance
 */
static void
LogAddress(JNIEnv *env, jobject obj, const char *message, const struct sockaddr_in *address) {
    char ip[INET_ADDRSTRLEN];
    // 将IP地址转换为字符串
    if (NULL == inet_ntop(PF_INET, &(address->sin_addr), ip, INET_ADDRSTRLEN)) {
        ThrowErrornoException(env, "java/io/IOException", errno);
    } else {
        // 将端口转换为主机字节顺序
        unsigned short port = ntohs(address->sin_port);
        LogMessage(env, obj, "%s %s:%hu.", message, ip, port);
    }
}

/**
 * 在给定的socket上阻塞和等待客户链接
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket descriptor
 * @return client socket
 * @throws IOException
 */
static int AcceptOnSocket(JNIEnv *env, jobject obj, int sd) {
    struct sockaddr_in *address;
    socklen_t addressLength = sizeof(address);

    // 阻塞和等待客户端链接
    LogMessage(env, obj, "Waiting for a client connection...");

    int clientSocket = accept(sd, (sockaddr *) &address, &addressLength);
    if (-1 == clientSocket) {
        ThrowErrornoException(env, "java/io/IOException", errno);
    } else {
        LogMessage(env, obj, "Client connection from ", &address);
    }
    return clientSocket;
}

/**
 *
 * @param env
 * @param obj
 * @param sd
 * @param buffer
 * @param bufferSize
 * @return
 */
static ssize_t ReceiveFromSocket(JNIEnv *env, jobject obj,
                                 int sd, char *buffer, size_t bufferSize) {
    // 阻塞并接收来自socket的数据放在缓冲区
    LogMessage(env, obj, "Receiving from the socket...");
    ssize_t recvSize = recv(sd, buffer, bufferSize - 1, 0);

    if (-1 == recvSize) {
        ThrowErrornoException(env, "java/io/IOException", errno);
    }
}