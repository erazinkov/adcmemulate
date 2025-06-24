#include "mainclass.h"

#include <QTimer>
#include <QDebug>
#include <QCoreApplication>

#include <iostream>

MainClass::MainClass(QObject *parent) : QObject(parent)
{
    MainClass::setSignalHandlerObject(this);
    connect(&m_controller, &Controller::handleResults, this, &MainClass::closeApp);
    m_controller.operate("Start");
}

MainClass *MainClass::realSelf;
void MainClass::handleSignal(int /*num*/)
{
    closeApp();
}

void MainClass::closeApp()
{
    m_controller.stopOperate();
    QCoreApplication::quit();
}
