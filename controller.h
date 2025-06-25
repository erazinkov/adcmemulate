#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QThread>

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller();
    ~Controller() override;
public slots:

signals:
    void operate(const QString &);
    void stopOperate();
    void closeApp();
    void handleResults(const QString &);
};
#endif // CONTROLLER_H
