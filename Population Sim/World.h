#ifndef World_h
#define World_h
#include "Common.h"
#include "Immigrant_Generator.h"

class World {
    friend class Excel_Helper;
private:
    ofstream outFile;
    string outFileName;
    vector <string> data;
    vector <string> imiplace;
    vector <string> movplace;
    Immigrant_Generator generator;
    vector <City> cities;
    vector <pair<double, double>> cityCenters;
    //inputs
    double a;
    double b;
    double c0;
    double m;
    unsigned int date;
    unsigned int totalCities;//after moving
    unsigned int totalPopulation;
    double averageFitness;
    void writeFile();
    void addData();
    void advanceAge();
    void advanceImmigrantMechanic();
    void advanceFitness();
    unsigned int calculateFitness(unsigned int);
    void advanceMovingMechanic();
    unsigned int calculateLifeExpectancy(unsigned int);
    double calculateInfluence(unsigned int, pair<double, double>point);
public:
    int ac,caod,totalkids;
    double aod;
    double r[2]={0,0};
    World(double,double, double, double, unsigned int, unsigned int, double, string);//alpha, beta, birthrate, constant of influence rate of immigration dimensions moving mechanic
    void initializeSimulation();//TODO
    void advanceSimulation(unsigned int);
    void printData();//TODO
    void writeData();//TODO
};

template <typename T>
class threadsafe_queue {
private:
    mutable std::mutex mut;
    queue<T> data_queue;
    std::condition_variable data_cond;
public:
    threadsafe_queue() {}

    void push(T new_value) {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(std::move(new_value));
        data_cond.notify_one();
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lck(mut);
        data_cond.wait(lck, [this] { return !data_queue.empty(); });
        value = std::move(data_queue.front());
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        std::shared_ptr<T> res(
            std::make_shared<T>(std::move(data_queue.front())));
        data_queue.pop();
        return res;
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) return false;
        value = std::move(data_queue.front());
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) return std::shared_ptr<T>();
        std::shared_ptr<T> res(
            std::make_shared<T>(std::move(data_queue.front())));
        data_queue.pop();
        return res;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};

class join_threads {
    std::vector<std::thread>& threads;
public:
    explicit join_threads(std::vector<std::thread>& threads_) :
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

    std::unique_ptr<impl_base> impl;

    template <typename F>
    struct impl_type : impl_base {
        F f;
        impl_type(F&& f_) : f(std::move(f_)) {}
        void call() override { f(); }
    };

public:
    template <typename F>
    function_wrapper(F&& f) :
        impl(new impl_type<F>(std::move(f))) {
        int i = 32;
        i++;
    }

    void operator()() {
        impl->call();
    }
    function_wrapper() = default;

    function_wrapper(function_wrapper&& other) :
        impl(std::move(other.impl)) {
        int i = 32;
        i++;
    }

    function_wrapper& operator=(function_wrapper&& other) {
        impl = std::move(other.impl);
        return *this;
    }

    function_wrapper(const function_wrapper&) = delete;
    function_wrapper(function_wrapper&) = delete;
    function_wrapper& operator=(const function_wrapper&) = delete;
};

class thread_pool {
    std::atomic_bool done;
    std::vector<std::thread> threads;
    join_threads joiner;
    threadsafe_queue<function_wrapper> work_queue;

    void worker_thread() {
        while (!done) {
            function_wrapper task;
            if (work_queue.try_pop(task)) { task(); }
            else { std::this_thread::yield(); }
        }
    }

public:
    template <typename FunctionType>
    std::future<typename std::invoke_result<FunctionType>::type>
        submit(FunctionType f) {
        using result_type = typename std::invoke_result<FunctionType>::type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }

    thread_pool(unsigned int numthreads = std::thread::hardware_concurrency()) :
        done(false), joiner(threads) {
        unsigned int thread_count = numthreads;
        try {
            for (unsigned i = 0; i < thread_count; ++i) {
                threads.push_back(
                    std::thread(&thread_pool::worker_thread, this));
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

#endif /* World_h */
