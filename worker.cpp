#include "worker.h"
#include "channelmap.h"
#include "decoder.h"

#include <QDebug>
#include <QThread>

void Worker::doWork(const QString &parameter) {
    QString result;
    /* ... here is the expensive or blocking operation ... */
    qDebug() << "Start.";
    const ChannelMap pre = ChannelMap::mapNAP();
    Decoder decoder("/home/egor/shares/tmp/tochka_1", pre);
    auto p{decoder.positionsOfCMAPHeaders()};
    std::cout << "/home/egor/Documents/tochka_1" << " - " << p.size() << std::endl;
    // long long counter{0};

    // while (counter < 100'000 && !this->thread()->isInterruptionRequested())
    // {
    //     qDebug() << counter;
    //     counter++;
    // }

    // qDebug() << "Counter:" << counter;
    emit resultReady(QString("%1").arg(p.size()));
}

void Worker::stopWork()
{
    qDebug() << "StopWork";
}

