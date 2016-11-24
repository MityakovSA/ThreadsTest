#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cmath>


std::mutex lockprint;
std::mutex lockqueue;
std::vector<bool> notified;
std::queue<int> values;
std::condition_variable check;
bool done = false;


void _print(int ind, int n)
{
    {
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        std::unique_lock<std::mutex> locker(lockprint);
        std::cout << ind << ") THREAD_ID: " << std::this_thread::get_id() << std::endl;
    }

    while (!done)
    {
        std::unique_lock<std::mutex> locker(lockqueue);
        while (!notified.at(ind))
        {
            check.wait(locker);
        }
        if (done) return;
        while ((!values.empty()) && (notified.at(ind)))
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::unique_lock<std::mutex> locker(lockprint);
            std::cout << std::this_thread::get_id() << " : " << values.front() << std::endl;
            values.pop();
            if (values.empty())
               {
                   done = true;
                   for (int i = 0; i < n; ++i) notified.at(i) = true;
                   check.notify_all();
                   return;
               }
            notified.at(ind) = false;
            if (ind == (n-1)) notified.at(0) = true;
            else notified.at(ind+1) = true;
            check.notify_one();
        }
    }

    return;
}


int main()
{
    for (int i = 0; i < 5; ++i)
    {
        values.push(i+1);
    }
    int _n = 0;
    while (!_n)
    {
        std::cout << "Threads number: ";
        std::cin >> _n;
        if ((_n < 1) || (_n > std::thread::hardware_concurrency()))
        {
            std::cout << "Wrong threads number!" << std::endl;
            _n = 0;
        }
    }
    notified.push_back(true);
    for(int i = 0; i < (_n-1); ++i)
    {
        notified.push_back(false);
    }
    std::vector<std::thread> threads;
    for(int i = 0; i < _n; ++i)
    {
        threads.push_back(std::thread(_print, i, _n));
    }
    for(auto& t: threads)
        t.join();
    return 0;
}