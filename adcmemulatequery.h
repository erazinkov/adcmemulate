#ifndef ADCMEMULATEQUERY_H
#define ADCMEMULATEQUERY_H

#include <QString>

struct ADCMEmulateQuery
{
    QString input;
    QString output;
    long long begin{0};
    long long end{0};
    long long number{1};
    long long size{1};
    ulong delay{0};
    long long overlap{0};
};

#endif // ADCMEMULATEQUERY_H
