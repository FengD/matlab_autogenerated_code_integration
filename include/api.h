/* -*- mode: C++ -*-
 *
 *  Copyright (C) 2019 Feng DING, Hirain
 *
 *  License: Modified BSD Software License Agreement
 *
 *  The api is used to execute the simulink model as embedded code in ubuntu system
 *
 */


#ifndef _API_H_
#define _API_H_
// the header of simulink can struct model
#include "can_message.h"

/**
* @Brief The methode which should be execute as the first methode in main.
* Init the can message list in the memory.
*
*/
void init();

/**
* @Brief Allocate memory to stock each can message in certain place.
* @param [array] idList
* @param [int] size of the list
*/
void createCanMsgInMemByList(long *idList, int idListSize);

/**
* @Brief get the simulink CAN_MESSAGE Type by id
* @param [long] id of the message
* @return [struct] can message
*/
CAN_MESSAGE getSimulinkCanMsgByIdInMem(long id);

/**
* @Brief update the can message in the memory
* @param [struct] can message
*/
void updateMemBySimulinkCanMsg(CAN_MESSAGE *canmsg);

/**
* @Brief print message in the terminal
* @param [struct] can message
*/
void printSimulinkCanMsg(CAN_MESSAGE *canmsg);

/**
* @Brief Create a thread of can receive
* @param [int] the interface of socket can. should be 0, 1, etc.
* @param [array] the list of ids of can message which accepted
* @param [array] the size of the list
*/
void createAndJoinCanRxThread(int interface, long *idList, int idListSize);

/**
* @Brief Create a thread of can transmit
* @param [int] the interface of socket can. should be 0, 1, etc.
* @param [array] the list of ids of can message which accepted
* @param [array] the size of the list
* @param [int] the period of can transmit. The unit is usec.
*/
void createAndJoinCanTxThread(int interface, long *idList, int idListSize, int timePeriodUsec);

/**
* @Brief Create a thread of simulink model execution.
* @param [function] step methode of the loop
* @param [function] init methode
* @param [function] ternimate methode
* @param [int] the period of simulink step model execution. The unit is usec.
*/
void createAndJoinSimulinkThread( void (*oneStepFunc)(), void (*initializeFunc)(), void (*ternimateFunc)(), int timePeriodUsec);

#endif // _UTIL_H_
