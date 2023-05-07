#ifndef _CLIENT_INFO_H
#define _CLIENT_INFO_H

#include <vector>
#include <string>
using namespace std;

class Client_Info{
    private:
        string UUID;
        int fd;
        string ip;
        int packid;
        //string request;
        
        
    public:
        Client_Info(string id, int cfd, string cip, int pid){
            UUID = id;
            fd = cfd;
            ip = cip;
            packid = pid;
        }
        Client_Info(){

        }
        void setUUID(string id){
            UUID = id;
        }
        void setFD(int cfd){
            fd = cfd;
        }
        void setIP(string cip){
            UUID = cip;
        }
        void setPackID(int pid){
            packid = pid;
        }
        // void setRequest(string req){
        //     request = req;
        // }
        string getUUID(){
            return UUID;
        }
        int getFD(){
            return fd;
        }
        string getIP(){
            return ip;
        } 
        int getPackID(){
            return packid;
        }
        // string getRequest(){
        //     return request;
        // }    
};

#endif