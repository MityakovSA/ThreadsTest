#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>


std::mutex lockprint;
std::mutex lockqueue;
std::queue<int> values;


void _print(int n)
{
    {
        std::unique_lock<std::mutex> locker(lockprint);
        std::cout << "THREAD_ID: " << std::this_thread::get_id() << std::endl;
    }
    {
        std::unique_lock<std::mutex> locker(lockqueue);
        while (!values.empty())
        {
            std::unique_lock<std::mutex> locker(lockprint);
            std::cout << std::this_thread::get_id() << " : " << values.front() << std::endl;
            values.pop();
        }
    }
}


int main()
{
    for (int i = 0; i < 5; ++i)
    {
        values.push(i+1);
    }
    std::cout << "Threads number: " << std::endl;
    int _n;
    std::cin >> _n;
    std::vector<std::thread> threads;
    for(int i = 0; i < _n; ++i)
        threads.push_back(std::thread(_print, _n));
    for(auto &t: threads)
        t.join();
    return 0;
}