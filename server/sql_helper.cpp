#include "sql_helper.h"
#include "exception.h"

/*
    Check whether order's one item is enough in inventory. 
    Return true if enough, false if not.
    return version through reference.
*/
bool checkInventory(connection * C, int itemId, int itemAmount, int whID, int & version) {
  //create nontransaction object for SELECT operation
  nontransaction N(*C);

  // create sql statement, we need to select item amount from inventory table
  stringstream sql;
  sql << "SELECT VERSION FROM INVENTORY WHERE "
         "ITEM_ID= "
      << itemId << " AND WH_ID= " << whID 
      << " AND ITEM_AMOUNT >= "<< itemAmount << ";";

  // execute sql statement and get the result set
  result InventoryRes(N.exec(sql.str()));

  // if the result set is empty
  if (InventoryRes.size() == 0) {
    return false;
  }

  
  // get the version from the table
  version = InventoryRes[0][0].as<int>();

  return true;
}


/*
    decrease inventory in the warehouse and check the version of inventory.
    If version is not matched, throw exception.
*/
void decreaseInventory(connection * C, int whID, int count, int productId, int version) {
  work W(*C);
  stringstream sql;
  cout << "start decrease inventory" << endl;
  sql << "UPDATE INVENTORY set ITEM_AMOUNT = INVENTORY.ITEM_AMOUNT-" << count
      << ", VERSION = INVENTORY.VERSION+1"
      << " WHERE ITEM_ID= " << productId << " AND WH_ID= " << whID
      << " AND VERSION= " << version << ";";

  result Updates(W.exec(sql.str()));
  result::size_type rows = Updates.affected_rows();
  if (rows == 0) {
    throw VersionNotMatch(
        "Invalid update: version of this order does not match.\n");
  }
  W.commit();
  //cout << "end decrease inventory" << endl;
}

/*
    add inventory of item in the warehouse and update its version id.
    check if inventory exist this item, if not exist, we need to insert, else we need upadte
*/
void addInventory(connection * C, int whID, int count, int productId) {
  work W(*C);
  stringstream sql;
  cout << "start add inventory" << endl;
  sql << "INSERT INTO INVENTORY (ITEM_ID, ITEM_AMOUNT, WH_ID, VERSION) "
         "VALUES("
      << productId << ", " << count << ", " << whID << ", 1"
      << ") ON CONFLICT (ITEM_ID, WH_ID) DO UPDATE "
         "set ITEM_AMOUNT = INVENTORY.ITEM_AMOUNT+"
      << count << ", VERSION = INVENTORY.VERSION+1"
      << " WHERE INVENTORY.ITEM_ID= " << productId << " AND INVENTORY.WH_ID= " << whID << ";";
  W.exec(sql.str());
  W.commit();
  //cout << "end add inventory" << endl;
}

/*
    get description from the item 
*/
string getDesc(connection * C, int itemId) {
  nontransaction N(*C);

  // create sql statement, we need to select item amount from inventory table
  stringstream sql;
  sql << "SELECT DESCRIPTION FROM ITEM WHERE "
         "ID= "
      << itemId << ";";

  // execute sql statement and get the result set
  result ItemRes(N.exec(sql.str()));
  string description = ItemRes[0][0].as<string>();
  return description;
}

/*
    update specific order status to be 'packed'
*/
void updatePacked(connection * C, int packageId) {
  work W(*C);
  stringstream sql;
  string packed("packed");
  sql << "UPDATE ORDERS set STATUS= " << W.quote(packed)
      << " WHERE PACK_ID= " << packageId << ";";

  W.exec(sql.str());
  W.commit();
}


/*
    check whether the order status is packed
    if packed: return true, if not packed, return false.
*/
bool checkPacked(connection * C, int packageID) {
  work W(*C);
  stringstream sql;

  sql << "SELECT STATUS FROM ORDERS WHERE PACK_ID = " << packageID << ";";
  result statusRes(W.exec(sql.str()));
  string packageStatus = statusRes[0][0].as<string>();

  W.commit();
  if (packageStatus == "packed") {
    return true;
  }
  return false;
}

void updateLoading(connection * C, int packageId) {
  work W(*C);
  stringstream sql;
  string loaded("loading");
  sql << "UPDATE ORDERS set STATUS= " << W.quote(loaded)
      << " WHERE PACK_ID= " << packageId << ";";

  W.exec(sql.str());
  W.commit();
}

/*
    update specific order status to be 'loaded'
*/
void updateLoaded(connection * C, int packageId) {
  work W(*C);
  stringstream sql;
  string loaded("loaded");
  sql << "UPDATE ORDERS set STATUS= " << W.quote(loaded)
      << " WHERE PACK_ID= " << packageId << ";";

  W.exec(sql.str());
  W.commit();
}

void updateTruckid(connection* C, int packageId, int truck_id){
  work W(*C);
  stringstream sql;
  string loaded("loaded");
  sql << "UPDATE ORDERS set TRUCK_ID= " << truck_id
      << " WHERE PACK_ID= " << packageId << ";";

  W.exec(sql.str());
  W.commit();
}

/*
    get truckid from the item 
*/
int getTruckid(connection * C, int packageId) {
  //create nontransaction object for SELECT operation
  nontransaction N(*C);

  // create sql statement, we need to select item amount from inventory table
  stringstream sql;
  sql << "SELECT TRUCK_ID FROM ORDERS WHERE "
         "PACK_ID= "
      << packageId << ";";

  // execute sql statement and get the result set
  result ItemRes(N.exec(sql.str()));
  int truckid = ItemRes[0][0].as<int>();
  return truckid;
}

/*
    get warehouseID from the item 
*/
int getWarehouseid(connection * C, int packageId) {
  nontransaction N(*C);

  stringstream sql;
  sql << "SELECT WH_ID FROM ORDERS WHERE "
         "PACK_ID= "
      << packageId << ";";

  // execute sql statement and get the result set
  result ItemRes(N.exec(sql.str()));
  int warehouseid = ItemRes[0][0].as<int>();
  return warehouseid;
}

/*
    update specific order status to be 'delivering'
*/
void updateDelivering(connection * C, int packageId) {
  work W(*C);
  stringstream sql;
  string loaded("delivering");
  sql << "UPDATE ORDERS set STATUS= " << W.quote(loaded)
      << " WHERE PACK_ID= " << packageId << ";";

  W.exec(sql.str());
  W.commit();
}

/*
    update specific order status to be 'delivered'
*/
void updateDelivered(connection * C, int packageId) {
  work W(*C);
  stringstream sql;
  string delivered("delivered");
  sql << "UPDATE ORDERS set STATUS= " << W.quote(delivered)
      << " WHERE PACK_ID= " << packageId << ";";

  W.exec(sql.str());
  W.commit();
}

