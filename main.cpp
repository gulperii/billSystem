#include <fstream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>

using namespace std;

//Data to pass to customer threads
struct customer_thread_data {
    int customer_id;
    int ATM_id;
    string bill_type;
    int bill_id;
    int payment;
    int sleep_time;
    //Which atm mutex to listen to
    pthread_mutex_t *ATM_mutex;

};

//Data to pass to ATM threads
struct atm_thread_data {
    int atm_Id;
};

// Balances for the bills
int electricity, water, gas, telecommunications, cableTv = 0;
int NUM_CUSTOMERS;
int NUM_ATMS = 10;

string outfile = "";

//Dictionaries to map the bill types and corresponding bill ids.
map<string, int> bill_map = {{"electricity",       0},
                             {"water",             1},
                             {"gas",               2},
                             {"telecommunication", 3},
                             {"cableTV",           4}};
map<int, string> rev_bill_map = {{0, "electricity"},
                                 {1, "water"},
                                 {2, "gas"},
                                 {3, "telecommunication"},
                                 {4, "cableTV"}};

//Every index represents an ATM. When a customer requests a payment, it passes the necessary information to the corresponding array index (ATM_Id -1)
//Inner array represnts = <customer_Id,bill_Id,payment_amount>
int request[10][3] = {{-1, -1, -1},
                      {-1, -1, -1},
                      {-1, -1, -1},
                      {-1, -1, -1},
                      {-1, -1, -1},
                      {-1, -1, -1},
                      {-1, -1, -1},
                      {-1, -1, -1},
                      {-1, -1, -1},
                      {-1, -1, -1}};

//Mutex locks for every ATM
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

//Mutex lock for writing to ouput log file
pthread_mutex_t m_write = PTHREAD_MUTEX_INITIALIZER;

//Mutex lock for every bill type
pthread_mutex_t m_electricity = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_water = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_gas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_telecommunications = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_cableTv = PTHREAD_MUTEX_INITIALIZER;


//Helper function to realize the payment
int pay(int customer_id, int bill_id, int amount) {
    //Since every bill_Id requiers a different mutex lock, we compare and find correct lock
    switch (bill_id) {
        case 0:
            pthread_mutex_lock(&m_electricity);
            electricity += amount;
            pthread_mutex_unlock(&m_electricity);
            break;

        case 1:
            pthread_mutex_lock(&m_water);
            water += amount;
            pthread_mutex_unlock(&m_water);
            break;
        case 2:
            pthread_mutex_lock(&m_gas);
            gas += amount;
            pthread_mutex_unlock(&m_gas);
            break;


        case 3:
            pthread_mutex_lock(&m_telecommunications);
            telecommunications += amount;
            pthread_mutex_unlock(&m_telecommunications);
            break;

        case 4:
            pthread_mutex_lock(&m_cableTv);
            cableTv += amount;
            pthread_mutex_unlock(&m_cableTv);
            break;

    }
    return 1;
}

//helper function to log the payments
void output(string line) {
    ofstream logfile;
    logfile.open(outfile, ios_base::app);
    logfile << line;
    logfile.close();
}

//Runner function for ATM threads
void *runner(void *threadarg) {
    struct atm_thread_data *my_data;
    my_data = (struct atm_thread_data *) threadarg;
    while (true) {
        //If index is still -1, no customer has requested a payment
        while (request[my_data->atm_Id - 1][2] == -1) {}
        pay(request[my_data->atm_Id - 1][0], request[my_data->atm_Id - 1][1], request[my_data->atm_Id - 1][2]);

        string line =
                "Customer" + to_string(request[my_data->atm_Id - 1][0]) + "," +
                to_string(request[my_data->atm_Id - 1][2]) +
                "TL," + rev_bill_map[request[my_data->atm_Id - 1][1]] + "\n";
        //Lock for writing to ouput file
        pthread_mutex_lock(&m_write);
        output(line);
        //Unlock
        pthread_mutex_unlock(&m_write);
        //Turn the array to initial state
        request[my_data->atm_Id - 1][0] = -1;
        request[my_data->atm_Id - 1][1] = -1;
        request[my_data->atm_Id - 1][2] = -1;
    }


}

