#ifndef _LOG_H
#define _LOG_H
#include <mutex>
#include <ctime>
#include <fstream>
#include <iostream>

using namespace std;

class Log{
    //const string fileLocation = "/var/log/erss/server.log";
    //const string fileLocation = "/home/xw202/erss-project-hb179-xm202/server/server2.log";
    const string fileLocation = "/code/server.log";
    
    std::ofstream ofs;
    mutex lock;

    public:
        Log(){
            ofs.open(fileLocation, std::ofstream::out | std::ofstream::app);
            if(ofs.is_open()){
                cout << "logger file open\n";
            }else{
                throw iostream::failure("Unable to open the log file");
            }
        }

        void writeString(string &content){
            if(ofs.is_open() && lock.try_lock()){
                cout << "writing\n";
                ofs << content << endl;
                ofs.flush();
                lock.unlock();
            }
        }

        void closeFile(){
            ofs.close();
            
        }


};

#endif
