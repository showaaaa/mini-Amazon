#ifndef _ORDER_PROCESS_H
#define _ORDER_PROCESS_H

#include "server.h"
#include "sql_helper.h"
#include "Warehouse.h"

void assign_warehouse(int packid, int client_fd);
void processOrder(const Order& order); 
void processPurchaseMore(APurchaseMore m);
void processUnenoughOrder(const Order& order);
void processPacked(APacked m);
void processLoaded(ALoaded m);
void pushWorldQueue(const ACommands& acommand, int seqNum);
void packOrder(Order order);
void processPacked(APacked m);
void processLoaded(ALoaded m);
void processUDelivered(UPackageDelivered m);
void callATruck(Order order);
void pushUpsQueue(const AMessage& amessage);
void processTruckArrived(UTruckAtWH m);

#endif