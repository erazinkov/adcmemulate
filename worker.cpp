#include "worker.h"
#include "channelmap.h"
#include "decoder.h"

#include <QDebug>
#include <QThread>

#include "adcmemulateparser.h"
#include "adcmemulateprocess.h"

void Worker::doWork(const QString &parameter) {
    QString result;
    ADCMEmulateParser adcmEmulateParser;
    if (adcmEmulateParser.ok())
    {
        Decoder decoder(adcmEmulateParser.query().input.toStdString(), ChannelMap::mapNAP());
        decoder.process();
        qInfo() << adcmEmulateParser.query().input
                  << " MP: " << decoder.offsets().size()
                  << " TIME: " << decoder.counters().time;
        ADCMEmulateProcess adcmEmulateProcess(adcmEmulateParser.query(),
                                              QList<qint64>(decoder.offsets().begin(), decoder.offsets().end()));
        adcmEmulateProcess.process();
    }
    /* ... here is the expensive or blocking operation ... */

    // long long counter{0};

    // while (counter < 100'000 && !this->thread()->isInterruptionRequested())
    // {
    //     qDebug() << counter;
    //     counter++;
    // }

    // qDebug() << "Counter:" << counter;
    emit resultReady(QString("Finished"));
}

void Worker::stopWork()
{
    qDebug() << "StopWork";
}

