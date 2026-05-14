#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

class binarySemaphore {
    private:
        bool flag;
        std::mutex mtx;
        std::condition_variable cv;
    public:
        binarySemaphore(bool initial = true) : flag(initial) {}

        void acquire() {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this](){return flag == true;});
            flag = false; // consume
        }
        
        void release() {
            {
                std::lock_guard<std::mutex> lock(mtx);
                flag = true;
            }
            cv.notify_one();
        }
};

binarySemaphore bs;

int global_counter(0);

void increment() {
    bs.acquire();
    for (int i=1;i<=100000;i++) {
        global_counter++;
    }
    bs.release();
}

int main() {

    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout<<global_counter;

}