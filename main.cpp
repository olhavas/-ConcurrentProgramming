#include <iostream>
#include <mutex>
#include <pthread.h>
#include <thread>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>


const int32_t DSIZE = 10000000;

void randAndSave()
{
    std::ofstream myfile;
    myfile.open ("../inputdata.txt");
    for (int i = 0; i < DSIZE; i++)
    {
        myfile << static_cast<int>(rand() % 256) << " ";
    }
    myfile.close();
}
void readFromFile(std::vector<int> &data, std::string nameoffile)
{
    std::ifstream in("../"+nameoffile, std::ios::in);

      if(in.is_open())
      {
          int x ;
        while(in >> x)
        {
            data.push_back( static_cast<int>(x));
        }
        in.close();
    }

}




struct Result {

    Result() { histogram.fill(0);}
    std::mutex _mutex;
    std::array<int,256> histogram;
    void incrementOnIndex(int index) {
        std::lock_guard<std::mutex>lock(_mutex);
        histogram[index]++;
    }
};

void writeToFile(Result* result, std::string nameoffile)
{
    std::ofstream  out;
    out.open("../"+nameoffile);
    for(auto i =0; i < result->histogram.size(); i++)
    {
        out<<i<<'='<<result->histogram[i]<<'\n';
    }
    auto suma = 0;
    for(auto x : result->histogram) {
        suma+=x;
    }
    out<<"sum=" << suma;

    out.close();
}

int main(int argc, const char * argv[]) {

  std::vector<int> data;
    readFromFile(data, "inputdata.txt");


    int dataSize = data.size();
    int numberOfThreads = 1;

   // while (numberOfThreads > 0) {
        Result *result = new Result();
        numberOfThreads = 1;

        std::cout << "num of thread <2, 4, 8,10 , 12 , 14 , 16>";
        std::cin >> numberOfThreads;
        if (numberOfThreads == 1) {
            for (int j = 0; j <= dataSize; j++) {
                result->incrementOnIndex(data[j]);
            }
        } else {


            int blockSize = dataSize / numberOfThreads;
            int lastThreadSize = (dataSize - blockSize * numberOfThreads) + blockSize;

            std::vector<std::thread> threadVector;

            for (int i = 0; i < numberOfThreads - 1; i++) {
                threadVector.push_back(std::thread([&]() {
                    int first = i * blockSize;
                    int last = (i + 1) * blockSize - 1;
                    for (int j = first; j <= last; j++) {
                        result->incrementOnIndex(data[j]);
                    }
                }));
            }
            threadVector.push_back(std::thread([&]() {
                int first = (numberOfThreads - 1) * blockSize;
                int last = ((numberOfThreads - 1) * blockSize - 1) + lastThreadSize;
                for (int j = first; j <= last; j++) {
                    result->incrementOnIndex(data[j]);
                }
            }));

            for (auto &thread : threadVector) {
                if (thread.joinable()) thread.join();
            }
        }

        writeToFile(result, "out" + std::to_string(numberOfThreads) + ".txt");
        delete result;
   // }

    return 0;
}