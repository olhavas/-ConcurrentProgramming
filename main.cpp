#include <iostream>
#include <mutex>
#include <pthread.h>
#include <thread>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>


using namespace std;


void rand_and_save()
{
    ofstream myfile;
    myfile.open ("../inputdata.txt");
    for (int i = 0; i <10000000; i++)
    {
        myfile << static_cast<int>(rand() % 256) << " ";
    }
    myfile.close();
}
void read_from_file()


int main() {

    srand (time(NULL));
    //generate file with input data
    //rand_and_save();
    uint num_of_threads = 1;
    cout<< "Change quantity of thread (2, 4, 8, 10, 12, 14, 16): ";
    cin>>num_of_threads;
    vector 


    if(num_of_threads == 0 || num_of_threads ==1)
    {

    }










    return 0;
}