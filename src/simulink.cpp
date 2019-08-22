#include "simulink.h"
#include "boost/function.hpp"

struct timespec interval_simulink_;

boost::function<void()> oneStepFunc_, initializeFunc_, ternimateFunc_;

bool isOneStepFuncSet_ = false, isInitializeFuncSet_ = false, isTernimateFuncSet_ = false;

int interval_time_usec_simulink_ = 20000;

void setOneStepFunc(boost::function<void()> &oneStepFunc) {
  oneStepFunc_ = oneStepFunc;
  isOneStepFuncSet_ = true;
}

void setInitializeFunc(boost::function<void()> &initializeFunc) {
  initializeFunc_ = initializeFunc;
  isInitializeFuncSet_ = true;
}

void setTernimateFunc(boost::function<void()> &ternimateFunc) {
  ternimateFunc_ = ternimateFunc;
  isTernimateFuncSet_ = true;
}

void setExecutePeriod(int usec) {
  interval_time_usec_simulink_ = usec;
}

void rtOneStep(void) {
  if (isOneStepFuncSet_) {
    (oneStepFunc_)();
  }

  // static boolean_T OverrunFlag = false;
  // if (OverrunFlag) {
  //   rtmSetErrorStatus(VisionLaneLocation_20ms_S32V_M, "Overrun");
  //   return;
  // }
  //
  // OverrunFlag = true;
  //
  // // ----------------------- Step function of model
  // VisionLaneLocation_20ms_S32V_V1_step();
  // // ----------------------- Step function of model
  //
  // OverrunFlag = false;
}

void rtInitialize(void) {
  if (isInitializeFuncSet_) {
    (initializeFunc_)();
  }
}

void rtTerminate(void) {
  if (isTernimateFuncSet_) {
    (ternimateFunc_)();
  }
}

void *simulinkThreadFn(void *data) {
  (void)data;
  interval_simulink_.tv_sec = interval_time_usec_simulink_ / 1000000;
	interval_simulink_.tv_nsec = (interval_time_usec_simulink_ * 1000) % 1000000000;
  struct timespec start_time, end_time, elapsed_time, sleep_time;

  rtInitialize();

  while (1) {
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    rtOneStep();

		if (!interval_simulink_.tv_sec && !interval_simulink_.tv_nsec)
			/* Terminate TX thread if TX interval is zero */
			break;

		clock_gettime(CLOCK_MONOTONIC, &end_time);
		calc_time_diff(&start_time, &end_time, &elapsed_time);
		calc_time_diff(&elapsed_time, &interval_simulink_, &sleep_time);

		if (sleep_time.tv_sec < 0) {
			/* Skip sleep if sleep_time is negative */
			printf("Elapsed time ([%6lu.%06ld]) greater than UNPACK interval. Skipping sleep!\n",
			       (unsigned long)elapsed_time.tv_sec,
			       elapsed_time.tv_nsec / 1000);
			continue;
		}

		clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_time, NULL);
  }

  rtTerminate();
  return NULL;
}
