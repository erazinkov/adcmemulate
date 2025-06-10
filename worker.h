#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString &parameter);
    void stopWork();
signals:
    void resultReady(const QString &result);

public:
    bool isCancelled{false};
};

#endif // WORKER_H
