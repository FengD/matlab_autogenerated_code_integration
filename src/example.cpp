#include "api.h"

#define CAN1_TX_IDLIST_SIZE 5
#define CAN1_TX_IDLIST_SIZE2 10
#define CAN1_RX_IDLIST_SIZE 6

long can1TxIdList[CAN1_TX_IDLIST_SIZE] = {0x101, 0x102, 0x103, 0x104, 0x105};
long can1RxIdList[CAN1_RX_IDLIST_SIZE] = {0x111, 0x112, 0x333, 0x666, 0x232, 0x231};
long can1TxIdList2[CAN1_TX_IDLIST_SIZE2] = {0x300, 0x301, 0x302, 0x303, 0x304, 0x305, 0x306, 0x307, 0x308, 0x309};

void oneStepFunc () {
}

void initializeFunc () {
}

void ternimateFunc () {
}

int main() {
  init();
  createCanMsgInMemByList(1, CAN1_TX_IDLIST_SIZE, can1TxIdList);
  createCanMsgInMemByList(1, CAN1_RX_IDLIST_SIZE, can1RxIdList);
  createCanMsgInMemByList(1, CAN1_TX_IDLIST_SIZE2, can1TxIdList2);

  createAndJoinCanRxThread(1, CAN1_RX_IDLIST_SIZE, can1RxIdList);
  createAndJoinCanTxThread(1, CAN1_TX_IDLIST_SIZE, 1000000, can1TxIdList);
  createAndJoinCanTxThread(1, CAN1_TX_IDLIST_SIZE2, 100000, can1TxIdList2);

  createAndJoinSimulinkThread(20000, &oneStepFunc, &initializeFunc, &ternimateFunc);
  return 0;
}
