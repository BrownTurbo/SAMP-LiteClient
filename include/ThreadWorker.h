#pragma once
#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QtCore/QThread>
#include <QtCore/QList>
#include <QtCore/QQueue>
#include <QtCore/QDebug>
#include <QtCore/QObject>

class ThreadManager : public QObject {
    Q_OBJECT
public:
    ThreadManager(int maxThreads) : maxThreads(maxThreads), threadCount(0) {}

    template <typename WorkerType>
    void queueWorker(WorkerType* worker) {
        if (threads.count() < maxThreads) {
            workerQueue.enqueue(worker);
        } else {
            qDebug() << "Max threads reached, worker not queued.";
        }
    }

    void processNextWorker() {
        if (!workerQueue.isEmpty() && threads.count() < maxThreads) {
            auto worker = workerQueue.dequeue();
            threads.append(worker);
            connect(worker, &QThread::finished, this, &ThreadManager::onWorkerFinished);
            worker->start();

            threadCount++;
            qDebug() << "Thread started. ID:" << worker->currentThreadId() << "Thread number:" << threadCount;
        }
    }

    void stopAllThreads(int waitT = -1) {
        for (auto thread : threads) {
            thread->requestInterruption();
            thread->quit();
            if(waitT == -1)
                thread->wait();
            else
                thread->wait(waitT);
            thread->deleteLater();
            delete thread;
        }
        threads.clear();
        workerQueue.clear();
    }

private slots:
    void onWorkerFinished() {
        QThread *finishedThread = qobject_cast<QThread *>(sender());
        if (finishedThread) {
            qDebug() << "Thread finished. ID:" << finishedThread->currentThreadId()
                     << "Thread number:" << threadCount;
            processNextWorker();
        }
    }

private:
    QList<QThread*> threads;
    QQueue<QThread*> workerQueue;
    int maxThreads;
    int threadCount;
};

#endif
