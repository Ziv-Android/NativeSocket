//
// Created by ziv on 2017/7/10.
//
#include "common.h"
#include "UdpTools.h"

/**
 * 新建一个UDP Socket
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @return SocketDescriptor
 */
int NewUdpSocket(JNIEnv *env, jobject obj) {
    LogMessage(env, obj, "Constructing a new UDP socket...");
    // 创建 数据报 Socket
    int udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    if (-1 == udpSocket) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    }

    return udpSocket;
}

/**
 * 从socket中阻塞并接收数据保存到缓冲区，填充客户端地址
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket descriptor
 * @param address client address
 * @param buffer data buffer
 * @param bufferSize buffer size
 * @return receive size
 * @throws IOException
 */
ssize_t ReceiveDatagramFromSocket(JNIEnv *env, jobject obj, int sd, struct sockaddr_in *address,
                                  char *buffer, size_t bufferSize) {
    socklen_t addressLength = sizeof(struct sockaddr_in);
    LogMessage(env, obj, "Receiving from the socket...");
    // 从socket中接收数据
    ssize_t recvSize = recvfrom(sd, buffer, bufferSize, 0, (sockaddr *) address, &addressLength);
    if (-1 == recvSize) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    } else {
        LogAddress(env, obj, "Received from", address);

        buffer[recvSize] = NULL;
        if (recvSize > 0) {
            LogMessage(env, obj, "Received %d bytes: %s", recvSize, buffer);
        }
    }
    return recvSize;
}

/**
 * 通过socket发送制定的数据报到指定地址
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket decsriptor
 * @param address remote address
 * @param buffer data buffer
 * @param bufferSize buffer size
 * @return sent size
 * @throws IOException
 */
ssize_t SendDatagramToSocket(JNIEnv *env, jobject obj, int sd, const struct sockaddr_in *address,
                             const char *buffer, size_t bufferSize) {
    LogAddress(env, obj, "Sending to", address);
    ssize_t sendSize = sendto(sd, buffer, bufferSize, 0,
                              (const sockaddr *) address, sizeof(address));
    if (-1 == sendSize) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    } else {
        LogMessage(env, obj, "Sent %d bytes: %s", sendSize, buffer);
    }
    return sendSize;
}