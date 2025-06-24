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
                  << "Mp:" << decoder.offsets().size()
                  << "Time:" << decoder.counters().time;
        ADCMEmulateProcess adcmEmulateProcess(adcmEmulateParser.query(),
                                              QList<qint64>(decoder.offsets().begin(), decoder.offsets().end()));
        adcmEmulateProcess.process();
    }
    emit resultReady(QString("Finished"));
}

