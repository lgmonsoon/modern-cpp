#include <queue>
#include <chrono>
#include <mutex>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <semaphore>
#include <algorithm>

int main() {
    std::queue<int> producers;
    std::mutex mtx;
    std::condition_variable cv;
    bool notified = false;

    auto producer = [&](){
        for(int i = 0; ; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(90));
            std::unique_lock<std::mutex> lock(mtx);
            std::cout << "producing " << i << std::endl;
            producers.push(i);
            notified = true;
            cv.notify_all();
        }
    };

    auto consumer = [&](){
        while(true) {
            std::unique_lock<std::mutex> lock(mtx);
            while(!notified) {
                cv.wait(lock);
            }
            /* TODO */
            lock.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            lock.lock();
            while(!producers.empty()) {
                std::cout << "consuming " << producers.front() << std::endl;
                producers.pop();
            }
            notified = false;
        }
    };

    std::thread p{producer};
    std::vector<std::thread> vt;
    for(int i = 0; i < 10; ++i) {
        vt.emplace_back(std::thread{consumer});
    }
    std::cout << "vt.size() == " << vt.size() << std::endl;
    p.join();
    for_each(vt.begin(), vt.end(), [](auto& c){ c.join(); });

    return 0;
}