#ifndef ADDPRODUCTTOSTORE_H
#define ADDPRODUCTTOSTORE_H

#include <QDialog>
#include <QString>

namespace Ui
{
    class AddProductToStore;
}

class AddProductToStore : public QDialog
{
    Q_OBJECT

public:
    explicit AddProductToStore(QWidget *parent = nullptr);
    ~AddProductToStore();

    QString getProductType() const;
    QString getName() const;
    double getPrice() const;
    int getQuantity() const;

    QString getExpiryDate() const;
    double getVolume() const;
    int getWarranty() const;

private slots:
    void on_Type_currentIndexChanged();
    void setFieldsForType(int typeIndex);
    void validateForm();

private:
    Ui::AddProductToStore *ui;
};

#endif
