#ifndef _SERVER_H
#define _SERVER_H

#include <thread>
#include <stdio.h>
#include <mutex>
#include "sbuf.h"
#include "order.h"
#include "client_info.h"
//#include "request.h"
//#include "response.h"
//#include "socket.h"
//#include "tinyxml2.h"
#include "sql_helper.h"
#include "log.h"
//#include <pqxx/pqxx>
#include "protobufCommunication.hpp"
#include "world_amazon.pb.h"
#include "AResponseHandler.h"
#include "UMessageHandler.h"
#include "Warehouse.h"
#include "exception.h"
//#include "orderProcess.h"
#define NTHREADS 80
#define SBUFSIZE 5000
//#define MAXLINE 8192
#define MAXLINE 65536
#define MAXSEQNUM 65536
using namespace pqxx;
//using namespace tinyxml2;


class Server{
    // 饿汉单例
    public:
        
        static Server* GetInstance();
        Log& GetLog();
        
        static void deleteInstance();
        
        void Print();
        vector<Warehouse> whList; 
        static connection * connectDB();
        static void disConnectDB(connection * C);
        size_t getSeqNum();
        //getWarehouse();
    private:
        
        Server();
        ~Server();
        
        Server(const Server &signal);
        const Server &operator=(const Server &signal);
        void getWorldIDFromUPS_handshake();
        void keepSendingToWorld();
        void keepSendingToUps();
        void keepReceivingFromUps();
        void keepReceivingFromWorld();
        void receiveOrderFromFrontEnd();
        void acceptOrderRequest();
        void processFrontendClient();
        void PACKOrder();
        //void CALLTruck();
        void RECEIVEAResponse();
        void PROCESSUnenoughOrder();
        void RECEIVEUMessage();

    private:
        static Server *g_pServer;
        string worldHostName;
        string worldPortNum;
        string upsHostName;
        string upsPortNum;
        string portNum;
        string amazon_listenport;
        int worldID;
        int n_warehouse;
        int n_trucks;
        int ups_fd;
        int world_fd;
        int ups_fd2;
        //int server_fd;

    public:
        void run();
    public:
        vector<bool> seqNumTable; // whether commands with specific seqNum is acked.
        size_t curSeqNum;
        mutex seqNum_lck;


        // record whether response from world with world's seqNum is executed.
        //true executed, false not.
        vector<bool> WorldseqNumTable; 
        // record whether response from UPS with UPS's seqNum is executed.
        //true executed, false not.
        //vector<bool> UPSseqNumTable; 
        

        Sbuf<ACommands> worldQueue;
        Sbuf<Order> orderQueue;
        Sbuf<AMessage> upsQueue;

        Sbuf<Client_Info> frontendClientQueue;
        Sbuf<Order> packOrderQueue;
        //Sbuf<Order> callTruckQueue;
        Sbuf<AResponses> receivedAResponseQueue;
        Sbuf<UMessage> receivedUMessageQueue;
        //Sbuf sbuf(SBUFSIZE);

        Log log;
};




#endif