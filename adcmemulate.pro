QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        adcmemulateparser.cpp \
        adcmemulateprocess.cpp \
        channelmap.cpp \
        controller.cpp \
        decoder.cpp \
        main.cpp \
        mainclass.cpp \
        myprocess.cpp \
        worker.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    adcm_df.h \
    adcmemulateparser.h \
    adcmemulateprocess.h \
    adcmemulatequery.h \
    channelmap.h \
    controller.h \
    decoder.h \
    mainclass.h \
    myprocess.h \
    progressbar.h \
    spinner.h \
    worker.h
