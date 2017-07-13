//
// Created by ziv on 17-6-28.
//
#include "TcpTools.h"
#include "common.h"

/**
 * 构造新的TCP Socket
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @return socket descriptor
 * @throws IOException
 */
int NewTcpSocket(JNIEnv *env, jobject obj) {
    LogMessage(env, obj, "Constructing a new TCP socket...");
    // 创建 数据流 Socket
    int tcpSocket = socket(PF_INET, SOCK_STREAM, 0);

    // check socket
    if (-1 == tcpSocket) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    }

    return tcpSocket;
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
void ListenOnSocket(JNIEnv *env, jobject obj, int sd, int backlog) {
    // 监听给定backlog的socket
    LogMessage(env, obj, "Listening on socket with a backlog of %d pending connections.", backlog);
    if (-1 == listen(sd, backlog)) {
        ThrowErrnoException(env, "java/io/IOException", errno);
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
int AcceptOnSocket(JNIEnv *env, jobject obj, int sd) {
    struct sockaddr_in *address;
    socklen_t addressLength = sizeof(address);

    // 阻塞和等待客户端链接
    LogMessage(env, obj, "Waiting for a client connection...");

    int clientSocket = accept(sd, (sockaddr *) &address, &addressLength);
    if (-1 == clientSocket) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    } else {
        LogMessage(env, obj, "Client connection from ", &address);
    }
    return clientSocket;
}

/**
 * 阻塞并接受来自Socket的数据放到缓冲区
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket descriptor
 * @param buffer data buffer
 * @param bufferSize buffer size
 * @return receive size
 * @throws IOException
 */
ssize_t ReceiveFromSocket(JNIEnv *env, jobject obj,
                          int sd, char *buffer, size_t bufferSize) {
    // 阻塞并接收来自socket的数据放在缓冲区
    LogMessage(env, obj, "Receiving from the socket...");
    ssize_t recvSize = recv(sd, buffer, bufferSize - 1, 0);

    if (-1 == recvSize) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    } else {
        // 已NULL结尾缓冲区形成一个字符串
        buffer[recvSize] = NULL;

        if (recvSize > 0) {
            LogMessage(env, obj, "Received %d bytes: %s", recvSize, buffer);
        } else {
            LogMessage(env, obj, "Client disconnected.");
        }
    }
    return recvSize;
}

/**
 * 将数据缓冲区发送到Socket
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket descriptor
 * @param buffer data buffer
 * @param bufferSize buffer size
 * @return send size
 * @throws IOException
 */
ssize_t SendToSocket(JNIEnv *env, jobject obj,
                     int sd, const char *buffer, size_t bufferSize) {
    LogMessage(env, obj, "Sending to the socket...");
    ssize_t sendSize = send(sd, buffer, bufferSize, 0);

    if (-1 == sendSize) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    } else {
        if (sendSize > 0) {
            LogMessage(env, obj, "Sent %d bytes: %s", sendSize, buffer);
        } else {
            LogMessage(env, obj, "Client disconnected.");
        }
    }
    return sendSize;
}

/**
 * 连接到给定的IP地址和给定的端口号
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket descriptor
 * @param ip IP address
 * @param port port number
 * @throws IOException
 */
void ConnectToAddress(JNIEnv *env, jobject obj, int sd, const char *ip, unsigned short port) {
    LogMessage(env, obj, "Connecting to %s:%uh ...", ip, port);
    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));
    address.sin_family = PF_INET;

    // 将IP地址字符串转换为网络地址
    if (0 == inet_aton(ip, &(address.sin_addr))) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    } else {
        // 将端口号转换为网络字节顺序
        address.sin_port = htons(port);
        // 转换为地址，connect方法通过提供的协议地址连接socket和server socket
        if (-1 == connect(sd, (const sockaddr *) &address, sizeof(address))) {
            ThrowErrnoException(env, "java/io/IOException", errno);
        } else {
            LogMessage(env, obj, "Connected.");
        }
    }
}