#ifndef MYPROCESS_H
#define MYPROCESS_H

#include <QObject>

class MyProcess: public QObject
{
    Q_OBJECT
public:
    MyProcess(QObject *parent = nullptr);
    ~MyProcess() override;
public slots:
    void stop();
signals:
    void finished();
public:
    void process();
};

#endif // MYPROCESS_H
