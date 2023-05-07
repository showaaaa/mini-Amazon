#include "server.h"
#include "orderProcess.h"
#include "protobufCommunication.hpp"
#include "socket.h"
#include "sql_helper.h"
Server * Server::g_pServer = new (std::nothrow) Server;

Server* Server::GetInstance(){
    return g_pServer;
}

Log& Server::GetLog(){
    return log;
}

void Server::deleteInstance(){
    if(g_pServer){
        delete g_pServer;
        g_pServer = NULL;
    }
}

void Server::Print(){
    cout << "instance address is: " << this << endl;
}

Server::Server(): worldQueue(SBUFSIZE), orderQueue(SBUFSIZE), upsQueue(SBUFSIZE), frontendClientQueue(SBUFSIZE), packOrderQueue(SBUFSIZE), receivedAResponseQueue(SBUFSIZE), receivedUMessageQueue(SBUFSIZE){
    
    cout << "constructor" << endl;
    worldHostName = "vcm-30937.vm.duke.edu";
    worldPortNum = "23456";
    upsHostName = "vcm-30717.vm.duke.edu"; //testing with heba
    //upsHostName = "172.28.184.254";
    upsPortNum = "54321";
    amazon_listenport = "6543";
    n_warehouse = 4;
    portNum = "9999";


    curSeqNum = 0;
    for (size_t i = 0; i < MAXSEQNUM; i++) {
        seqNumTable.push_back(false);
        WorldseqNumTable.push_back(false);
        //UPSseqNumTable.push_back(false);
    }
}

Server::~Server(){
    cout << "deconstructor" << endl;
}

void Server::run(){
    try{
        //thread processFrontendClient(assign_warehouse);
        for(int i = 0; i < NTHREADS; i++){
            thread processFrontendClient(&Server::processFrontendClient, this);
            processFrontendClient.detach();
            thread packOrderThread(&Server::PACKOrder, this);
            packOrderThread.detach();
            // thread callTruckThread(&Server::CALLTruck, this);
            // callTruckThread.detach();
            thread receivedAResponseThread(&Server::RECEIVEAResponse, this);
            receivedAResponseThread.detach();
            thread processUnenoughOrderThread(&Server::PROCESSUnenoughOrder, this);
            processUnenoughOrderThread.detach();
            thread receivedUMessageThread(&Server::RECEIVEUMessage, this);
            receivedUMessageThread.detach();
        }

        
        getWorldIDFromUPS_handshake();
        thread recvWorld(&Server::keepReceivingFromWorld, this);
        thread recvUPS(&Server::keepReceivingFromUps,this);
        thread sendWorld(&Server::keepSendingToWorld, this);
        thread sendUPS(&Server::keepSendingToUps, this);
        acceptOrderRequest();
    }
    catch(const std::exception & e){
        std::cerr << e.what() << '\n';
        close(ups_fd);
        close(world_fd);
        close(ups_fd2);
        string logm = "exception occur, server down";
        log.writeString(logm);
        return;
    }
}
void Server::acceptOrderRequest() {
  // create server socket, listen to port.
  int listenfd = open_listenfd(portNum.c_str());

  int requestUUID = 0;
  // keep receving orders request from front-end web.
  while (1) {
    // wait to accept connection.
    int client_fd;
    string ip;
    try {
      client_fd = accept_getip(listenfd, &ip);
    }
    catch (const std::exception & e) {
      std::cerr << e.what() << '\n';
      continue;
    }

    // receive request.
    string msg;
    try {
      msg = recvMsg(client_fd);
    }
    catch (const std::exception & e) {
      std::cerr << e.what() << '\n';
      continue;
    }
    int packid = stoi(msg);


    //Client_Info info(to_string(requestUUID), clientfd, ip);                                            //line:netp:tiny:close
    //printf("infofd:%d\n", info.getFD());
    //sbuf.subf_insert(info);
    Client_Info info(to_string(requestUUID), client_fd, ip, packid);
    frontendClientQueue.subf_insert(info);
    requestUUID++;
  }
}
void Server::getWorldIDFromUPS_handshake(){
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    int listenfd = open_listenfd(amazon_listenport.c_str());
    string ip;
    int times0 = 0;
    while(1){
        try{
            cout << ++times0 <<"try to accept connection from UPS" << endl;
            ;
            string logm = to_string(times0) + " try to accept connection from UPS";
            log.writeString(logm);
            ups_fd = accept_getip(listenfd, &ip);
            break;
        }
        catch(const std::exception & e){
            std::cerr << e.what() << '\n';
            continue;
        }
    }
    //ups_fd = accept_getip(listenfd, &ip);
    // 1) get worldID from UPS
    UtoAzConnect uazc;
    unique_ptr<socket_in> in1(new socket_in(ups_fd));
    if(recvMesgFrom<UtoAzConnect>(uazc, in1.get()) == false){
        throw Failure("fail to recv UtoAzConnect from UPS for worldID.");
    }

    worldID = uazc.worldid();
    cout << "worldID: " << worldID << endl;

    // 2) 1.connect to world
    int times = 0;
    while(1){

        try{
            cout << ++times <<"try to connect to world" << endl;
            world_fd = open_clientfd(worldHostName.c_str(), worldPortNum.c_str());
            break;
        }
        catch(const std::exception & e){
            std::cerr << e.what() << '\n';
            continue;
        }
    }
    
    //world_fd = open_clientfd(worldHostName.c_str(), worldPortNum.c_str());
    AConnect ac;
    
    AInitWarehouse * w1 = ac.add_initwh();
    w1->set_id(0);
    w1->set_x(1);
    w1->set_y(1);
    whList.push_back(Warehouse(1,1,0));
    AInitWarehouse * w2 = ac.add_initwh();
    w2->set_id(1);
    w2->set_x(1);
    w2->set_y(-1);
    whList.push_back(Warehouse(1,-1,1));
    AInitWarehouse * w3 = ac.add_initwh();
    w3->set_id(2);
    w3->set_x(-1);
    w3->set_y(1);
    whList.push_back(Warehouse(-1,1,2));
    AInitWarehouse * w4 = ac.add_initwh();
    w4->set_id(3);
    w4->set_x(-1);
    w4->set_y(-1);
    whList.push_back(Warehouse(-1,-1,3));
    ac.set_worldid(worldID);
    ac.set_isamazon(true);
    //send AConnect to world
    unique_ptr<socket_out> out2(new socket_out(world_fd));
    if (sendMesgTo<AConnect>(ac, out2.get()) == false) {
        throw Failure("fail to send AConnect to world.");
    }

    //receive AConnected
    AConnected aced;
    unique_ptr<socket_in> in2(new socket_in(world_fd));
    if (recvMesgFrom<AConnected>(aced, in2.get()) == false) {
        throw Failure("fail to recv AConnected from world.");
    }

    //check AConnected request
    if (aced.result() != "connected!") {
        throw Failure("fail to initialize the world.");
    }

    // 2) 2. send AzConnected to ups to indicate the Amazon connect to world
    AzConnected azced;
    azced.set_worldid(worldID);
    azced.set_result("success");

    ups_fd2 = open_clientfd(upsHostName.c_str(), upsPortNum.c_str());

    unique_ptr<socket_out> out1(new socket_out(ups_fd2));
    if (sendMesgTo<AzConnected>(azced, out1.get()) == false) {
        throw Failure("fail to send AZconnected to UPS.");
    }

    cout << "handshake success" << endl;
    string logm = "handshake success";
    log.writeString(logm);

}

