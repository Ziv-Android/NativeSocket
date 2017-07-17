//
// Created by ziv on 2017/7/13.
//
#ifndef NATIVESOCKET_LOCALTOOLS_H
#define NATIVESOCKET_LOCALTOOLS_H

#include "common.h"

int NewLocalSocket(JNIEnv *env, jobject obj);

void BindLocalSocketToName(JNIEnv *env, jobject obj, int sd, const char *name);

int AcceptOnLocalSocket(JNIEnv *env, jobject obj, int sd);

#endif //NATIVESOCKET_LOCALTOOLS_H
