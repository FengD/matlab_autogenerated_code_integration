/* -*- mode: C++ -*-
 *
 *  Copyright (C) 2019 Feng DING, Hirain
 *
 *  License: Modified BSD Software License Agreement
 *
 *  util
 *
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>

int create_thread(pthread_t *thread, void *thread_func(void *), void *arg);

void calc_time_diff(const struct timespec *lo, const struct timespec *hi, struct timespec *diff);

#endif // _UTIL_H_
