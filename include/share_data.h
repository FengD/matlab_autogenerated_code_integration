/* -*- mode: C++ -*-
 *
 *  Copyright (C) 2019 Feng DING, Hirain
 *
 *  License: Modified BSD Software License Agreement
 *
 */

#ifndef _SHARE_DATA_H_
#define _SHARE_DATA_H_

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <linux/can.h>
#include <linux/can/raw.h>

typedef long KeyType;
typedef struct can_frame MappedType;
typedef std::pair<const long, struct can_frame> ValueType;
typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager> ShmAlloc;
typedef boost::unordered_map<KeyType, MappedType, boost::hash<KeyType>, std::equal_to<KeyType>, ShmAlloc> ShmHashMap;

void initShmMap();
void addCanListInShm(long *list, int sizeList);
void updateCanFrame(struct can_frame canFrame);
void getCanFrameById(long id, struct can_frame *canFrame);

#endif // _SHARE_DATA_H_
