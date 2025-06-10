#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>

#include "controller.h"

class MainClass : public QObject
{
    Q_OBJECT

    static MainClass *realSelf;
    void handleSignal(int num);
    static void setSignalHandlerObject(MainClass *newRealSelf)
    {
        MainClass::realSelf = newRealSelf;
    }
public:
    MainClass(QObject *parent = nullptr);
    static void callSignalHandler(int num)
    {
        realSelf->handleSignal(num);
    }

public slots:
    void closeApp();
private:
    Controller m_controller;
};

#endif // MAINCLASS_H
