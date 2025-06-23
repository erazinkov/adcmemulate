#ifndef ADCMEMULATEPROCESS_H
#define ADCMEMULATEPROCESS_H

#include <QObject>

#include "adcmemulatequery.h"

class ADCMEmulateProcess : public QObject
{
    Q_OBJECT
public:
    ADCMEmulateProcess(const ADCMEmulateQuery query, const QList<qint64> offsets, QObject *parent = nullptr);
    void handle() const;
    void emulate();
private:
    struct SelectedPosition {
        qint64 position;
        qsizetype size;
    };
    const ADCMEmulateQuery m_query;
    QList<qint64> m_offsets;
};

#endif // ADCMEMULATEPROCESS_H
