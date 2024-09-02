QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Specify the include directory for Crypto++ headers
INCLUDEPATH += /usr/include/crypto++

# Specify the location of the Crypto++ library file
LIBS += -L/usr/lib/x86_64-linux-gnu/cryptopp -lcrypto++

SOURCES += \
    main.cpp \
    bluecheese.cpp

HEADERS += \
    bluecheese.h

FORMS += \
    bluecheese.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
