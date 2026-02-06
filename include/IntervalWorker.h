#pragma once
#include <QThread>
#include <chrono>
#include <thread>
#include <functional>

class IntervalWorker : public QThread {
    Q_OBJECT
public:
    explicit IntervalWorker(int intervalMs, std::function<void()> task = nullptr)
        : m_interval(intervalMs), m_task(std::move(task)) {}

signals:
    void tick();

protected:
    void run() override {
        auto next_tick = std::chrono::steady_clock::now();
        const auto duration = std::chrono::milliseconds(m_interval);

        while (!isInterruptionRequested()) {
            // 1. Emit signal for UI updates (asynchronous)
            emit tick();

            // 2. Execute functional task if provided (synchronous in this thread)
            if (m_task) {
                m_task();
            }

            next_tick += duration;

            // 3. Precision sleep
            // Use a check to ensure we haven't already passed next_tick
            if (std::chrono::steady_clock::now() < next_tick) {
                std::this_thread::sleep_until(next_tick);
            } else {
                // If the task was so slow it exceeded 750ms, reset next_tick
                next_tick = std::chrono::steady_clock::now();
            }
        }
    }

private:
    const int m_interval;
    const std::function<void()> m_task;
};
