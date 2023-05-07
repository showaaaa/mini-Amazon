#include "UMessageHandler.h"

UMessageHandler::UMessageHandler(const UMessage & m) {
  if(m.has_truckatwh()){
    utruckarrives.push_back(std::move(m.truckatwh()));
  }

  if(m.has_packagedelivered()){
    udelivereds.push_back(std::move(m.packagedelivered()));
  }
  
  // for (int i = 0; i < r.arrive_size(); i++) {
  //   utruckarrives.push_back(std::move(r.arrive(i)));
  //   seqNums.push_back(r.arrive(i).seqnum());
  // }
  // for (int i = 0; i < r.delivered_size(); i++) {
  //   udelivereds.push_back(std::move(r.delivered(i)));
  //   seqNums.push_back(r.delivered(i).seqnum());
  // }

  // // record acks from ups
  // for (int i = 0; i < r.acks_size(); i++) {
  //   Server* server = Server::GetInstance();
  //   server->seqNumTable[r.acks(i)] = true;
  // }
}

/*
  check whether given seqNum has been executed.If yes, return true,
  else return false. If given seqNum is not executed, record it in 
  the executed table.
*/
// bool UMessageHandler::checkAndRecord(int seqNum) {
//   // check whether this response has been executed

//   Server* server = Server::GetInstance();
//   if(server->UPSseqNumTable[seqNum] == true){
//     return true;
//   }
//   server->UPSseqNumTable[seqNum] = true;
//   return false;
// }


void UMessageHandler::handle() {
  // ACK responses to UPS.
  // AUCommand aucommand;
  // for (int i = 0; i < seqNums.size(); i++) {
  //   aucommand.add_acks(i);
  //   aucommand.set_acks(i, seqNums[i]);
  // }
  // Server* server = Server::GetInstance();
  // server->upsQueue.subf_insert(aucommand);

  // use different threads to handle different responses.
  for (auto r : utruckarrives) {
    //cout << "start handle truckarrive" << endl;
    
    processTruckArrived(r);
  }

  for (auto r : udelivereds) {
    //cout << "start handle udelivered" << endl;
    processUDelivered(r);
  }
}