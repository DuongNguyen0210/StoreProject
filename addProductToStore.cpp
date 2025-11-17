#include "AddProductToStore.h"
#include "ui_addproducttostore.h"
#include <QDate>
#include <QPushButton>
#include <QMessageBox>

AddProductToStore::AddProductToStore(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddProductToStore)
{
    ui->setupUi(this);

    ui->Type->setPlaceholderText("Chọn loại sản phẩm...");
    ui->Type->addItem("Chọn loại sản phẩm...");
    ui->Type->addItem("Đồ ăn");
    ui->Type->addItem("Thức uống");
    ui->Type->addItem("Đồ gia dụng");

    ui->Price->setMinimum(0.0);
    ui->Price->setMaximum(1000000.0);
    ui->Price->setSuffix(" đ");

    ui->Quantity->setMinimum(0);
    ui->Quantity->setMaximum(10000);

    ui->Volume->setMinimum(0.0);
    ui->Volume->setMaximum(100000.0);
    ui->Volume->setSuffix(" ml");

    ui->dateEdit->setDisplayFormat("dd/MM/yyyy");
    QDate minDate(2025, 11, 17);
    ui->dateEdit->setMinimumDate(minDate.addDays(1));
    ui->dateEdit->setDate(minDate.addDays(1));

    connect(ui->Name, &QLineEdit::textChanged, this, &AddProductToStore::validateForm);
    connect(ui->Quantity, QOverload<int>::of(&QSpinBox::valueChanged), this, &AddProductToStore::validateForm);
    connect(ui->Price, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &AddProductToStore::validateForm);
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &AddProductToStore::validateForm);
    connect(ui->Volume, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &AddProductToStore::validateForm);
    connect(ui->Warranty, QOverload<int>::of(&QSpinBox::valueChanged), this, &AddProductToStore::validateForm);

    setFieldsForType(ui->Type->currentIndex());
    validateForm();
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


void AddProductToStore::validateForm()
{
    bool isValid = true;
    int typeIndex = ui->Type->currentIndex();

    if (ui->Name->text().isEmpty())
        isValid = false;
    if (ui->Quantity->value() <= 0)
        isValid = false;

    if (typeIndex == 1 || typeIndex == 2)
    {
        if (ui->dateEdit->date() == QDate(2025, 11, 17) || !ui->dateEdit->date().isValid())
            isValid = false;
    }
    else if (typeIndex == 2)
    {
        if (ui->Volume->value() <= 0)
            isValid = false;
    }
    else if (typeIndex == 3)
    {
        if (ui->Warranty->value() <= 0)
            isValid = false;
    }
    else
        isValid = false;

    ui->buttonBox->setEnabled(isValid);
    if (ui->buttonBox)
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}

void AddProductToStore::setFieldsForType(int typeIndex)
{
    bool generalEnabled = (typeIndex != 0);

    ui->Name->setEnabled(generalEnabled);
    ui->Quantity->setEnabled(generalEnabled);
    ui->Price->setEnabled(generalEnabled);

    ui->dateEdit->setEnabled(typeIndex == 1 || typeIndex == 2);
    ui->Volume->setEnabled(typeIndex == 2);
    ui->Warranty->setEnabled(typeIndex == 3);
    validateForm();
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
