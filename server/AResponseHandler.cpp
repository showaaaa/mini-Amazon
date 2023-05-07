#include "AResponseHandler.h"

AResponseHandler::AResponseHandler(const AResponses & r) {
  for (int i = 0; i < r.arrived_size(); i++) {
    apurchasemores.push_back(std::move(r.arrived(i)));
    seqNums.push_back(r.arrived(i).seqnum());
  }

  for (int i = 0; i < r.ready_size(); i++) {
    apackeds.push_back(std::move(r.ready(i)));
    seqNums.push_back(r.ready(i).seqnum());
  }

  for (int i = 0; i < r.loaded_size(); i++) {
    aloadeds.push_back(std::move(r.loaded(i)));
    seqNums.push_back(r.loaded(i).seqnum());
  }

  // record acks from world
  for (int i = 0; i < r.acks_size(); i++) {
    Server* server = Server::GetInstance();
    server->seqNumTable[r.acks(i)] = true;
  }
}

/*
  check whether given seqNum has been executed.If yes, return true,
  else return false. If given seqNum is not executed, record it in 
  the executed table.
*/
bool AResponseHandler::checkAndRecord(int seqNum) {
  // check whether this response has been executed

  Server* server = Server::GetInstance();
  if(server->WorldseqNumTable[seqNum] == true){
    return true;
  }
  server->WorldseqNumTable[seqNum] = true;
  return false;
}

/*
    use different threads to handle different type of responses, and ack those messages.
*/
void AResponseHandler::handle() {
    // ACK responses to world.
    ACommands ac;
    for (int i = 0; i < seqNums.size(); i++) {
    ac.add_acks(i);
    ac.set_acks(i, seqNums[i]);
    }
    Server* server = Server::GetInstance();
    server->worldQueue.subf_insert(ac);

    //use different threads to handle different responses.
    for (auto r : apurchasemores) {
        if (checkAndRecord(r.seqnum()) == false) {
            //thread t(processPurchaseMore, r);
            //t.detach();
            processPurchaseMore(r);
        }
    }

    for (auto r : apackeds) {
        if (checkAndRecord(r.seqnum()) == false) {
            //thread t(processPacked, r);
            //t.detach();
            processPacked(r);
        }
    }

    for (auto r : aloadeds) {
        if (checkAndRecord(r.seqnum()) == false) {
            //thread t(processLoaded, r);
            //t.detach();
            processLoaded(r);
        }
    }

}
