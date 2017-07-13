//
// Created by ziv on 17-6-27.
//
#include <common.h>

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

#define EXCEPTION_CLASS "java/lang/RuntimeException"
/**
 *
 * @param env JNIEnv interface
 * @param msg message text
 * @return
 */
int throwNewException(JNIEnv *env, const char *msg) {
    // 抛出异常
    jclass exceptionClazz = env->FindClass(EXCEPTION_CLASS);
    return env->ThrowNew(exceptionClazz, msg);
}