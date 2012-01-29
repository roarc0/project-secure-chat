#include <pthread.h>
#include <cstdlib>
#include <errno.h>
#include <cstdio>

#ifndef THREAD_H
#define THREAD_H


using namespace std;

pthread_t start_thread(void* (*)(void*), void*);
//bool kill_thread(tid);

#endif
