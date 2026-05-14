#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex mtx;
    std::condition_variable cv;

    bool stop;

public:
    ThreadPool(size_t numThreads) : stop(false) {

        for (size_t i = 0; i < numThreads; ++i) {

            workers.emplace_back([this, i]() {

                while (true) {

                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(mtx);

                        cv.wait(lock, [this]() {
                            return stop || !tasks.empty();
                        });

                        // shutdown condition
                        if (stop && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    std::cout << "task done by thread "
                              << i << std::endl;

                    task();
                }
            });
        }
    }

    void enqueue(std::function<void()> task) {

        {
            std::lock_guard<std::mutex> lock(mtx);

            tasks.push(std::move(task));
        }

        cv.notify_one();
    }

    ~ThreadPool() {

        {
            std::lock_guard<std::mutex> lock(mtx);
            stop = true;
        }

        cv.notify_all();

        for (auto &worker : workers) {
            worker.join();
        }
    }
};

std::mutex iomtx;

int main() {

    ThreadPool pool(4);

    for (uint32_t i(1); i<=20; i++) {
        pool.enqueue([i](){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            {
                std::lock_guard<std::mutex> lock(iomtx);
                std::cout<<"thread "<<std::this_thread::get_id()<<" is running "<<i<<" task"<<std::endl;
            }
        });
    }

    return 0;
}