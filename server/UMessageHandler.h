#ifndef _AURESPONSEHANDLER_H
#define _AURESPONSEHANDLER_H

#include <vector>

#include "orderProcess.h"
#include "server.h"
#include "protobufCommunication.hpp"

using namespace std;

class UMessageHandler {
 private:
  vector<UTruckAtWH> utruckarrives;
  vector<UPackageDelivered> udelivereds;
  //vector<int> seqNums;

 public:
  UMessageHandler(const UMessage & m);
  ~UMessageHandler() {}
  void handle();

 private:
  //bool checkAndRecord(int seqNum);
};

#endif