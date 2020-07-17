#ifndef Threadpool_h
#define Threadpool_h

#include "Common.h"

template <typename T>
class threadsafe_queue {
private:
    mutable mutex mut;
    queue<T> data_queue;
    condition_variable data_cond;
public:
    threadsafe_queue() {}
    
    void push(T new_value) {
        lock_guard<mutex> lk(mut);
        data_queue.push(move(new_value));
        data_cond.notify_one();
    }
    
    void wait_and_pop(T& value) {
        unique_lock<mutex> lck(mut);
        data_cond.wait(lck, [this] { return !data_queue.empty(); });
        value = move(data_queue.front());
        data_queue.pop();
    }
    
    shared_ptr<T> wait_and_pop() {
        unique_lock<mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        shared_ptr<T> res(
                          make_shared<T>(move(data_queue.front())));
        data_queue.pop();
        return res;
    }
    
    bool try_pop(T& value) {
        lock_guard<mutex> lk(mut);
        if (data_queue.empty()) return false;
        value = move(data_queue.front());
        data_queue.pop();
        return true;
    }
    
    shared_ptr<T> try_pop() {
        lock_guard<mutex> lk(mut);
        if (data_queue.empty()) return shared_ptr<T>();
        shared_ptr<T> res(
                          make_shared<T>(move(data_queue.front())));
        data_queue.pop();
        return res;
    }
    
    bool empty() const {
        lock_guard<mutex> lk(mut);
        return data_queue.empty();
    }
};

class join_threads {
    vector<thread>& threads;
public:
    explicit join_threads(vector<thread>& threads_) :
    threads(threads_) {}
    
    ~join_threads() {
        for (unsigned long i = 0; i < threads.size(); ++i) {
            if
                (threads[i].joinable()) threads[i].join();
        }
    }
};

class function_wrapper {
    struct impl_base {
        virtual void call() = 0;
        virtual ~impl_base() {}
    };
    
    unique_ptr<impl_base> impl;
    
    template <typename F>
    struct impl_type : impl_base {
        F f;
        impl_type(F&& f_) : f(move(f_)) {}
        void call() override { f(); }
    };
    
public:
    template <typename F>
    function_wrapper(F&& f) :
    impl(new impl_type<F>(move(f))) {
        int i = 32;
        i++;
    }
    
    void operator()() {
        impl->call();
    }
    function_wrapper() = default;
    
    function_wrapper(function_wrapper&& other) :
    impl(move(other.impl)) {
        int i = 32;
        i++;
    }
    
    function_wrapper& operator=(function_wrapper&& other) {
        impl = move(other.impl);
        return *this;
    }
    
    function_wrapper(const function_wrapper&) = delete;
    function_wrapper(function_wrapper&) = delete;
    function_wrapper& operator=(const function_wrapper&) = delete;
};

class thread_pool {
    atomic_bool done;
    vector<thread> threads;
    join_threads joiner;
    threadsafe_queue<function_wrapper> work_queue;
    
    void worker_thread() {
        while (!done) {
            function_wrapper task;
            if (work_queue.try_pop(task)) { task(); }
            else { this_thread::yield(); }
        }
    }
    
public:
    template <typename FunctionType>
    future<typename invoke_result<FunctionType>::type>
    submit(FunctionType f) {
        using result_type = typename invoke_result<FunctionType>::type;
        packaged_task<result_type()> task(move(f));
        future<result_type> res(task.get_future());
        work_queue.push(move(task));
        return res;
    }
    
    thread_pool(unsigned int numthreads = thread::hardware_concurrency()) :
    done(false), joiner(threads) {
        unsigned int thread_count = numthreads;
        try {
            for (unsigned i = 0; i < thread_count; ++i) {
                threads.push_back(
                                  thread(&thread_pool::worker_thread, this));
            }
        }
        catch (...) {
            done = true;
            throw;
        }
    }
    
    ~thread_pool() { done = true; }
    
    bool Qmt() { return work_queue.empty(); }
};

#endif
