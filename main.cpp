#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <ctime>
#include <vector>
#include <unordered_map>
#include <condition_variable>
#include <chrono>



bool volatile busy = false;
const int N= 4;
const int ITERS = 50;
const int times10less = 10;
std::mutex io_mutex;
std::mutex timutex;

struct Time {
    clock_t start,stop;
    double time;

};



class ProducentKonsument {

    private:
        std::mutex _mutex;
        std::mutex mutex_add;
        std::mutex mutex_remove;
        std::condition_variable cv;
        int counter,in,out;
        std::vector<int> buffer;

        std::unordered_map<std::thread::id,Time> times;
        ProducentKonsument(int n) :counter(0), in(0),out(0), buffer(n) {times = std::unordered_map<std::thread::id,Time> ();};
    public:
    std::queue <std::thread> threads;

    void insert(int m){

        {
            std::unique_lock<std::mutex> lck(_mutex);
            cv.wait(lck, [&](){ return (counter != buffer.size());});
            stopTime();
            buffer[in] = m;
            in = (in+1)%buffer.size();
            ++counter;
            std::this_thread::sleep_for(std::chrono::milliseconds (11));
            cv.notify_one();
        }
    }
    int take ()
    {
        std::unique_lock<std::mutex> lck(_mutex);
        cv.wait(lck, [&](){ return (counter !=0);});
        stopTime();
        int i = buffer[out];
        out = (out+1)%buffer.size();
        --counter;
        cv.notify_one();
        return  i;
    }
    void startTime(){
        std::lock_guard<std::mutex> timelk (timutex);
        Time t;
        t.start = clock();
        std::pair<std::thread::id, Time> p = make_pair(std::this_thread::get_id(), t);
        times.insert(p);
    }
    void stopTime (){
        std::lock_guard<std::mutex> timelk (timutex);
        times[std::this_thread::get_id()].stop = clock();
        times[std::this_thread::get_id()].time = static_cast<double>(times[std::this_thread::get_id()].stop - times[std::this_thread::get_id()].start)/CLOCKS_PER_SEC;

    }
    static ProducentKonsument& get_buffer()
    {
        static ProducentKonsument buf(N);
        return buf;
    }

    static void produce()
    {
        for (int n = 0; n < ITERS; ++n)
        {
            {
                std::lock_guard<std::mutex> lock(io_mutex);
                get_buffer().startTime();
                std::cout << "poduced: " << n << std::endl;
            }
            get_buffer().insert(n);
        }
    }

    static void consumtion()
    {
        for (int x=0; x < (ITERS)/times10less; ++x)
        {
            int n = get_buffer().take();
            {
                std::lock_guard<std::mutex> lock(io_mutex);
                get_buffer().startTime();
                std::cout << "consumed: " << n << std::endl;
            }
        }
    }
    static void add(std::thread&& thread){
       // get_buffer().mutex_add.lock();
        get_buffer().threads.push(std::move(thread));
       // get_buffer().mutex_add.unlock();
    }
    static bool isEmpty(){
        return get_buffer().threads.empty();
    }

    static void remove() {
        //get_buffer().mutex_remove.lock();
        std::thread thread = std::move( get_buffer().threads.front());
        get_buffer().threads.pop();
        if (thread.joinable()) thread.join();
        //get_buffer().mutex_remove.unlock();
    }
    static void getMap() {
        for (auto const& pair: get_buffer().times) {
            std::cout <<  pair.first << ": " << pair.second.time << "\n";
        }

    }
};




int main(int argc, const char * argv[]) {
//    srand(time(NULL));
//    randAndSave();

    int numberOfThreads = 2*times10less;
    std::cout<<numberOfThreads<<' '<< numberOfThreads/times10less<<"\n";
    std::vector<int> data;
    int j = 0, ccounter =0,pcounter = 0;
//    for (auto i = 0 ; i < (numberOfThreads+(numberOfThreads/times10less)); i++) {
//
//
//        if(j==0 && ccounter< numberOfThreads){
//            j++;
//            ProducentKonsument::add(std::thread(&ProducentKonsument::consumtion));
//            std::cout<<i<<"con\n";
//            ccounter++;
//        }
//        else if(j>=1 && pcounter < numberOfThreads/times10less){
//            j++;
//            ProducentKonsument::add(std::thread(&ProducentKonsument::produce));
//            std::cout<<i<<"prod\n";
//            pcounter++;
//            if(j == 9){j=0;}
//
//        }
//        else  {
//            std::cout <<i<<'\n';
//        }
//    }

    for (int i = 0; i < numberOfThreads; i++) {
        ProducentKonsument::get_buffer().threads.push(std::thread(&ProducentKonsument::consumtion));
    }
    for (int i = 0; i < numberOfThreads/times10less; i++) {
        ProducentKonsument::get_buffer().threads.push(std::thread(&ProducentKonsument::produce));
    }
    while (!ProducentKonsument::isEmpty()){
        ProducentKonsument::remove();
    }

    ProducentKonsument::getMap();

}