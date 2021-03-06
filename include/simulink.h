/* -*- mode: C++ -*-
 *
 *  Copyright (C) 2019 Feng DING, Hirain
 *
 *  License: Modified BSD Software License Agreement
 *
 */

#ifndef _SIMULINK_H_
#define _SIMULINK_H_

#include <sys/shm.h>
#include <stddef.h>
#include <stdio.h>                     /* This ert_main.c example uses printf/fflush */
#include <pthread.h>
#include <boost/function.hpp>
#include "util.h"
#include "rtwtypes.h"

void setOneStepFunc(boost::function<void()> &oneStepFunc);

void setInitializeFunc(boost::function<void()> &initializeFunc);

void setTernimateFunc(boost::function<void()> &ternimateFunc);

void setExecutePeriod(int usec);

void rtOneStep(void);

void rtInitialize(void);

void rtTerminate(void);

void *simulinkThreadFn(void *data);

#endif // _SIMULINK_H_
