#include "mainclass.h"

#include <QTimer>
#include <QDebug>
#include <QCoreApplication>

#include <QCommandLineParser>
#include "adcmemulatequery.h"
#include "adcmemulateparser.h"

MainClass::MainClass(QObject *parent) : QObject(parent)
{
    MainClass::setSignalHandlerObject(this);
    connect(&m_controller, &Controller::handleResults, this, [&](){
        closeApp();
    });
    m_controller.operate("Start");
//    QCommandLineParser parser;
//    ADCMEmulateQuery query;
//    ADCMEmulateParser adcmEmulateParser(parser, query);
//    auto parseResult = adcmEmulateParser.parseResult();
//    qDebug() << parseResult;
}

MainClass *MainClass::realSelf;
void MainClass::handleSignal(int /*num*/)
{
    closeApp();
}

void MainClass::closeApp()
{
    m_controller.stopOperate();
    qDebug() << "good bye";
    QCoreApplication::quit();
}
