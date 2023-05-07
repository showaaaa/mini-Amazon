#ifndef _ARESPONSEHANDLER_H
#define _ARESPONSEHANDLER_H

using namespace std;
#include <vector>
#include "server.h"
//#include "orderProcess.h"
#include "protobufCommunication.hpp"
class AResponseHandler {
 private:
  vector<APurchaseMore> apurchasemores;
  vector<APacked> apackeds;
  vector<ALoaded> aloadeds;
  vector<int> seqNums;

 public:
  AResponseHandler(const AResponses & r);
  ~AResponseHandler() {}
  void handle();

 private:
  bool checkAndRecord(int seqNum);
};

#endif