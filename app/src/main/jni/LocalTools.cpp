//
// Created by ziv on 2017/7/13.
//

#include "common.h"

/**
 * 构造一个新的原生UNIX socket
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @return socket descriptor
 * @throws IOException
 */
int NewLocalSocket(JNIEnv *env, jobject obj) {
    LogMessage(env, obj, "Constructing a new Local UNIX socket...");
    int localSocket = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (-1 == localSocket) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    }
    return localSocket;
}

/**
 * 将本地 UNIX socket与某一名称绑定
 *
 * @param env JNIEnv
 * @param obj
 * @param sd
 * @param name
 */
void BindLocalSocketToName(JNIEnv *env, jobject obj, int sd, const char *name) {
    struct sockaddr_un address;
    // 名字长度
    const ssize_t nameLength = strlen(name);
    // 路径长度初始化与名称长度相等
    size_t pathLength = nameLength;
    // 如果名字不是以'/'开头，即它在抽象命名空间中(in the abstract namespace)
    bool abstractNamespace = ('/' != name[0]);
    // 抽象命名空间要求目录第一个字节是0字节，更新路径长度包括0字节
    if (abstractNamespace) {
        pathLength++;
    }
    // 检查路径长度
    if (pathLength > sizeof(address.sun_path)) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    } else {
        // 清除地址字节
        memset(&address, 0, sizeof(address));
        address.sun_family = PF_LOCAL;
        // Socket路径
        char *sunPath = address.sun_path;
        // 抽象命名空间要求目录第一个字节是0字节
        if (abstractNamespace) {
            *sunPath++ = NULL;
        }
        // 追加本地名字
        strcpy(sunPath, name);
        // strncpy(sunPath, name, nameLength);
        // 地址长度
        socklen_t addressLength = (offsetof(struct sockaddr_un, sun_path)) + pathLength;
        // 如果socket名已经绑定，取消连接
        unlink(address.sun_path);
        // 绑定Socket
        LogMessage(env, obj, "Binding to local name %s%s.", (abstractNamespace) ? "(null)" : "",
                   name);
        if (-1 == bind(sd, (const sockaddr *) &address, addressLength)) {
            ThrowErrnoException(env, "java/io/IOException", errno);
        }
    }
}

/**
 * 阻塞并等待指定socket上即将到来的客户端连接
 *
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket descriptor
 * @return client socket
 * @throws IOException
 */
int AcceptOnLocalSocket(JNIEnv *env, jobject obj, int sd) {
    // 阻塞并等待即将到来的客户端连接并接受它
    LogMessage(env, obj, "Waiting for a client connection...");
    int clientSocket = accept(sd, NULL, NULL);
    // 客户端socket无效，连接失败
    if (-1 == clientSocket) {
        ThrowErrnoException(env, "java/io/IOException", errno);
    }
    return clientSocket;
}