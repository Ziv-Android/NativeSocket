//
// Created by ziv on 2017/7/4.
//
#include "com_ziv_socket_ServerActivity.h"
#include "com_ziv_socket_ClientActivity.h"
#include "tools.h"

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