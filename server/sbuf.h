#ifndef _SBUF_H
#define _SBUF_H
#include <vector>

#include "function.h"
using namespace std;

template<typename T>
class Sbuf{
    public:
        vector<T>buf; /* Buffer array */ 
        int n;                  /* Maximum number of slots */
        int front;              /* buf[(front+1)%n] is first item */
        int rear;               /* buf[rear%n] is last item */
        sem_t mutex;            /* Protects accesses to buf */
        sem_t slots;            /* Counts available slots */
        sem_t items;            /* Counts available items */

        Sbuf(int num);
        void subf_insert(T info);
        T subf_remove();
};

template<typename T>
Sbuf<T>::Sbuf(int num){
    this->n = num;
    this->buf = vector<T>(num);
    this->front = 0;
    this->rear     = 0;
    sem_init(&this->mutex, 0, 1);
    sem_init(&this->slots, 0, num);
    sem_init(&this->items, 0, 0);
}

template<typename T>
void Sbuf<T>::subf_insert(T info){
    P(&this->slots);
    P(&this->mutex);

    this->buf[(++this->rear)%(this->n)] = info;

    V(&this->mutex);                          
    V(&this->items);
}

template<typename T>
T Sbuf<T>::subf_remove(){
    T info;

    P(&this->items);                          
    P(&this->mutex);                          
    info = this->buf[(++this->front)%(this->n)];  
    V(&this->mutex);                          
    V(&this->slots);

    return info;
}

#endif