#include "worker.h"

#include <QDebug>
#include <QThread>

void Worker::doWork(const QString &parameter) {
    QString result;
    /* ... here is the expensive or blocking operation ... */
    qDebug() << "Start.";
    long long counter{0};

    while (counter < 100'000 && !this->thread()->isInterruptionRequested())
    {
        qDebug() << counter;
        counter++;
    }

    qDebug() << "Counter:" << counter;
    emit resultReady(QString("%1").arg(counter));
}

void Worker::stopWork()
{
    qDebug() << "StopWork";
}

