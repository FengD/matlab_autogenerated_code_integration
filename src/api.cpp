#include "api.h"
#include "share_data.h"
#include "can_no_fd.h"
#include "simulink.h"
#include "util.h"
#include <pthread.h>
#include <thread>
#include <boost/bind.hpp>
#include <boost/function.hpp>

typedef boost::function<void(void)> Func;

struct thread_arg {
	long *idList;
	int interface;
	int idListSize;
	int timePeriodUsec;
};

CAN_MESSAGE getSimulinkCanMsgByIdInMem(long id) {
	struct can_frame *canFrame;
	CAN_MESSAGE canmsg;
	canFrame = (struct can_frame*) malloc(sizeof(can_frame));
	getCanFrameById(id, canFrame);
	canmsg.ID = id;
  memcpy(canmsg.Data, canFrame->data, canFrame->can_dlc);
  canmsg.Length = canFrame->can_dlc;
  canmsg.Extended = 0;
	return canmsg;
}

void updateMemBySimulinkCanMsg(CAN_MESSAGE *canmsg) {
	struct can_frame canFrame;
	canFrame.can_id = canmsg->ID;
	canFrame.can_dlc = canmsg->Length;
	memcpy(canFrame.data, canmsg->Data, canmsg->Length);
	updateCanFrame(canFrame);
}

void printSimulinkCanMsg(CAN_MESSAGE *canmsg) {
	printf("(HEX)id: %x ", canmsg->ID);
  for (int i = 0; i < canmsg->Length; i++) {
    printf("%d ", canmsg->Data[i]);
  }
  printf("\n");
}

void createAndJoinSimulinkThread( void (*oneStepFunc)(), void (*initializeFunc)(), void (*ternimateFunc)(), int timePeriodUsec) {
  Func oneStepFuncF(boost::bind(oneStepFunc));
  Func initializeFuncF(boost::bind(initializeFunc));
  Func ternimateFuncF(boost::bind(ternimateFunc));
  setOneStepFunc(oneStepFuncF);
  setInitializeFunc(initializeFuncF);
  setTernimateFunc(ternimateFuncF);
  setExecutePeriod(timePeriodUsec);

  pthread_t simulinkThread;
  memset(&simulinkThread, 0, sizeof(simulinkThread));
  if (pthread_create(&simulinkThread, NULL, simulinkThreadFn, NULL) == 0) {
    printf("simulink thread successfully created!\n");
  } else {
    printf("simulink thread created on error!\n");
  }

  if (simulinkThread != 0) {
    pthread_join(simulinkThread, NULL);
  }
}

void init() {
  initShmMap();
}

void createCanMsgInMemByList(long *idList, int idListSize) {
  addCanListInShm(idList, idListSize);
}

void *canRxThreadFn(void *args) {
	struct thread_arg *argsThread = (struct thread_arg *) args;
	// printf("Rx interface: %d, idSize:%d\n", argsThread->interface, argsThread->idListSize);
	struct can_hdl *canRx_hdl;
	canRx_hdl = (struct can_hdl*) malloc(sizeof(can_hdl));

	struct can_cfg *cfg;
  cfg = (struct can_cfg*) malloc(sizeof(struct can_cfg));

	if (argsThread->idListSize > 0) {
    struct can_filter *canFilterCan;
    canFilterCan = (struct can_filter*) malloc(argsThread->idListSize * sizeof(struct can_filter));
    for(int i = 0; i < argsThread->idListSize; i++) {
      canFilterCan[i].can_id = argsThread->idList[i];
      canFilterCan[i].can_mask = CAN_SFF_MASK;
    }

    cfg->rx_filter = (struct can_filter*) malloc(argsThread->idListSize * sizeof(struct can_filter));
    memcpy(cfg->rx_filter, canFilterCan, argsThread->idListSize * sizeof(struct can_filter));
    cfg->rx_filter_len = argsThread->idListSize;
  }
	ssize_t rdlen;
  char device[10];
  sprintf(device, "can%d", argsThread->interface);
  cfg->ifname = device;
  can_open(&canRx_hdl, cfg);

	struct can_frame *can_rx_frame;
  can_rx_frame = (struct can_frame*) malloc(sizeof(can_frame));
	memset(can_rx_frame, 0, sizeof(can_frame));

	for (;;) {
		rdlen = can_read(canRx_hdl, can_rx_frame);
		if (rdlen < 0) {
			printf("can_read errno %d\n", errno);
			break;
		}
		updateCanFrame(*can_rx_frame);
	}
	return NULL;
}

