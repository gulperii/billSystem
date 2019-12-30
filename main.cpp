#include <iostream>
#include <fstream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
using namespace std;
int electricity, water, gas, telecommunications, cableTv = 0;
string input_log;
int NUM_CUSTOMERS;
int NUM_ATMS = 10;

struct customer_thread_data {
    int  customer_id;
    int ATM_id;
    int bill_type;
    int payment;
    int sleep_time;

};


void *pay(void *threadarg) {
    struct customer_thread_data *my_data;
    my_data = (struct customer_thread_data *) threadarg;

    cout << "Sleep time : " << my_data->sleep_time ;
    sleep(my_data->sleep_time);

    if(sleep){

    }
    pthread_exit(NULL);
}

void create_atms(pthread_t *at){
    int rc;
    for(int i = 0; i< NUM_ATMS;i++){
        rc = pthread_create(&at[i],NULL,NULL);
    }
    if(rc){
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    }
}
void create_customers(ifstream &file, pthread_t *ct, customer_thread_data *ctd){
    int rc;
    for (int i =0; i < NUM_CUSTOMERS; i++){
        //Customer id 1 den başlıyor
        ctd[i].customer_id = i +1;
        file >> ctd[i].sleep_time;
        // ATM id bir eksili
        file >> ctd[i].ATM_id;
        file >> ctd[i].bill_type;
        file >> ctd[i].payment;
        rc = pthread_create(&ct[i],NULL,pay,(void *)&ctd[i]);


        if(rc){
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }

}
int main(int argc, char *argv[]) {
    input_log = argv[1];
    ifstream file(input_log);
    file >> NUM_CUSTOMERS;
    pthread_t customer_t[NUM_CUSTOMERS];
    pthread_t atm_t[NUM_ATMS];
    struct customer_thread_data ctd[NUM_CUSTOMERS];
    create_customers(file,customer_t,ctd);
    return 0;
}

