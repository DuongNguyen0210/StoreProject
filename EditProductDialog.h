#ifndef EDITPRODUCTDIALOG_H
#define EDITPRODUCTDIALOG_H

#include <QDialog>
#include "Product.h"
#include "Food.h"
#include "Beverage.h"
#include "HouseholdItem.h"

namespace Ui {
class EditProductDialog;
}

class EditProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditProductDialog(Product* product, QWidget *parent = nullptr);
    ~EditProductDialog();

    QString getName() const;
    double getPrice() const;
    double getImportPrice() const;
    double getProfitMargin() const;
    int getQuantity() const;
    QString getExpiryDate() const;
    double getVolume() const;
    int getWarranty() const;

private slots:
    void validateForm();
    void calculateSellingPrice();

private:
    Ui::EditProductDialog *ui;
    Product* m_product;
    void setupFormForProductType();
};

#endif
