#include <iostream>
#include <mutex>
#include <thread>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>


const int32_t DSIZE = 10000000;
struct Result {

    Result() { histogram.fill(0);}
    std::mutex _mutex;
    std::array<int,256> histogram;
    void incrementOnIndex(int index) {
        //std::lock_guard<std::mutex>lock(_mutex);
        histogram[index]++;
    }
    void initResult(){
        for( auto& i : histogram) {
            i = 0;
        }
    }
};

struct Time {
    double  time = 0;
    int thread = 0;
};

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


void writeToFile(const Result &result, std::string nameoffile)
{
    std::ofstream  out;
    out.open("../"+nameoffile);
    for(auto i =0; i < result.histogram.size(); i++)
    {
        out<<i<<'='<<result.histogram[i]<<'\n';
    }
    auto suma = 0;
    for(auto x : result.histogram) {
        suma+=x;
    }
    out<<"sum=" << suma;

    out.close();
}

void writeToFile(const std::vector<Time> &times, std::string nameoffile)
{
    std::ofstream  out;
    out.open("../"+nameoffile);
    for(auto i =0; i < times.size(); i++)
    {
        out<<times[i].thread<<" thread(s) "<<times[i].time<<".s\n";
    }
    out.close();
}


int main(int argc, const char * argv[]) {
//    srand(time(NULL));
//    randAndSave();

    Result result ;
    int numberOfThreads = 1;
    std::vector<int> data;
    readFromFile(data, "inputdata.txt");
    clock_t start,stop;
    std::vector<Time> times;
    while (numberOfThreads > 0) {

        int dataSize = data.size();
        numberOfThreads = 1;
        result.initResult();
        Time time;

        std::cout << "num of thread <2, 4, 8,10 , 12 , 14 , 16>";
        std::cin >> numberOfThreads;
        if (numberOfThreads < 0) {
            writeToFile(times, "asynctimes.txt");
            return 0;
        }
        if (numberOfThreads == 1) {
            start = clock();
            for (int j = 0; j < dataSize; j++) {
                result.incrementOnIndex(data[j]);
            }
            stop = clock();
            time.thread = numberOfThreads;
            time.time = static_cast<double>(stop - start)/CLOCKS_PER_SEC;
            times.push_back(time);
        } else {
            int blockSize = dataSize / numberOfThreads;
            int lastThreadSize = (dataSize - blockSize * numberOfThreads) + blockSize;

            std::vector<std::thread> threadVector;
            start = clock();

            for (int i = 0; i < numberOfThreads - 1; i++) {
                threadVector.push_back(std::thread([=, &result, &data ]() {
                    int first = i * blockSize;
                    int last = (i + 1) * blockSize - 1;
                    for (int j = first; j <= last; j++) {
                        result.incrementOnIndex(data[j]);
                    }
                }));
            }
            threadVector.push_back(std::thread([=, &result, &data]() {
                int first = (numberOfThreads - 1) * blockSize;
                int last = ((numberOfThreads - 1) * blockSize - 1) + lastThreadSize;
                for (int j = first; j <= last; j++) {
                    result.incrementOnIndex(data[j]);
                }
            }));

            for (auto &thread : threadVector) {
                if (thread.joinable()) thread.join();
            }
            stop = clock();
            time.thread = numberOfThreads;
            time.time = static_cast<double>(stop - start)/CLOCKS_PER_SEC;
            times.push_back(time);
        }
        writeToFile(result, "asyncout" + std::to_string(numberOfThreads) + ".txt");
  }

}