#include "share_data.h"
#include <iostream>

#define SHM_NAME "CanList"
#define SHM_NAME0 "CanList0"
#define SHM_NAME1 "CanList1"
#define CAN_HASH_MAP_LIST_SIZE 4096

void deleteShmMap() {
  struct shm_remove {
    shm_remove() {
      boost::interprocess::shared_memory_object::remove(SHM_NAME0);
      boost::interprocess::shared_memory_object::remove(SHM_NAME1);
    }
    ~shm_remove(){
      boost::interprocess::shared_memory_object::remove(SHM_NAME0);
      boost::interprocess::shared_memory_object::remove(SHM_NAME0);
    }
  } remover;
}


void initShmMap(int canInterface) {
  char shmName[10];
	snprintf(shmName, 10, "%s%d", SHM_NAME, canInterface);
  boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, shmName, CAN_HASH_MAP_LIST_SIZE);
}

void addCanListInShm(int canInterface, long *list, int sizeList) {
  char shmName[10];
	snprintf(shmName, 10, "%s%d", SHM_NAME, canInterface);
  boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, shmName, CAN_HASH_MAP_LIST_SIZE);
  ShmHashMap *canMap = segment.find_or_construct<ShmHashMap>(shmName)(3, boost::hash<KeyType>(), std::equal_to<KeyType>(), segment.get_allocator<ValueType>());
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

void updateCanFrame(int canInterface, struct can_frame canFrame) {
  char shmName[10];
	snprintf(shmName, 10, "%s%d", SHM_NAME, canInterface);
  boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, shmName, CAN_HASH_MAP_LIST_SIZE);
  ShmHashMap *canMap = segment.find_or_construct<ShmHashMap>(shmName)(3, boost::hash<KeyType>(), std::equal_to<KeyType>(), segment.get_allocator<ValueType>());
  ShmHashMap::iterator iter = canMap->find(canFrame.can_id);
  if (iter != canMap->end()) {
    iter->second = canFrame;
  }
}

void getCanFrameById(int canInterface, long id, struct can_frame *canFrame) {
  char shmName[10];
	snprintf(shmName, 10, "%s%d", SHM_NAME, canInterface);
  boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, shmName, CAN_HASH_MAP_LIST_SIZE);
  ShmHashMap *canMap = segment.find_or_construct<ShmHashMap>(shmName)(3, boost::hash<KeyType>(), std::equal_to<KeyType>(), segment.get_allocator<ValueType>());
  ShmHashMap::iterator iter = canMap->find(id);
  if (iter != canMap->end()) {
    *canFrame = iter->second;
  }
}
