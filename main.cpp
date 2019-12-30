#include <iostream>
#include <fstream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
using namespace std;
//ortak hesaplar
int electricity, water, gas, telecommunications, cableTv = 0;
string input_log;
int NUM_CUSTOMERS;
int NUM_ATMS = 10;
pthread_mutex_t balance_changer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t atm10 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t atm1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t atm2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t atm3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t atm4 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t atm5 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t atm6 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t atm7 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t atm8 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t atm9 = PTHREAD_MUTEX_INITIALIZER;

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
        switch(my_data->ATM_id){
            case 1:
                pthread_mutex_lock(&atm1);
            case 2:
                pthread_mutex_lock(&atm2);
            case 3:
                pthread_mutex_lock(&atm3);
            case 4:
                pthread_mutex_lock(&atm4);
            case 5:
                pthread_mutex_lock(&atm5);
            case 6:
                pthread_mutex_lock(&atm6);
            case 7:
                pthread_mutex_lock(&atm7);
            case 8:
                pthread_mutex_lock(&atm8);
            case 9:
                pthread_mutex_lock(&atm9);
            case 10:
                pthread_mutex_lock(&atm10);


        }



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

