#include "orderProcess.h"

#include "socket.h"

#include <string>
/*
  check inventory for the given order. If enough, use different threads to 
  begin packing and ordering a truck. Otherwise, send APurchasemore command to 
  world.
*/
void assign_warehouse(int packid, int client_fd){
    /*
        given packid, get order from db, find the nearest warehouse, update warehouse_id.
        If not found, send "Failed"
        Create an order object, send "ACK", processs.
    */
    unique_ptr<connection> C(Server::connectDB());
    Server* server = Server::GetInstance();

    nontransaction N(*C);
    string query_sql = "SELECT PACK_ID, ADDR_X, ADDR_Y, AMOUNT, PRICE, CUSTOMER_NAME, ITEM_ID, UPS_ID FROM ORDERS WHERE ";
    query_sql += "PACK_ID="+to_string(packid);
    query_sql+=" ;";
    result R(N.exec(query_sql));

    //Server* server = Server::GetInstance();

    int x, y, amount, item_id, ups_id;
    float price;
    string customer_name;
    if(R.empty()==false){
        x=R[0][1].as<int>();
        y=R[0][2].as<int>();
        amount = R[0][3].as<int>();
        price = R[0][4].as<float>();
        customer_name = R[0][5].as<string>();
        item_id = R[0][6].as<int>();
        ups_id = R[0][7].as<int>();
    }else{
        C->disconnect();
        string ackResponse = "FAILED";
        sendMsg(client_fd, ackResponse.c_str(), ackResponse.length());
        close(client_fd);
        return ;
    }
    
    Order order(packid, x, y, amount, price, customer_name, item_id, ups_id);

    vector<Warehouse> warehouses = server->whList;
    vector<int> distances;
    int min_d=-1, min_idx=-1;
    for(int i=0; i<warehouses.size(); i++){
        Warehouse w = warehouses[i];
        int dx = abs(w.x - x);
        int dy = abs(w.y - y);
        int d = dx*dx+dy*dy;
        if(min_d<0 || min_d>d){
            min_d =d;
            min_idx = i;
        }
    }
    cout<<"UPDATE ORDERS\n";
    C->disconnect();
    unique_ptr<connection> D(Server::connectDB());
    work W(*D);
    string sql;
    sql += "UPDATE ORDERS set WH_ID="+to_string(warehouses[min_idx].id);
    sql += " WHERE PACK_ID="+to_string(packid)+";";
    order.setWhID(warehouses[min_idx].id);
    W.exec(sql);

    D->disconnect();
    string ackResponse = "ACK";
    sendMsg(client_fd, ackResponse.c_str(), ackResponse.length());
    close(client_fd);
    cout<<"Process Order\n";
    processOrder(order);

}
void processOrder(const Order & order) {
    unique_ptr<connection> C(Server::connectDB());
    Server* server = Server::GetInstance();
    string logm;
    Log& log = server->GetLog();
    while(1){
        try{
            // check inventory
            int itemId = order.getItemId();
            int itemAmt = order.getAmount();
            int packageId = order.getPackId();
            //int upsid = order.getUPSId();
            int whID = order.getWhID();
            int upsid = order.getUPSId();
            int version = -1;
            bool isEnough = checkInventory(C.get(), itemId, itemAmt, whID, version);

            if(isEnough){
                decreaseInventory(C.get(), whID, itemAmt, itemId, version);
                Server::disConnectDB(C.get());
                cout << "Inventory is enough for order " << order.getPackId() << endl;
                logm = "Inventory is enough for order " + order.getPackId();
                
                log.writeString(logm);
                //create new thread to send APack Command to the world and ASendTruck to the UPS
                //thread t1(packOrder, order);
                //t1.detach();
                server->packOrderQueue.subf_insert(order);

                //thread t2(callATruck, order);
                //t2.detach();
                //server->callTruckQueue.subf_insert(order);
                callATruck(order);
                return;
            }else{
                logm = "Inventory is not enough for order " + order.getPackId();
                //Log& log = server->GetLog();
                log.writeString(logm);
                cout << "Inventory is not enough for order " << order.getPackId()
                    << ", send APurchasemore to world" << endl;
                //put it into order queue(when add inventory, pop an order from this queue)
                server->orderQueue.subf_insert(order);
                // create APurchasemore command
                ACommands ac;
                APurchaseMore * ap = ac.add_buy();
                ap->set_whnum(whID);
                AProduct * aproduct = ap->add_things();
                aproduct->set_id(itemId);
                //buy 100 * itemAmt
                aproduct->set_count(100 * itemAmt);
                //cout << "before get desription\n";
                aproduct->set_description(getDesc(C.get(), itemId));

                //cout << "after get desription\n";
                // add seqNum to this command.
                size_t seqNum = server->getSeqNum();
                ap->set_seqnum(seqNum);

                //cout << "before push word queue \n" << endl;
                //continue push to WordldQueue until get ack(check from seqNumTable)
                pushWorldQueue(ac, seqNum);
                cout << "push word queue successful\n" << endl;
                break;
            }
        }catch(const VersionNotMatch & e){
            std::cerr << e.what() << '\n';
        }
    }
  
}

