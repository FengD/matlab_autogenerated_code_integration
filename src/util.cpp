/* -*- mode: C++ -*-
 *
 *  Copyright (C) 2019 Feng DING, Hirain
 *
 *  License: Modified BSD Software License Agreement
 *
 *  util
 *
 */

#include "util.h"
#include <string.h>
#include <stdio.h>

int create_thread(pthread_t *thread, void *thread_func(void *), void *arg) {
	struct sched_param param;
	void *stack_buf;
	pthread_attr_t attr;
	int ret;

	stack_buf = mmap(NULL, PTHREAD_STACK_MIN, PROT_READ | PROT_WRITE,
			 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (stack_buf == MAP_FAILED) {
		printf("mmap failed\n");
		ret = -1;
		goto out;
	}
	memset(stack_buf, 0, PTHREAD_STACK_MIN);

	/* Initialize pthread attributes (default values) */
	ret = pthread_attr_init(&attr);
	if (ret) {
		printf("init pthread attributes failed\n");
		goto out;
	}

	/* Set pthread stack to already pre-faulted stack */
	ret = pthread_attr_setstack(&attr, stack_buf, PTHREAD_STACK_MIN);
	if (ret) {
		printf("pthread setstack failed\n");
		goto out;
	}

	/* Set scheduler policy and priority of pthread */
	ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	if (ret) {
		printf("pthread setschedpolicy failed\n");
		goto out;
	}
	param.sched_priority = 80;
	ret = pthread_attr_setschedparam(&attr, &param);
	if (ret) {
		printf("pthread setschedparam failed\n");
		goto out;
	}
	/* Use scheduling parameters of attr */
	ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	if (ret) {
		printf("pthread setinheritsched failed\n");
		goto out;
	}

	/* Create a pthread with specified attributes */
	ret = pthread_create(thread, &attr, thread_func, arg);
	if (ret) {
		printf("create pthread failed\n");
		goto out;
	}

out:
	return ret;
}

void calc_time_diff(const struct timespec *lo, const struct timespec *hi, struct timespec *diff) {
	diff->tv_sec = hi->tv_sec - lo->tv_sec;
	diff->tv_nsec = hi->tv_nsec - lo->tv_nsec;
	if (diff->tv_nsec < 0) {
		diff->tv_sec--;
		diff->tv_nsec += 1000000000;
	}
}
