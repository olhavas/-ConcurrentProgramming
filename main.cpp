#include <iostream>
#include <mutex>
#include <pthread.h>
#include <thread>
#include <string>

using namespace std;
std::mutex m_mutex;

void shared_print(std::string str)
{

    std::lock_guard<std::mutex> locker(m_mutex);
    std::thread::id this_id = std::this_thread::get_id();
    cout<<this_id<<' '<<str<<'\n';
}

void fibonacci(const int num)
{
    std::string fib = "";
    if(num == 0) {
        fib.append(to_string(num));
        shared_print(fib);
        fib.append(" ");
    }
    else if(num >= 2)
    {
        int a = 0, b = 1;
        fib.append(std::to_string(a)+ " ");
        fib.append(std::to_string(b)+ " ");
        for(int i=0; i < (num-1); i++)
        {
            std::swap(a, b);
            b += a;
            fib.append(std::to_string(b)+ " ");
            shared_print(fib);
        }
    }

}
void fib_sum(const int num)
{
    std::string fib = "";

    if(num == 0) {
        fib.append(0);
        shared_print(fib);
        fib.append(" ");
    }
    else if(num>= 2)
    {
        int a = 0, b = 1, sum = 0;
        for(int i=0; i < (num-1); i++)
        {
            std::swap(a, b);
            b += a;
            sum+= b;
            fib.append(std::to_string(sum)+ " ");
            shared_print(fib);

        }
    }
}

int main() {
    int n = 10;
    std::thread first(fibonacci, n);
    std::thread second(fib_sum, n);
    cout<< "main \n";
    if(first.joinable())
    first.join();
    if(second.joinable())
    second.join();



    return 0;
}