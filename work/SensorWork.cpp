// SensorWork.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <thread>
#include <spdlog/spdlog.h>
using namespace std;

/// <summary>
/// ֻʹ�ñ�׼�⽨����ʱѭ��������
/// ���� start()/shutdown() ���룬��û�п�������
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

