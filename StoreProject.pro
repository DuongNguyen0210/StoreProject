QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Beverage.cpp \
    Bill.cpp \
    BillItem.cpp \
    Cashier.cpp \
    Customer.cpp \
    Food.cpp \
    HouseholdItem.cpp \
    Manager.cpp \
    Payment.cpp \
    Product.cpp \
    Store.cpp \
    User.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Beverage.h \
    Bill.h \
    BillItem.h \
    Cashier.h \
    Customer.h \
    Exceptions.h \
    Food.h \
    HouseholdItem.h \
    Manager.h \
    Payment.h \
    Product.h \
    Store.h \
    User.h \
    hashtable.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc
