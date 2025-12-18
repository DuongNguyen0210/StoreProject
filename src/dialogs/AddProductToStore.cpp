#include "dialogs/AddProductToStore.h"
#include "ui_addproducttostore.h"
#include <QDate>
#include <QPushButton>
#include <QMessageBox>

AddProductToStore::AddProductToStore(QWidget *parent) : QDialog(parent), ui(new Ui::AddProductToStore)
{
    ui->setupUi(this);

    ui->Type->setPlaceholderText("Chọn loại sản phẩm...");
    ui->Type->addItem("Chọn loại sản phẩm...");
    ui->Type->addItem("Đồ ăn");
    ui->Type->addItem("Thức uống");
    ui->Type->addItem("Đồ gia dụng");

    ui->ImportPrice->setMinimum(0.0);
    ui->ImportPrice->setMaximum(1000000.0);
    ui->ImportPrice->setSuffix(" đ");
    
    ui->ProfitMargin->setMinimum(0.0);
    ui->ProfitMargin->setMaximum(1000.0);
    ui->ProfitMargin->setSuffix(" %");
    ui->ProfitMargin->setDecimals(1);
    
    ui->Price->setMinimum(0.0);
    ui->Price->setMaximum(10000000.0);
    ui->Price->setSuffix(" đ");
    ui->Price->setReadOnly(true);
    ui->Price->setButtonSymbols(QAbstractSpinBox::NoButtons);

    ui->Quantity->setMinimum(0);
    ui->Quantity->setMaximum(10000);

    ui->Volume->setMinimum(0.0);
    ui->Volume->setMaximum(100000.0);
    ui->Volume->setSuffix(" ml");

    ui->dateEdit->setDisplayFormat("dd/MM/yyyy");
    ui->dateEdit->setMinimumDate(QDate::currentDate().addDays(1));
    ui->dateEdit->setDate(QDate::currentDate().addDays(1));

    ui->Type->setCurrentIndex(0);

    connect(ui->ImportPrice, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &AddProductToStore::calculateSellingPrice);
    connect(ui->ProfitMargin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &AddProductToStore::calculateSellingPrice);

    connect(ui->Name, &QLineEdit::textChanged, this, &AddProductToStore::valiForm);
    connect(ui->Quantity, QOverload<int>::of(&QSpinBox::valueChanged), this, &AddProductToStore::valiForm);
    connect(ui->ImportPrice, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &AddProductToStore::valiForm);
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &AddProductToStore::valiForm);
    connect(ui->Volume, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &AddProductToStore::valiForm);
    connect(ui->Warranty, QOverload<int>::of(&QSpinBox::valueChanged), this, &AddProductToStore::valiForm);

    setFieldsForType(ui->Type->currentIndex());
    valiForm();
}

AddProductToStore::~AddProductToStore()
{
    delete ui;
}

void AddProductToStore::on_Type_currentIndexChanged()
{
    int typeIndex = ui->Type->currentIndex();
    setFieldsForType(typeIndex);
}


void AddProductToStore::valiForm()
{
    bool isValid = true;
    int typeIndex = ui->Type->currentIndex();

    if (ui->Name->text().isEmpty())
        isValid = false;
    if (ui->Quantity->value() <= 0)
        isValid = false;
    if(ui->ImportPrice->value() == 0)
        isValid = false;
    if (typeIndex == 2)
    {
        if (ui->Volume->value() <= 0)
            isValid = false;
    }
    else if (typeIndex == 3)
    {
        if (ui->Warranty->value() <= 0)
            isValid = false;
    }

    if (ui->buttonBox)
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
    }
}

void AddProductToStore::setFieldsForType(int typeIndex)
{
    bool generalEnabled = (typeIndex != 0);

    ui->Name->setEnabled(generalEnabled);
    ui->Quantity->setEnabled(generalEnabled);
    ui->Price->setEnabled(generalEnabled);

    bool isFoodOrBeverage = (typeIndex == 1 || typeIndex == 2);
    bool isBeverage = (typeIndex == 2);
    bool isHousehold = (typeIndex == 3);

    ui->labelAddProductExpiryDate->setVisible(isFoodOrBeverage);
    ui->dateEdit->setVisible(isFoodOrBeverage);
    ui->dateEdit->setEnabled(isFoodOrBeverage);
    ui->ImportPrice->setEnabled(generalEnabled);
    ui->ProfitMargin->setEnabled(generalEnabled);

    ui->labelAddProductVolume->setVisible(isBeverage);
    ui->Volume->setVisible(isBeverage);
    ui->Volume->setEnabled(isBeverage);

    ui->labelAddProducWarranty->setVisible(isHousehold);
    ui->Warranty->setVisible(isHousehold);
    ui->Warranty->setEnabled(isHousehold);

    valiForm();
}

void AddProductToStore::calculateSellingPrice()
{
    double importPrice = ui->ImportPrice->value();
    double profitMargin = ui->ProfitMargin->value();
    double sellingPrice = importPrice + (importPrice * profitMargin / 100.0);
    ui->Price->setValue(sellingPrice);
}

QString AddProductToStore::getProductType() const
{
    return ui->Type->currentText();
}

QString AddProductToStore::getName() const
{
    return ui->Name->text();
}

double AddProductToStore::getPrice() const
{
    return ui->Price->value();
}

double AddProductToStore::getImportPrice() const
{
    return ui->ImportPrice->value();
}

double AddProductToStore::getProfitMargin() const
{
    return ui->ProfitMargin->value();
}

int AddProductToStore::getQuantity() const
{
    return ui->Quantity->value();
}

QString AddProductToStore::getExpiryDate() const
{
    return ui->dateEdit->date().toString("dd/MM/yyyy");
}

double AddProductToStore::getVolume() const
{
    return ui->Volume->value();
}

int AddProductToStore::getWarranty() const
{
    return ui->Warranty->value();
}
