#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

template<typename T>
class Monitor {
public:
    Monitor() = default;

    // Add an item to the monitor
    void put(const T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(item);
        cond_var_.notify_one(); // Notify one waiting thread
    }

    // Remove an item from the monitor
    T get() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [this] { return !queue_.empty(); }); // Wait until the queue is not empty
        T item = queue_.front();
        queue_.pop();
        return item;
    }

    // Check if the monitor is empty
    bool isEmpty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::mutex mutex_;
    std::condition_variable cond_var_;
    std::queue<T> queue_;
};

class DiningPhilosophers {
private:
    std::mutex mtx;
    std::condition_variable cv[5];
    enum State { THINKING, HUNGRY, EATING };
    State state[5];
    
public:
    DiningPhilosophers() {
        for (int i = 0; i < 5; i++) {
            state[i] = THINKING;
        }
    }
    
    void test(int i) {
        if (state[i] == HUNGRY && 
            state[(i + 4) % 5] != EATING && 
            state[(i + 1) % 5] != EATING) {
            state[i] = EATING;
            cv[i].notify_one();
        }
    }
    
    void pickup_chopsticks(int i) {
        std::unique_lock<std::mutex> lock(mtx);
        state[i] = HUNGRY;
        test(i);
        while (state[i] != EATING) {
            cv[i].wait(lock);
        }
    }
    
    void putdown_chopsticks(int i) {
        std::unique_lock<std::mutex> lock(mtx);
        state[i] = THINKING;
        test((i + 4) % 5);  // Test left neighbor
        test((i + 1) % 5);  // Test right neighbor
    }
    
    void philosopher(int i) {
        while (true) {
            std::cout << "Philosopher " << i << " is thinking\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            //std::cout << "Philosopher " << i << " is hungry\n";
            pickup_chopsticks(i);
            
            std::cout << "Philosopher " << i << " is eating\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            putdown_chopsticks(i);
        }
    }
};

int main() {
    DiningPhilosophers dp;
    std::thread philosophers[5];
    
    // Create 5 philosophers
    for (int i = 0; i < 5; i++) {
        philosophers[i] = std::thread(&DiningPhilosophers::philosopher, &dp, i);
    }
    
    // Join threads
    for (int i = 0; i < 5; i++) {
        philosophers[i].join();
    }
    
    return 0;
}
