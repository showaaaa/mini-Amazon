#ifndef _SQL_HELPER_H
#define _SQL_HELPER_H

#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include "order.h"
//#include "exception.h"
using namespace pqxx;
using namespace std;

bool checkInventory(connection * C, int itemId, int itemAmount, int whID, int & version);
void addInventory(connection * C, int whID, int count, int productId);
void decreaseInventory(connection * C, int whID, int count, int productId, int version);
string getDesc(connection * C, int itemId);
void updatePacked(connection * C, int packageId);

bool checkPacked(connection * C, int packageID);
void updateLoading(connection * C, int packageId);
void updateLoaded(connection * C, int packageId);
void updateTruckid(connection* C, int packageId, int truck_id);
int getTruckid(connection * C, int packageId);
int getWarehouseid(connection * C, int packageId);
void updateDelivering(connection * C, int packageId);
void updateDelivered(connection * C, int packageId);

#endif