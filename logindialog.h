#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "Store.h"
#include "User.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(Store* store, QWidget *parent = nullptr);
    ~LoginDialog();

    User* getLoggedInUser() const;

private slots:
    void onLoginClicked();

private:
    Ui::LoginDialog *ui;
    Store* m_store;
    User* m_loggedInUser;
};

#endif
