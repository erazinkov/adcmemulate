#include "myprocess.h"

#include <QDebug>

#include <QCoreApplication>

MyProcess::MyProcess(QObject *parent) : QObject(parent)
{
}

MyProcess::~MyProcess()
{
    qDebug() << "Destructor called";
}

void MyProcess::stop()
{
    qDebug() << "Stopped.";
}

void MyProcess::process()
{
    qDebug() << "Start.";
    long long counter{0};

    for (long long i{0}; i < 2'000'000'000; ++i)
    {
        counter = i;
    }

    qDebug() << counter;
    emit(finished());
}