connection * Server::connectDB(){
    connection *C;
    
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("host=db dbname=comment user=postgres password=1234 port=5432");
    if (C->is_open()) {
        cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
        cout << "Can't open database" << endl;
        //throw failure??? may already throw by original function
        throw Failure("Can't open database");
    }
    return C;
}

void Server::disConnectDB(connection * C) {
  C->disconnect();
}

size_t Server::getSeqNum() {
    lock_guard<mutex> lck(seqNum_lck);
    size_t res = curSeqNum;
    curSeqNum++;
    return res;
}

void Server::keepReceivingFromWorld(){
    unique_ptr<socket_in> world_in(new socket_in(world_fd));
    while (1) {
        AResponses r;
        if (recvMesgFrom<AResponses>(r, world_in.get()) == false) {
            continue;
        }
        cout << "receive AResponses from world: " << r.DebugString() << endl;
        //AResponseHandler h(r);
        //h.handle();
        receivedAResponseQueue.subf_insert(r);
  }
}

/*
  keep receiving UMessage from ups. 
*/
void Server::keepReceivingFromUps() {
  unique_ptr<socket_in> ups_in(new socket_in(ups_fd));
  while (1) {
    UMessage r;
    if (recvMesgFrom<UMessage>(r, ups_in.get()) == false) {
      continue;
    }
    cout << "receive UMessage from UPS:" << r.DebugString() << endl;
    //UMessageHandler h(r);
    //h.handle();
    receivedUMessageQueue.subf_insert(r);
  }
}

/*
  keep sending ACommand from worldQueue to world.
*/
void Server::keepSendingToWorld() {
    unique_ptr<socket_out> out2(new socket_out(world_fd));
    while (1) {
        ACommands msg = worldQueue.subf_remove();
        cout << "content to world: "<< msg.DebugString() << endl;
        if (sendMesgTo(msg, out2.get()) == false) {
            throw Failure("fail to send message to world. i think it need to reconnect");
        }
    }
}

/*
  keep sending AUCommand from upsQueue to UPS.
*/
void Server::keepSendingToUps() {
  unique_ptr<socket_out> out(new socket_out(ups_fd2));
  while (1) {
    AMessage msg = upsQueue.subf_remove();

    cout << "content to ups: "<< msg.DebugString() << endl;
    if (sendMesgTo(msg, out.get()) == false) {
      throw Failure("fail to send AMessage to ups.");
    }
  }
}

void Server::processFrontendClient(){
  while(1){
    Client_Info info = frontendClientQueue.subf_remove();
    assign_warehouse(info.getPackID(), info.getFD());
  }
}

void Server::PACKOrder(){
  while(1){
    
    Order order = packOrderQueue.subf_remove();
    packOrder(order);
  }
}

// void Server::CALLTruck(){
//   while(1){
    
//     Order order = callTruckQueue.subf_remove();
//     callATruck(order);
//   }
// }

void Server::RECEIVEAResponse(){
  while(1){
    
    //Order order = callTruckQueue.subf_remove();
    //callATruck(order);
    AResponses msg = receivedAResponseQueue.subf_remove();
    AResponseHandler handler(msg);
    handler.handle();
  }
}

void Server::RECEIVEUMessage(){
  while(1){
    
    //Order order = callTruckQueue.subf_remove();
    //callATruck(order);
    //AResponses msg = receivedAResponseQueue.subf_remove();
    //AResponseHandler handler(msg);
    UMessage msg = receivedUMessageQueue.subf_remove();
    UMessageHandler handler(msg);
    handler.handle();
  }
}

void Server::PROCESSUnenoughOrder(){
  while(1){
    Order order = orderQueue.subf_remove();
    processUnenoughOrder(order);
  }
}

