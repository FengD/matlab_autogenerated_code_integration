/* -*- mode: C++ -*-
 *
 *  Copyright (C) 2019 Feng DING, Hirain
 *
 *  License: Modified BSD Software License Agreement
 *
 */
 
#include "share_data.h"
#include <iostream>

#define SHM_NAME "CanList"

void initShmMap() {
  struct shm_remove {
    shm_remove() { boost::interprocess::shared_memory_object::remove(SHM_NAME); }
    ~shm_remove(){ boost::interprocess::shared_memory_object::remove(SHM_NAME); }
  } remover;
  boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, SHM_NAME, 65536);
}

void addCanListInShm(long *list, int sizeList) {
  boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, SHM_NAME, 65536);
  ShmHashMap *canMap = segment.find_or_construct<ShmHashMap>(SHM_NAME)(3, boost::hash<KeyType>(), std::equal_to<KeyType>(), segment.get_allocator<ValueType>());
  for(int i = 0; i < sizeList; ++i) {
    ShmHashMap::iterator iter = canMap->find(list[i]);
    if (iter != canMap->end()) {
      std::cout << "id " << list[i] << " already exits." << std::endl;
      continue;
    }
    struct can_frame canFrame;
    memset(canFrame.data, 0, 8);
    canFrame.can_id = list[i];
    canFrame.can_dlc = 8;
    canMap->insert(ValueType(list[i], (MappedType)canFrame));
  }
}

void updateCanFrame(struct can_frame canFrame) {
  boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, SHM_NAME, 65536);
  ShmHashMap *canMap = segment.find_or_construct<ShmHashMap>(SHM_NAME)(3, boost::hash<KeyType>(), std::equal_to<KeyType>(), segment.get_allocator<ValueType>());
  ShmHashMap::iterator iter = canMap->find(canFrame.can_id);
  if (iter != canMap->end()) {
    iter->second = canFrame;
    // memcpy(iter->second, canFrame, sizeof(can_frame));
  }
}

void getCanFrameById(long id, struct can_frame *canFrame) {
  boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, SHM_NAME, 65536);
  ShmHashMap *canMap = segment.find_or_construct<ShmHashMap>(SHM_NAME)(3, boost::hash<KeyType>(), std::equal_to<KeyType>(), segment.get_allocator<ValueType>());
  ShmHashMap::iterator iter = canMap->find(id);
  if (iter != canMap->end()) {
    *canFrame = iter->second;
    // memcpy(canFrame, iter->second, sizeof(can_frame));
  }
}