void processUnenoughOrder(const Order & order) {
    unique_ptr<connection> C(Server::connectDB());
    Server* server = Server::GetInstance();
    string logm;
    Log& log = server->GetLog();
    while(1){
        try{
            // check inventory
            int itemId = order.getItemId();
            int itemAmt = order.getAmount();
            int packageId = order.getPackId();
            //int upsid = order.getUPSId();
            int whID = order.getWhID();
            int upsid = order.getUPSId();
            int version = -1;
            bool isEnough = checkInventory(C.get(), itemId, itemAmt, whID, version);

            if(isEnough){
                decreaseInventory(C.get(), whID, itemAmt, itemId, version);
                Server::disConnectDB(C.get());
                logm = "Inventory is enough for order " + order.getPackId();
                //Log& log = server->GetLog();
                log.writeString(logm);
                cout << "Inventory is enough for order " << order.getPackId() << endl;
                //create new thread to send APack Command to the world and ASendTruck to the UPS
                //thread t1(packOrder, order);
                //t1.detach();
                server->packOrderQueue.subf_insert(order);

                //thread t2(callATruck, order);
                //t2.detach();
                //server->callTruckQueue.subf_insert(order);
                callATruck(order);
                return;
            }
        }catch(const VersionNotMatch & e){
            std::cerr << e.what() << '\n';
        }
    }
  
}

/*
  Send APack command to the world to pack given order.
*/
void packOrder(Order order) {
    cout << "begin pack order " << order.getPackId() << endl;
    Server* server = Server::GetInstance();
    unique_ptr<connection> C(Server::connectDB());

    int whouse_id = order.getWhID();

    // create Apack command
    ACommands acommand;
    APack * apack = acommand.add_topack();
    apack->set_whnum(whouse_id);
    apack->set_shipid(order.getPackId());
    AProduct * aproduct = apack->add_things();
    aproduct->set_id(order.getItemId());
    aproduct->set_count(order.getAmount());
    aproduct->set_description(getDesc(C.get(), order.getItemId()));

    // add seqNum to this command.
    size_t seqNum = server->getSeqNum();
    apack->set_seqnum(seqNum);

    Server::disConnectDB(C.get());
    pushWorldQueue(acommand, seqNum);
    cout<<"already send Apack command to world.\n"; 
}

/*
  Send ASendTruck command to UPS to assign a truck to delivery the package.
*/
void callATruck(Order order) {
  //cout << "begin call a truck for order " << order.getPackId() << endl;
  Server* server = Server::GetInstance();

  // get warehouse information
  int whouse_x = -1;
  int whouse_y = -1;
  int whouse_id = order.getWhID();
  vector<Warehouse> whList = server->whList;
  for (auto wh : whList) {
    if (wh.id == whouse_id) {
      whouse_x = wh.x;
      whouse_y = wh.y;
      break;
    }
  }

  AMessage amessage = AMessage();
  amessage.mutable_sendtruck()->set_package_id(order.getPackId());
  amessage.mutable_sendtruck()->set_warehouse_id(whouse_id);
  amessage.mutable_sendtruck()->set_user_id(order.getUPSId());
  amessage.mutable_sendtruck()->set_x(order.getAddressX());
  amessage.mutable_sendtruck()->set_y(order.getAddressY());
  AItem * item = amessage.mutable_sendtruck()->add_items();

  item->set_description(order.getDescription());
  item->set_count(order.getAmount());
 

  // add seqNum to this command.
  //int seqNum = server->getSeqNum();
  //aOrderTruck->set_seqnum(seqNum);

  //cout << "created amessage sent to ups content: " << amessage.DebugString() << endl;
  pushUpsQueue(amessage);
  cout<<"already send ASendTruck command to UPS.\n"; 
}

/*
    Go to the database to increase the corresponding product inventory in the 
    corresponding warehouse. Then pop a order from orderqueue to process)
*/
void processPurchaseMore(APurchaseMore m) {
  //cout << "begin process purchase more" << endl;
    unique_ptr<connection> C(Server::connectDB());

    //warehouse id
    int whID = m.whnum();
    Server* server = Server::GetInstance();
    string logm;
    Log& log = server->GetLog();

    // all products
    vector<AProduct> products;
    for (int i = 0; i < m.things_size(); i++) {
        products.push_back(std::move(m.things(i)));
    }

    // process each product
    for (int i = 0; i < products.size(); i++) {
        int count = products[i].count();
        int productId = products[i].id();
        addInventory(C.get(), whID, count, productId);
        cout << "Purchased " << count << " products(" << productId
            << ") are arrived at warehouse " << whID << endl;
        logm = "Purchased " + to_string(count) + " products" + to_string(productId) + " are arrived at warehouse " + to_string(whID);
        log.writeString(logm);
    }
    Server::disConnectDB(C.get());

    // process previous saved order
    

    //Order order = server->orderQueue.subf_remove();

    //processOrder(order);
}

