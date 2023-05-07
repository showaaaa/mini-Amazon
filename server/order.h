#ifndef _ORDER_H
#define _ORDER_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

class Order{
    private:
        int address_x;
        int address_y;
        int amount;
        int item_id;
        float price;
        string item_description;
        string customer_name;
        int UPS_ID;
        int pack_id;
        int wh_id;
    public:
        Order(){};
        Order( int pack_id, int addr_x, int addr_y, int amount, float price, string customer_name, int item_id, int ups_id):
        pack_id(pack_id), address_x(addr_x) , address_y(addr_y), amount(amount), price(price),
        customer_name(customer_name), item_id(item_id), UPS_ID(ups_id){
            item_description = "No descirption yet";
            //UPS_ID = 2;
            wh_id = -1;
        }
        int getAddressX() const { return address_x; }
        int getAddressY() const { return address_y; }
        int getAmount() const { return amount; }
        int getItemId() const { return item_id; }
        float getPrice() const { return price; }
        int getUPSId() const { return UPS_ID; }
        string getDescription() const { return item_description; }
        string getCustomerName() const {return customer_name;}
        void setPackId(int packid) { pack_id = packid; }
        int getPackId() const { return pack_id; }
        void setWhID(int whID) { wh_id = whID; }
        int getWhID() const { return wh_id; }
        void showOrder() const {
            cout << "------------------------\n";
            cout << "package_id: " << pack_id << endl;
            cout << "item_id: " << item_id << endl;
            cout << "amount: " << amount << endl;
            cout << "address_x: " << address_x << endl;
            cout << "address_y: " << address_y << endl;
            cout << "price: " << price << endl;
            cout << "wh_id: " << wh_id << endl;
            cout << "item_description: " << item_description << endl;
            cout << "UPS_ID: " << UPS_ID << endl;
            cout << "customer name: " << customer_name <<endl;
            cout << "------------------------\n";
        }
};


#endif