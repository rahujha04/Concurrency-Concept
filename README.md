# Concurrency Concepts in C++

Just a personal repo where I've been implementing some concurrency and algorithmic stuff from scratch to get a better feel for how they actually work under the hood. No external libraries, pure C++11.

---

## What's inside

- [Binary Semaphore](#binary-semaphore)
- [Fast Exponentiation](#fast-exponentiation)
  - [Binary / Radix-2](#binary-exponentiation)
  - [Octal / Radix-8](#octal-exponentiation)
- [Thread Pool](#thread-pool)
- [Building](#building)

---

## Binary Semaphore

`binarySemaphoreImplementation/1.cpp`

I wanted to understand how a semaphore works internally before just calling `std::counting_semaphore` from C++20. So I built one from scratch using a `mutex` and a `condition_variable`.

The idea is simple — there's a boolean `flag`. `acquire()` blocks as long as `flag` is false, and once it's true, flips it to false and enters the critical section. `release()` sets it back to true and wakes up any waiting thread.

```cpp
void acquire() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return flag == true; });
    flag = false;
}

void release() {
    { std::lock_guard<std::mutex> lock(mtx); flag = true; }
    cv.notify_one();
}
```

To test it, two threads each do 100,000 increments on a shared counter. Without any synchronization you'd get a race and the result would be garbage. With the semaphore it always prints 200,000.

---

## Fast Exponentiation

### Binary Exponentiation

`FastExponentiation/binaryExponentiation.cpp`

The classic "exponentiation by squaring" — instead of multiplying `base` by itself `n` times, you look at the binary representation of `n` and process one bit per iteration. Goes from $O(n)$ down to $O(\log_2 n)$ multiplications.

```cpp
ll binaryExp(ll a, ll n) {
    ll result = 1;
    while (n) {
        if (n & 1) result *= a;
        a *= a;
        n >>= 1;
    }
    return result;
}
```

Every iteration: if the current bit is set, fold the current power of `a` into `result`. Then square `a` and shift `n` right. That's it.

---

### Octal Exponentiation

`FastExponentiation/octalExponentiation.cpp`

This one's an extension of the binary idea — instead of reading 1 bit at a time, read 3 bits at a time (i.e., work in base 8). Each "digit" in the octal representation of `n` is between 0–7, and you precompute $b^2$ and $b^4$ to handle all 8 cases with a switch. Then advance `base` by $b^8$ and shift `n` right by 3.

The math behind it:

$$n = d_0 + d_1 \cdot 8 + d_2 \cdot 8^2 + \dots \quad (0 \le d_i \le 7)$$

$$\text{base}^n = \text{base}^{d_0} \cdot (\text{base}^8)^{d_1} \cdot (\text{base}^{64})^{d_2} \cdots$$

Compared to binary exponentiation, you do a bit more work per iteration but the loop runs ~3x fewer times — so for large exponents it's noticeably faster.

---

## Thread Pool

`threadPool/threadPoolImplementation.cpp`

Spawning a new thread for every task is expensive. A thread pool fixes that by keeping a fixed set of worker threads alive and feeding them tasks from a shared queue.

Workers sit in a loop waiting on a `condition_variable`. When a task is pushed to the queue, one worker wakes up, grabs the task, runs it, and goes back to waiting.

```
enqueue(task) --> [task queue] --> worker threads pick up and run
```

Shutdown is handled in the destructor — set `stop = true`, wake everyone up with `notify_all()`, and join. Workers check `stop && tasks.empty()` before returning, so nothing gets dropped mid-flight.

The demo spins up 4 workers and submits 20 tasks (each sleeps 100ms). Without pooling that'd take ~2 seconds sequentially; with 4 workers it finishes in ~500ms.

```cpp
ThreadPool pool(4);
for (uint32_t i = 1; i <= 20; i++) {
    pool.enqueue([i]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "thread " << std::this_thread::get_id()
                  << " running task " << i << std::endl;
    });
}
```

---

## Building

Needs GCC with C++11 and pthreads (standard on Linux/macOS, available via MinGW on Windows).

```bash
# Binary Semaphore
g++ -std=c++11 -pthread binarySemaphoreImplementation/1.cpp -o semaphore && ./semaphore

# Binary Exponentiation
g++ -std=c++11 FastExponentiation/binaryExponentiation.cpp -o binexp && ./binexp

# Octal Exponentiation
g++ -std=c++11 FastExponentiation/octalExponentiation.cpp -o octexp && ./octexp

# Thread Pool
g++ -std=c++11 -pthread threadPool/threadPoolImplementation.cpp -o threadpool && ./threadpool
```

On Windows just drop the `./` and run the `.exe` directly.