/*
    Go to the database and change the order status to 'packed'
*/
void processPacked(APacked m) {
  cout << "begin process packed" << endl;
  //Connect the database
  unique_ptr<connection> C(Server::connectDB());
  Server* server = Server::GetInstance();
    string logm;
    Log& log = server->GetLog();
  //get shipid
  int packageId = m.shipid();

  //process this order status to be 'packed'
  updatePacked(C.get(), packageId);
  cout << "Already pack order " << packageId << endl;
    logm = "Already pack order " + to_string(packageId);
    log.writeString(logm);  

  Server::disConnectDB(C.get());
}

/*
  receive UTruckAtWH Response, keep checking database if the order status is packed 
  if order status is packed, then create APutOnTruck Command and send to World
*/
void processTruckArrived(UTruckAtWH m) {
  //Connect the database
  unique_ptr<connection> C(Server::connectDB());
  Server* server = Server::GetInstance();
  string logm;
    Log& log = server->GetLog();
  //get package id and truck id and warehouse id
  int packageId = m.package_id();
  int truckId = m.truck_id();
  int whId = m.warehouse_id();
  cout << "truck "<<m.truck_id()<<" arrive for order " << packageId << endl;
    updateTruckid(C.get(), packageId, truckId);
    logm = "truck " + to_string(m.truck_id()) + " arrive for order " + to_string(packageId);
    log.writeString(logm);
  //check database if the order status is packed
  while (1) {
    if (checkPacked(C.get(), packageId)) {
      break;
    }
    this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  //create APutOnTruck Command
  cout << "start load order " << packageId << endl;
  ACommands acommand;
  APutOnTruck * aPutOnTruck = acommand.add_load();
  aPutOnTruck->set_whnum(whId);
  aPutOnTruck->set_shipid(packageId);
  aPutOnTruck->set_truckid(truckId);

  // add seqNum to this command.
  //Server* server = Server::GetInstance();
  size_t seqNum = server->getSeqNum();
  aPutOnTruck->set_seqnum(seqNum);

  Server::disConnectDB(C.get());
  pushWorldQueue(acommand, seqNum);
  
  updateLoading(C.get(), packageId);
  
}

/*
    (Send StartDeliver to UPS, change order status to "loaded"
*/
void processLoaded(ALoaded m) {
  //Connect the database
  unique_ptr<connection> C(Server::connectDB());
  Server* server = Server::GetInstance();
  string logm;
  Log& log = server->GetLog();

  //update order status as "loaded"
  int packageId = m.shipid();
  updateLoaded(C.get(), packageId);
  cout << "Already load order " << packageId << endl;
  logm = "Already load order " + to_string(packageId);
  log.writeString(logm);
    int warehouseID = getWarehouseid(C.get(), packageId);
    int truckID = getTruckid(C.get(), packageId);
  // Create StartDeliver Command
  AMessage amessage = AMessage();
  amessage.mutable_truckloaded()->set_package_id(packageId);
  amessage.mutable_truckloaded()->set_truck_id(truckID);
  amessage.mutable_truckloaded()->set_warehouse_id(warehouseID);

  // add seqNum to this command.
//   Server::Ptr server = Server::get_instance();
//   size_t seqNum = server->getSeqNum();
//   aStartDeliver->set_seqnum(seqNum);
    //create ASendTruck Command

  //cout << "created amessage to UPS: " << amessage.DebugString() << endl;
  //cout << "start deliver order " << packageId << endl;
  pushUpsQueue(amessage);
  updateDelivering(C.get(), packageId);
  
  Server::disConnectDB(C.get());
}

/*
  receive Delivered Response, change order status to 'delivered'
*/
void processUDelivered(UPackageDelivered m) {
  unique_ptr<connection> C(Server::connectDB());
  Server* server = Server::GetInstance();
  string logm;
  Log& log = server->GetLog();
  //process this order status to be 'delivered'
  updateDelivered(C.get(), m.package_id());
  cout << "Already delivered order " << m.package_id() << endl;
  logm = "Already delivered order " + to_string(m.package_id());
  log.writeString(logm);
  Server::disConnectDB(C.get());
}

/*
    continue push to WordldQueue until get ack(check from seqNumTable)
*/
void pushWorldQueue(const ACommands & acommand, int seqNum) {
    Server* server = Server::GetInstance();
    int times = 1;
    while (1) {
      cout << times++ << "'s push to worldQueue" << endl;
      server->worldQueue.subf_insert(acommand);
      this_thread::sleep_for(std::chrono::milliseconds(2000));
      if (server->seqNumTable[seqNum] == true)
        break;
      }
}
/*
    continue push to upsQueue until get ack(check from seqNumTable)
*/
// void pushUpsQueue(const AUCommand & aucommand, int seqNum) {
//     Server* server = Server::GetInstance();
//     while (1) {
//     server->upsQueue.subf_insert(aucommand);
//     this_thread::sleep_for(std::chrono::milliseconds(1000));
//     if (server->seqNumTable[seqNum] == true)
//         break;
//     }
// }

/*
    push to upsQueue (just once, no ack needed)
*/
void pushUpsQueue(const AMessage& amessage) {
    Server* server = Server::GetInstance();
    
    server->upsQueue.subf_insert(amessage);
}