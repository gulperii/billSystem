#include <iostream>
#include <fstream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <map>

using namespace std;

struct customer_thread_data {
    int customer_id;
    int ATM_id;
    string bill_type;
    int bill_id;
    int payment;
    int sleep_time;
    pthread_mutex_t *ATM_mutex;

};
int electricity, water, gas, telecommunications, cableTv = 0;
ofstream outfile;
string input_log;
int NUM_CUSTOMERS;
int NUM_ATMS = 10;
map<string, int> bill_map = {{"electricity",        0},
                             {"water",              1},
                             {"gas",                2},
                             {"telecommunications", 3},
                             {"cableTv",            4}};
map<int, string> rev_bill_map = {{0, "electricity"},
                                 {1, "water"},
                                 {2, "gas"},
                                 {3, "telecommunications"},
                                 {4, "cableTv"}};

vector<vector<int>> request = {{-1, -1, -1},
                               {-1, -1, -1},
                               {-1, -1, -1},
                               {-1, -1, -1},
                               {-1, -1, -1},
                               {-1, -1, -1},
                               {-1, -1, -1},
                               {-1, -1, -1},
                               {-1, -1, -1},
                               {-1, -1, -1}};
//int request[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
//vector<struct customer_thread_data> ctd={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

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

pthread_mutex_t m_write = PTHREAD_MUTEX_INITIALIZER;


pthread_mutex_t m_electricity = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_water = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_gas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_telecommunications = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_cableTv = PTHREAD_MUTEX_INITIALIZER;


struct atm_thread_data {
    int atm_Id;
};

int pay(int customer_id, int bill_id, int amount) {
    switch (bill_id) {
        case 0:
            pthread_mutex_lock(&m_electricity);
            electricity += amount;
            pthread_mutex_unlock(&m_electricity);
        case 2:
            pthread_mutex_lock(&m_gas);
            gas += amount;
            pthread_mutex_unlock(&m_gas);
        case 1:
            pthread_mutex_lock(&m_water);
            water += amount;
            pthread_mutex_unlock(&m_water);
        case 3:
            pthread_mutex_lock(&m_telecommunications);
            telecommunications += amount;
            pthread_mutex_unlock(&m_telecommunications);
        case 4:
            pthread_mutex_lock(&m_cableTv);
            cableTv += amount;
            pthread_mutex_unlock(&m_cableTv);
    }
    return 1;
}


void *runner(void *threadarg) {
    struct atm_thread_data *my_data;
    my_data = (struct atm_thread_data *) threadarg;
    while (request[my_data->atm_Id][0] == -1) {}
    if (pay(request[my_data->atm_Id][0], request[my_data->atm_Id][1], request[my_data->atm_Id][2])) {
        request[my_data->atm_Id] = {-1, -1, -1};
        pthread_mutex_lock(&m_write);
        string line =
                "Customer" + to_string(request[my_data->atm_Id][0]) + "," + to_string(request[my_data->atm_Id][2]) +
                "TL," + rev_bill_map[request[my_data->atm_Id][1]] + "\n";
        outfile << line;
        pthread_mutex_unlock(&m_write);
    }
}


void *request_payment(void *threadarg) {
    struct customer_thread_data *my_data;
    my_data = (struct customer_thread_data *) threadarg;

    cout << "Sleep time : " << my_data->sleep_time;
    sleep(my_data->sleep_time);

    if (sleep) {
        pthread_mutex_lock(my_data->ATM_mutex);
        if (my_data->bill_type == "electricity") {
            int bill = 0;
        }
        request[my_data->ATM_id] = {my_data->customer_id, my_data->bill_id, my_data->payment};
        while (request[my_data->ATM_id][0] != -1) {}
        pthread_mutex_unlock(my_data->ATM_mutex);
    }

    pthread_exit(NULL);
}

void create_atms(pthread_t *at, atm_thread_data *atd) {
    int rc;
    for (int i = 0; i < NUM_ATMS; i++) {
        atd[i].atm_Id = i + 1;
        rc = pthread_create(&at[i], NULL, runner, (void *) &atd[i]);
    }
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    }
}

void create_customers(ifstream &file, pthread_t *ct, customer_thread_data *ctd) {
    int rc;

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        //Customer id 1 den başlıyor
        ctd[i].customer_id = i + 1;
        file >> ctd[i].sleep_time;
        // ATM id bir eksili
        file >> ctd[i].ATM_id;
        file >> ctd[i].bill_type;
        ctd[i].bill_id = bill_map[ctd[i].bill_type];
        file >> ctd[i].payment;
        switch (ctd[i].ATM_id) {
            case 1:
                ctd[i].ATM_mutex = &atm1;
            case 2:
                ctd[i].ATM_mutex = &atm2;

            case 3:
                ctd[i].ATM_mutex = &atm3;
            case 4:
                ctd[i].ATM_mutex = &atm4;

            case 5:
                ctd[i].ATM_mutex = &atm5;

            case 6:
                ctd[i].ATM_mutex = &atm6;
            case 7:
                ctd[i].ATM_mutex = &atm7;
            case 8:
                ctd[i].ATM_mutex = &atm8;
            case 9:
                ctd[i].ATM_mutex = &atm9;
            case 10:
                ctd[i].ATM_mutex = &atm10;

                rc = pthread_create(&ct[i], NULL, request_payment, (void *) &ctd[i]);


                if (rc) {
                    cout << "Error:unable to create thread," << rc << endl;
                    exit(-1);
                }
        }

    }
}

int main(int argc, char *argv[]) {
    input_log = argv[1];
    string output_log = input_log + "_log.txt";
    ifstream file(input_log);
    outfile.open(output_log);
    file >> NUM_CUSTOMERS;
    pthread_t customer_t[NUM_CUSTOMERS];
    pthread_t atm_t[NUM_ATMS];
    customer_thread_data ctd[NUM_CUSTOMERS];
    atm_thread_data atd[NUM_ATMS];
    create_customers(file, customer_t, ctd);
    create_atms(atm_t, atd);
    for (int i = 0; i < NUM_CUSTOMERS; i++)
        pthread_join(customer_t[i], NULL);
    outfile<<"All payments are completed \n";
    outfile<<"CableTV: "+ to_string(cableTv)+"\n";
    outfile<<"Electricity: "+ to_string(electricity)+"\n";
    outfile<<"Gas: "+ to_string(gas)+"\n";
    outfile<<"Telecommunication: "+ to_string(telecommunications)+"\n";
    outfile<<"Water: "+ to_string(water)+"\n";
    return 0;
}

