QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Include paths for new structure
INCLUDEPATH += include \
               include/models \
               include/core \
               include/dialogs \
               include/ui

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    src/dialogs/ManageCustomer.cpp \
    src/dialogs/AddCustomer.cpp \
    src/models/Product.cpp \
    src/models/Food.cpp \
    src/models/Beverage.cpp \
    src/models/HouseholdItem.cpp \
    src/models/Customer.cpp \
    src/models/User.cpp \
    src/models/Manager.cpp \
    src/models/Cashier.cpp \
    src/models/Bill.cpp \
    src/models/BillItem.cpp \
    src/models/Payment.cpp \
    src/core/Store.cpp \
    src/core/storePersistence.cpp \
    src/dialogs/AddProductToStore.cpp \
    src/dialogs/editproductdialog.cpp \
    src/dialogs/billdetaildialog.cpp \
    src/dialogs/StockProductDialog.cpp \
    src/dialogs/logindialog.cpp \
    src/dialogs/manageinventory.cpp \
    src/ui/mainwindow.cpp \
    src/ui/ThongKe.cpp

HEADERS += \
    include/dialogs/AddCustomer.h \
    include/dialogs/ManageCustomer.h \
    include/models/Product.h \
    include/models/Food.h \
    include/models/Beverage.h \
    include/models/HouseholdItem.h \
    include/models/Customer.h \
    include/models/User.h \
    include/models/Manager.h \
    include/models/Cashier.h \
    include/models/Bill.h \
    include/models/BillItem.h \
    include/models/Payment.h \
    include/core/Store.h \
    include/core/storepersistence.h \
    include/core/HashTable.h \
    include/dialogs/AddProductToStore.h \
    include/dialogs/EditProductDialog.h \
    include/dialogs/billdetaildialog.h \
    include/dialogs/StockProductDialog.h \
    include/dialogs/logindialog.h \
    include/dialogs/ManageInventory.h \
    include/ui/mainwindow.h \
    include/ui/ThongKe.h

FORMS += \
    ui/ManageCustomer.ui \
    ui/AddCustomer.ui \
    ui/mainwindow.ui \
    ui/ThongKe.ui \
    ui/AddProductToStore.ui \
    ui/editproductdialog.ui \
    ui/billdetaildialog.ui \
    ui/logindialog.ui \
    ui/manageinventory.ui \
    ui/stockProductdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/Resource.qrc

DISTFILES += \
    ../Store/Icon/banhmi.png \
    data/store_data.txt

# Copy store_data.txt to output directory
win32 {
    COPY_CMD = copy /y
} else {
    COPY_CMD = cp
}

# Define output directory (debug/release specifics)
CONFIG(debug, debug|release) {
    DEST_DIR = $$OUT_PWD/debug
    DATA_SRC = $$PWD/data/store_data.txt
} else {
    DEST_DIR = $$OUT_PWD/release
    DATA_SRC = $$PWD/data/store_data.txt
}

# Create data directory in output and copy file
QMAKE_POST_LINK += $$COPY_CMD \"$$shell_path($$DATA_SRC)\" \"$$shell_path($$DEST_DIR/store_data.txt)\"
