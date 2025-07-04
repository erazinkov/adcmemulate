#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT
public slots:
    void doWork(const QString &parameter);
signals:
    void resultReady(const QString &result);
};

#endif // WORKER_H
