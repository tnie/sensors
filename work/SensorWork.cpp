// SensorWork.cpp : 定义控制台应用程序的入口点。
//

#include <thread>
#include <spdlog/spdlog.h>
using namespace std;

/// <summary>
/// 只使用标准库建立定时循环的任务。
/// 可以 start()/shutdown() 重入，但没有考虑性能
/// </summary>
class SensorWork
{
    const std::chrono::seconds INTERVAL_OF_SENSOR = 5s;
public:
    SensorWork() = default;
    void start()
    {
        {
            std::lock_guard<std::mutex> gd(_mt);
            if (_running) return;
            _running = true;
        }
        std::thread th([&]() {
            std::unique_lock<std::mutex> lock(_mt);
            while (!_cond.wait_for(lock, INTERVAL_OF_SENSOR, [&]() {return !_running; }))
            {
                spdlog::info("**tick");
            }
            spdlog::info("**shutdown");
        });
        std::swap(th, _th);
    }
    void shutdown()
    {
        {
            std::lock_guard<mutex> gd(_mt);
            if (!_running) return;
            _running = false;
            _cond.notify_one();
        }
        if (_th.joinable())
        {
            _th.join();
        }
    }
    ~SensorWork()
    {
        shutdown();
    }
private:
    std::thread _th;
    std::condition_variable _cond;
    std::mutex _mt;
    bool _running = false;
};

int main()
{
    spdlog::info("start");
    SensorWork ins;
    ins.start();
    this_thread::sleep_for(17s);
    spdlog::info("suspend");
    ins.shutdown();
    this_thread::sleep_for(17s);
    spdlog::info("contine");
    ins.start();
    this_thread::sleep_for(17s);
    spdlog::info("end");
    ins.shutdown();
    return 0;
}

