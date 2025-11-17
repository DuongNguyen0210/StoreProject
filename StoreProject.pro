QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AddCustomerToStore.cpp \
    AddProductToStore.cpp \
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
    ThongKe.cpp \
    User.cpp \
    billdetaildialog.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AddCustomerToStore.h \
    AddProductToStore.h \
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
    ThongKe.h \
    User.h \
    billdetaildialog.h \
    hashtable.h \
    logindialog.h \
    mainwindow.h

FORMS += \
    AddCustomerToStore.ui \
    AddProductToStore.ui \
    ThongKe.ui \
    billdetaildialog.ui \
    logindialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

DISTFILES += \
    ../Store/Icon/banhmi.png