//Runner function for customer threads
void *request_payment(void *threadarg) {
    struct customer_thread_data *my_data;
    my_data = (struct customer_thread_data *) threadarg;
    //Sleep for the given amount of time
    usleep(my_data->sleep_time);
    //Lock for modifiying the request array
    pthread_mutex_lock(my_data->ATM_mutex);
    //Fill the array with relevant information
    request[my_data->ATM_id - 1][0] = my_data->customer_id;
    request[my_data->ATM_id - 1][1] = my_data->bill_id;
    request[my_data->ATM_id - 1][2] = my_data->payment;
    //If ATM thread has not done yet wait
    while (request[my_data->ATM_id - 1][2] != -1) {}
    //Unlock after payment is done
    pthread_mutex_unlock(my_data->ATM_mutex);

    pthread_exit(0);
}

//Create 10 atm threads and the data to pass
bool create_atms(pthread_t *at, atm_thread_data *atd) {
    int rc;
    for (int i = 0; i < NUM_ATMS; i++) {
        atd[i].atm_Id = i + 1;
        rc = pthread_create(&at[i], NULL, runner, (void *) &atd[i]);
    }
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    }
    return true;
}

//Create customer threads and data to pass to
void create_customers(ifstream &file, pthread_t *ct, customer_thread_data *ctd) {
    int rc;
    string dummy;
    int i = 0;
    while (getline(file, dummy)) {
        //Read the file line by line and seperate the line using comma as delimeter
        stringstream ss(dummy);
        string sleep, atm_id, bill_t, amount = "";
        getline(ss, sleep, ',');
        getline(ss, atm_id, ',');
        getline(ss, bill_t, ',');
        getline(ss, amount, ',');

        ctd[i].customer_id = i + 1;
        //Multiply by 1000 to convert to microseconds
        ctd[i].sleep_time = stoi(sleep) * 1000;
        //Convert strings to integers
        ctd[i].ATM_id = stoi(atm_id);
        ctd[i].bill_type = bill_t;
        ctd[i].bill_id = bill_map[ctd[i].bill_type];
        ctd[i].payment = stoi(amount);
        //Assign the correct lock according to the ATM of the customer
        switch (ctd[i].ATM_id) {
            case 1:
                ctd[i].ATM_mutex = &atm1;
                break;
            case 2:
                ctd[i].ATM_mutex = &atm2;
                break;

            case 3:
                ctd[i].ATM_mutex = &atm3;
                break;
            case 4:
                ctd[i].ATM_mutex = &atm4;
                break;

            case 5:
                ctd[i].ATM_mutex = &atm5;
                break;

            case 6:
                ctd[i].ATM_mutex = &atm6;
                break;
            case 7:
                ctd[i].ATM_mutex = &atm7;
                break;

            case 8:
                ctd[i].ATM_mutex = &atm8;
                break;

            case 9:
                ctd[i].ATM_mutex = &atm9;
                break;

            case 10:
                ctd[i].ATM_mutex = &atm10;
                break;

        }
        rc = pthread_create(&ct[i], NULL, request_payment, (void *) &ctd[i]);


        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);

        }
        i++;
    }
}

int main(int argc, char *argv[]) {
    string input_log = argv[1];

    //Find the correct output filename
    int index_of_dot = input_log.find_last_of(".");
    string filename = input_log.substr(0, index_of_dot);
    outfile = filename + "_log.txt";

    //read the first line and find the number of customers
    ifstream file;
    string line;
    file.open(input_log);
    getline(file, line);
    NUM_CUSTOMERS = stoi(line);

    //Stores customer threads
    pthread_t customer_t[NUM_CUSTOMERS];
    //Stores atm threads
    pthread_t atm_t[NUM_ATMS];

    //Stores atm threads' data
    atm_thread_data atd[NUM_ATMS];
    //Stores customer threads' data
    customer_thread_data ctd[NUM_CUSTOMERS];

    create_atms(atm_t, atd);
    create_customers(file, customer_t, ctd);

    //Wait for all customer threads to finish
    for (int i = 0; i < NUM_CUSTOMERS; i++)
        pthread_join(customer_t[i], NULL);

    //After all customer threads have finished, log the completion of the payments
    ofstream logfile;
    logfile.open(outfile, ios_base::app);
    logfile << "All payments are completed \n";
    logfile << "CableTV: " + to_string(cableTv) + "\n";
    logfile << "Electricity: " + to_string(electricity) + "\n";
    logfile << "Gas: " + to_string(gas) + "\n";
    logfile << "Telecommunication: " + to_string(telecommunications) + "\n";
    logfile << "Water: " + to_string(water) + "\n";
    return 0;
}