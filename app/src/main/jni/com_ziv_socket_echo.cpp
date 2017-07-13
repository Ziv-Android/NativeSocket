//
// Created by ziv on 2017/7/4.
//
#include "com_ziv_socket_ServerActivity.h"
#include "com_ziv_socket_ClientActivity.h"
#include "common.h"
#include "TcpTools.h"
#include "UdpTools.h"

JNIEXPORT void JNICALL Java_com_ziv_socket_ServerActivity_nativeStartTcpServer(
        JNIEnv *env, jobject obj, jint port) {
    int serverSocket = NewTcpSocket(env, obj);
    if (NULL == env->ExceptionOccurred()) {
        // 绑定Socket到指定端口
        BindSocketToPort(env, obj, serverSocket, (unsigned short) port);
        if (NULL != env->ExceptionOccurred()) {
            goto exit;
        }
        // 随机端口号
        if (0 == port) {
            // 获取当前绑定端口号的Socket
            GetSocketPort(env, obj, serverSocket);
            if (NULL != env->ExceptionOccurred()) {
                goto exit;
            }
        }
        // 等待链接4
        ListenOnSocket(env, obj, serverSocket, 4);
        if (NULL != env->ExceptionOccurred()) {
            goto exit;
        }
        int clientSocket = AcceptOnSocket(env, obj, serverSocket);
        if (NULL != env->ExceptionOccurred()) {
            goto exit;
        }
        char buffer[MAX_BUFFER_SIZE];
        ssize_t recvSize;
        ssize_t sentSize;
        // 接收并送回数据
        while (1) {
            recvSize = ReceiveFromSocket(env, obj, clientSocket, buffer, MAX_BUFFER_SIZE);
            if ((0 == recvSize) || (NULL != env->ExceptionOccurred())) {
                break;
            }
            sentSize = SendToSocket(env, obj, clientSocket, buffer, MAX_BUFFER_SIZE);
            if ((0 == sentSize) || (NULL != env->ExceptionOccurred())) {
                break;
            }
            // 关闭Socket
            close(clientSocket);
        }
    }
    exit:
    if (serverSocket > 0) {
        close(serverSocket);
    }
}

/*
 * Class:     com_ziv_socket_ClientActivity
 * Method:    nativeStartTcpClient
 * Signature: (Ljava/lang/String;ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_ziv_socket_ClientActivity_nativeStartTcpClient
        (JNIEnv *env, jobject obj, jstring ip, jint port, jstring message) {
    // 构造新的TCP socket
    int clientSocket = NewTcpSocket(env, obj);
    if (NULL == env->ExceptionOccurred()) {

        const char *ipAddress = env->GetStringUTFChars(ip, NULL);
        ConnectToAddress(env, obj, clientSocket, ipAddress, (unsigned short) port);
        env->ReleaseStringUTFChars(ip, ipAddress);

        if (NULL != env->ExceptionOccurred()) {
            goto exit;
        }

        const char *messageText = env->GetStringUTFChars(ip, NULL);
        if (NULL == messageText) {
            goto exit;
        }
        jsize messageSize = env->GetStringLength(message);
        SendToSocket(env, obj, clientSocket, messageText, messageSize);
        env->ReleaseStringUTFChars(message, messageText);

        if (NULL != env->ExceptionOccurred()) {
            goto exit;
        }
        char buffer[MAX_BUFFER_SIZE];
        ReceiveFromSocket(env, obj, clientSocket, buffer, MAX_BUFFER_SIZE);
    }

    exit:
    if (clientSocket > -1) {
        close(clientSocket);
    }
}

/*
 * Class:     com_ziv_socket_ServerActivity
 * Method:    nativeStartUdpServer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_ziv_socket_ServerActivity_nativeStartUdpServer
        (JNIEnv *env, jobject obj, jint port) {
    int serverSocket = NewUdpSocket(env, obj);
    if (NULL == env->ExceptionOccurred()) {
        // 绑定socket到指定端口号
        BindSocketToPort(env, obj, serverSocket, (unsigned short) port);
        if (NULL != env->ExceptionOccurred()) {
            goto exit;
        }
        // 随机端口号
        if (0 == port) {
            // 获取当前绑定的端口号socket
            GetSocketPort(env, obj, serverSocket);
            if (NULL != env->ExceptionOccurred()) {
                goto exit;
            }
        }
        // 客户端地址
        struct sockaddr_in address;
        memset(&address, 0, sizeof(address));

        char buffer[MAX_BUFFER_SIZE];
        ssize_t recvSize;
        ssize_t sentSize;

        // 从socket中接收
        recvSize = ReceiveDatagramFromSocket(env, obj, serverSocket,
                                             &address, buffer, MAX_BUFFER_SIZE);
        if ((0 == recvSize) || (NULL != env->ExceptionOccurred())) {
            goto exit;
        }

        // 发送给socket
        sentSize = SendDatagramToSocket(env, obj, serverSocket,
                                        &address, buffer, MAX_BUFFER_SIZE);
    }

    exit:
    if (serverSocket > 0) {
        close(serverSocket);
    }
}

/*
 * Class:     com_ziv_socket_ClientActivity
 * Method:    nativeStartUdpClient
 * Signature: (Ljava/lang/String;ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_ziv_socket_ClientActivity_nativeStartUdpClient
        (JNIEnv *env, jobject obj, jstring ip, jint port, jstring message) {
    int clientSocket = NewUdpSocket(env, obj);
    if (NULL == env->ExceptionOccurred()) {
        struct sockaddr_in address;

        memset(&address, 0, sizeof(address));
        address.sin_family = PF_INET;

        const char *ipAddress = env->GetStringUTFChars(ip, NULL);
        if (NULL == ipAddress) {
            goto exit;
        }

        // 将IP地址字符串字符串转为网络格式
        int result = inet_aton(ipAddress, &(address.sin_addr));
        env->ReleaseStringUTFChars(ip, ipAddress);
        // 转换失败
        if (0 == result) {
            ThrowErrnoException(env, "java/io/IOException", errno);
            goto exit;
        }
        // 将端口号转换为网络模式
        address.sin_port = htons(port);

        const char *messageText = env->GetStringUTFChars(message, NULL);
        if (NULL == messageText) {
            goto exit;
        }

        jsize messageSize = env->GetStringLength(message);
        // 发送消息给Socket
        SendDatagramToSocket(env, obj, clientSocket, &address, messageText, messageSize);
        env->ReleaseStringUTFChars(message, messageText);
        // 发送失败
        if (NULL != env->ExceptionOccurred()) {
            goto exit;
        }

        char buffer[MAX_BUFFER_SIZE];
        // 清除地址
        memset(&address, 0, sizeof(address));
        // 从socket接收
        ReceiveDatagramFromSocket(env, obj, clientSocket, &address, buffer, MAX_BUFFER_SIZE);
    }

    exit:
    if (clientSocket > 0) {
        close(clientSocket);
    }
}