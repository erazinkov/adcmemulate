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
        std::cout << "/home/egor/Documents/tochka_1"
                  << " MP: " << decoder.offsets().size()
                  << " TIME: " << decoder.counters().time
                  << std::endl;
        ADCMEmulateProcess adcmEmulateProcess(adcmEmulateParser.query(),
                                              QList<qint64>(decoder.offsets().begin(), decoder.offsets().end()));
        adcmEmulateProcess.handle();
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

