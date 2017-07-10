//
// Created by ziv on 17-6-28.
//
#include "tools.h"

/**
 * 信息格式化并调用logMessage方法输出
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param format message format and arguments
 */
void LogMessage(JNIEnv *env, jobject obj, const char *format, ...) {
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
void ThrowException(JNIEnv *env, const char *className, const char *message) {
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
void ThrowErrnoException(JNIEnv *env, const char *className, int errnum) {
    char buffer[MAX_LOG_MESSAGE_LENGTH];

    // 获取错误号消息
    if (-1 == strerror_r(errnum, buffer, MAX_LOG_MESSAGE_LENGTH)) {
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
int NewTcpSocket(JNIEnv *env, jobject obj) {
    // 构造socket
    LogMessage(env, obj, "Constructing a new TCP socket...");
    int tcpSocket = socket(PF_INET, SOCK_STREAM, 0);

    // check socket
    if (-1 == tcpSocket) {
        ThrowErrnoException(env, "java/io/IOException", errno);
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
void BindSocketToPort(JNIEnv *env, jobject obj, int sd, unsigned short port) {
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
        ThrowErrnoException(env, "java/io/IOException", errno);
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
unsigned short GetSocketPort(JNIEnv *env, jobject obj, int sd) {
    unsigned short port = 0;
    struct sockaddr_in address;
    socklen_t addressLength = sizeof(address);

    // 获取Socket地址
    if (-1 == getsockname(sd, (sockaddr *) &address, &addressLength)) {
        ThrowErrnoException(env, "java/io/IOException", errno);
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
void ListenOnSocket(JNIEnv *env, jobject obj, int sd, int backlog) {
    // 监听给定backlog的socket
    LogMessage(env, obj, "Listening on socket with a backlog of %d pending connections.", backlog);
    if (-1 == listen(sd, backlog)) {
        ThrowErrnoException(env, "java/io/IOException", errno);
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
void LogAddress(JNIEnv *env, jobject obj,
                const char *message, const struct sockaddr_in *address) {
    char ip[INET_ADDRSTRLEN];
    // 将IP地址转换为字符串
    if (NULL == inet_ntop(PF_INET, &(address->sin_addr), ip, INET_ADDRSTRLEN)) {
        ThrowErrnoException(env, "java/io/IOException", errno);
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
        if (sendSize > 0){
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
void ConnectToAddress(JNIEnv *env, jobject obj, int sd, const char* ip, unsigned short port){
    LogMessage(env, obj, "Connecting to %s:%uh ...", ip, port);
    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));
    address.sin_family = PF_INET;

    // 将IP地址字符串转换为网络地址
    if (0 == inet_aton(ip, &(address.sin_addr))){
        ThrowErrnoException(env, "java/io/IOException", errno);
    } else {
        // 将端口号转换为网络字节顺序
        address.sin_port = htons(port);
        // 转换为地址，connect方法通过提供的协议地址连接socket和server socket
        if (-1 == connect(sd, (const sockaddr *) &address, sizeof(address))){
            ThrowErrnoException(env, "java/io/IOException", errno);
        } else {
            LogMessage(env, obj, "Connected.");
        }
    }
}