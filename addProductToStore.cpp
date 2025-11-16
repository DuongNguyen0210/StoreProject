#include "addProductToStore.h"
#include "ui_addProductToStore.h"
#include "Product.h"

AddStockDialog::AddStockDialog(Store* store, QWidget *parent)
    : QDialog(parent), ui(new Ui::AddStockDialog), m_store(store)
{
    ui->setupUi(this);

    ui->spinQuantity->setMinimum(1);
    ui->spinQuantity->setMaximum(9999);

    if (m_store)
    {
        m_store->forEachProduct(
        [this](const QString&, Product* p)
        {
            if (p)
            {
                ui->comboProducts->addItem(p->getName(), p->getId());
            }
        });
    }
}

AddStockDialog::~AddStockDialog()
{
    delete ui;
}

QString AddStockDialog::getSelectedProductId() const
{
    return ui->comboProducts->currentData().toString();
}

int AddStockDialog::getQuantity() const
{
    return ui->spinQuantity->value();
}
