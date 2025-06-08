//* ThreadPool.h
//* https://stackoverflow.com/a/32593825/21874203

#ifndef IG20240526193442
#define IG20240526193442

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

//* Usage: thread_pool->queueJob([/* =, & */] { /* ... */ })
class ThreadPool
{
    //* Tells threads to stop looking for jobs
    bool should_terminate_ = false;

    //* Prevents data races to the job queue
    std::mutex queue_mutex_;

    //* Allows threads to wait on new jobs or termination
    std::condition_variable mutex_condition_;
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> poolJobs_;

    //* Allows to check for active jobs; Needed as threads are always active (waiting for jobs)
    std::mutex active_job_mutex_;
    int active_jobs_{ 0 };

public:
    ThreadPool();
    ~ThreadPool();

    //* Add a new job to the pool; use a lock so that there isn't a data race.
    void queueJob( std::function<void()> const& job );

    //* ThreadPool::Stop will _not_ terminate any currently running jobs, it just waits for them to finish via active_thread.join().
    void stop();

    //* Returns if there are queued jobs left
    //* The isJobQueued() function can be used in a while loop, such that the main thread can wait the threadpool to complete all the tasks before calling the threadpool destructor.
    bool isJobQueued();

    //* Waits for all jobs to finish without stopping threadpool
    void joinJobs() const;

    //* Return number of threads available
    size_t threadCount() const;

private:
    //* Each thread should be running its own infinite loop, constantly waiting for new tasks to grab and run.
    void start();

    //* The infinite loop function. This is a while (true) loop waiting for the task queue to open up.
    void threadLoop();
};

//* ThreadPool.cpp
//* The anonymous code blocks are used so that when they are exited, the std::unique_lock variables created within them go out of scope, unlocking the mutex.

inline ThreadPool::ThreadPool()
{
    start();
}

inline void ThreadPool::start()
{
    //* Max # of threads the system supports
    uint32_t const num_threads = std::thread::hardware_concurrency();

    threads_.reserve( num_threads );

    for ( uint32_t i = 0; i < num_threads; ++i )
    {
        threads_.emplace_back( std::thread( &ThreadPool::threadLoop, this ) );
    }
}

inline void ThreadPool::threadLoop()
{
    while ( true )
    {
        std::function<void()> threadJob;

        {
            std::unique_lock<std::mutex> lock( queue_mutex_ );

            mutex_condition_.wait(
                lock,
                [this]
                {
                    return !poolJobs_.empty() || should_terminate_;
                }
            );

            if ( should_terminate_ )
            {
                return;
            }

            threadJob = poolJobs_.front();

            poolJobs_.pop();
        }

        {
            std::unique_lock<std::mutex> lock( active_job_mutex_ );
            ++active_jobs_;
        }

        threadJob();

        {
            std::unique_lock<std::mutex> lock( active_job_mutex_ );
            --active_jobs_;
        }
    }
}

inline void ThreadPool::queueJob( std::function<void()> const& job )
{
    {
        std::unique_lock<std::mutex> lock( queue_mutex_ );
        poolJobs_.push( job );
    }

    mutex_condition_.notify_one();
}

inline bool ThreadPool::isJobQueued()
{
    bool isPoolBusy;

    {
        std::unique_lock<std::mutex> lock( queue_mutex_ );
        isPoolBusy = !poolJobs_.empty();
    }

    return isPoolBusy;
}

inline void ThreadPool::joinJobs() const
{
    while ( active_jobs_ )
        ;
}

inline size_t ThreadPool::threadCount() const
{
    return threads_.size();
}

inline void ThreadPool::stop()
{
    //* Only need to check for empty queue, because running jobs will be finished before threads are joined
    while ( isJobQueued() )
        ;

    {
        std::unique_lock<std::mutex> lock( queue_mutex_ );
        should_terminate_ = true;
    }

    mutex_condition_.notify_all();

    for ( std::thread& active_thread : threads_ )
    {
        active_thread.join();
    }

    threads_.clear();
}

inline ThreadPool::~ThreadPool()
{
    stop();
}

#endif
