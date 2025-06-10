#include "controller.h"

#include "worker.h"
#include <QDebug>

Controller::Controller() {
    Worker *worker = new Worker;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Controller::operate, worker, &Worker::doWork);
    connect(this, &Controller::stopOperate, &workerThread, &QThread::requestInterruption);
    connect(worker, &Worker::resultReady, this, &Controller::handleResults);
    workerThread.start();
}

Controller::~Controller() {
    workerThread.quit();
    workerThread.wait();
}

//void Controller::handleResults(const QString &result)
//{
////    qDebug() << "Result:" << result;
////    emit(closeApp());
//}