void createAndJoinCanRxThread(int interface, long *idList, int idListSize) {
	pthread_t canRxThread;
	// it should be pointer
	struct thread_arg *argsThread;
	argsThread = (thread_arg *) malloc(sizeof(thread_arg));
	argsThread->interface = interface;
	argsThread->idListSize = idListSize;
	argsThread->timePeriodUsec = 0;
	argsThread->idList = (long*) malloc(idListSize * sizeof(long));
	memcpy(argsThread->idList, idList, idListSize * sizeof(long));

	if (pthread_create(&canRxThread, NULL, canRxThreadFn, (void*) argsThread) == 0) {
		printf("canRx thread successfully created!\n");
	} else {
		printf("canRx thread created on error!\n");
	}
}

void *canTxThreadFn(void *args) {
	struct thread_arg *argsThread = (struct thread_arg *) args;
	// printf("Tx interface: %d, idSize:%d\n", argsThread->interface, argsThread->idListSize);
	struct timespec interval_canTx;
  interval_canTx.tv_sec = argsThread->timePeriodUsec / 1000000;
	interval_canTx.tv_nsec = (argsThread->timePeriodUsec * 1000) % 1000000000;

  struct can_hdl *canTx_hdl;
	canTx_hdl = (struct can_hdl*) malloc(sizeof(can_hdl));

	struct can_cfg *cfg;
  cfg = (struct can_cfg*) malloc(sizeof(struct can_cfg));

  if (argsThread->idListSize > 0) {
    struct can_filter *canFilterCan;
    canFilterCan = (struct can_filter*) malloc(argsThread->idListSize * sizeof(struct can_filter));
    for(int i = 0; i < argsThread->idListSize; i++) {
      canFilterCan[i].can_id = argsThread->idList[i];
      canFilterCan[i].can_mask = CAN_SFF_MASK;
    }

    cfg->rx_filter = (struct can_filter*) malloc(argsThread->idListSize * sizeof(struct can_filter));
    memcpy(cfg->rx_filter, canFilterCan, argsThread->idListSize * sizeof(struct can_filter));
    cfg->rx_filter_len = argsThread->idListSize;
  }

  char device[10];
  sprintf(device, "can%d", argsThread->interface);
  cfg->ifname = device;
  can_open(&canTx_hdl, cfg);

	struct can_frame *can_tx_frame;
  can_tx_frame = (struct can_frame*) malloc(sizeof(can_frame));
	memset(can_tx_frame, 0, sizeof(can_frame));
	struct timespec start_time, end_time, elapsed_time, sleep_time;
	for (;;) {
		clock_gettime(CLOCK_MONOTONIC, &start_time);
		// canmsg list send
		for (int i = 0; i < argsThread->idListSize; i++) {
			getCanFrameById(argsThread->idList[i], can_tx_frame);
			can_write(canTx_hdl, can_tx_frame);
		}

		if (!interval_canTx.tv_sec && !interval_canTx.tv_nsec)
			/* Terminate TX thread if TX interval is zero */
			break;

		clock_gettime(CLOCK_MONOTONIC, &end_time);

		calc_time_diff(&start_time, &end_time, &elapsed_time);
		calc_time_diff(&elapsed_time, &interval_canTx, &sleep_time);

		if (sleep_time.tv_sec < 0) {
			/* Skip sleep if sleep_time is negative */
			printf("Elapsed time ([%6lu.%06ld]) greater than TX interval. Skipping sleep!\n",
						 (unsigned long)elapsed_time.tv_sec,
						 elapsed_time.tv_nsec / 1000);
			continue;
		}

		clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_time, NULL);
	}
	return NULL;
}

void createAndJoinCanTxThread(int interface, long *idList, int idListSize, int timePeriodUsec) {
	pthread_t canTxThread;
	memset(&canTxThread, 0, sizeof(canTxThread));

	// it should be pointer
	struct thread_arg *argsThread;
	argsThread = (thread_arg *) malloc(sizeof(thread_arg));
	argsThread->interface = interface;
	argsThread->idListSize = idListSize;
	argsThread->timePeriodUsec = timePeriodUsec;
	argsThread->idList = (long*) malloc(idListSize * sizeof(long));
	memcpy(argsThread->idList, idList, idListSize * sizeof(long));

  if (pthread_create(&canTxThread, NULL, canTxThreadFn, (void*) argsThread) == 0) {
    printf("canTx thread successfully created!\n");
  } else {
    printf("canTx thread created on error!\n");
  }
}
