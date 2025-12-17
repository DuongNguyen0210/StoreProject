#include "EditProductDialog.h"
#include "ui_EditProductDialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QDate>

EditProductDialog::EditProductDialog(Product* product, QWidget *parent)
    : QDialog(parent), ui(new Ui::EditProductDialog), m_product(product)
{
    ui->setupUi(this);
    ui->ImportPrice->setMinimum(0.0);
    ui->ImportPrice->setMaximum(1000000.0);
    ui->ImportPrice->setSuffix(" đ");

    ui->ProfitMargin->setMinimum(0.0);
    ui->ProfitMargin->setMaximum(1000.0);
    ui->ProfitMargin->setSuffix(" %");
    ui->ProfitMargin->setDecimals(1);

    ui->Price->setMinimum(0.0);
    ui->Price->setMaximum(1000000000.0);
    ui->Price->setSuffix(" đ");
    ui->Price->setReadOnly(true);
    ui->Price->setButtonSymbols(QAbstractSpinBox::NoButtons);

    ui->Quantity->setMinimum(0);
    ui->Quantity->setMaximum(100000);

    ui->Volume->setMinimum(0.0);
    ui->Volume->setMaximum(100000.0);
    ui->Volume->setSuffix(" ml");

    ui->Warranty->setMinimum(0);
    ui->Warranty->setMaximum(1000);
    ui->Warranty->setSuffix(" tháng");

    ui->dateEdit->setDisplayFormat("dd/MM/yyyy");

    connect(ui->ImportPrice, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &EditProductDialog::calculateSellingPrice);
    connect(ui->ProfitMargin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &EditProductDialog::calculateSellingPrice);

    connect(ui->Name, &QLineEdit::textChanged, this, &EditProductDialog::validateForm);
    connect(ui->Quantity, QOverload<int>::of(&QSpinBox::valueChanged), this, &EditProductDialog::validateForm);
    connect(ui->ImportPrice, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &EditProductDialog::validateForm);
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &EditProductDialog::validateForm);
    connect(ui->Volume, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &EditProductDialog::validateForm);
    connect(ui->Warranty, QOverload<int>::of(&QSpinBox::valueChanged), this, &EditProductDialog::validateForm);

    setupFormForProductType();
    validateForm();
}

EditProductDialog::~EditProductDialog()
{
    delete ui;
}

void EditProductDialog::setupFormForProductType()
{
    Food* f = dynamic_cast<Food*>(m_product);
    Beverage* b = dynamic_cast<Beverage*>(m_product);
    HouseholdItem* h = dynamic_cast<HouseholdItem*>(m_product);

    ui->lblProductId->setText(QString("Mã sản phẩm: %1").arg(m_product->getId()));
    ui->Name->setText(m_product->getName());
    ui->ImportPrice->setValue(m_product->getImportPrice());
    ui->ProfitMargin->setValue(m_product->getProfitMargin());
    ui->Quantity->setValue(m_product->getQuantity());

    if (f)
    {
        ui->lblType->setText("Loại: Đồ ăn");
        ui->labelExpiryDate->setVisible(true);
        ui->dateEdit->setVisible(true);
        ui->labelVolume->setVisible(false);
        ui->Volume->setVisible(false);
        ui->labelWarranty->setVisible(false);
        ui->Warranty->setVisible(false);
        ui->dateEdit->setDate(QDate::fromString(f->getExpiryDate(), "dd/MM/yyyy"));
    }
    else if (b)
    {
        ui->lblType->setText("Loại: Thức uống");
        ui->labelExpiryDate->setVisible(true);
        ui->dateEdit->setVisible(true);
        ui->labelVolume->setVisible(true);
        ui->Volume->setVisible(true);
        ui->labelWarranty->setVisible(false);
        ui->Warranty->setVisible(false);
        ui->dateEdit->setDate(QDate::fromString(b->getExpiryDate(), "dd/MM/yyyy"));
        ui->Volume->setValue(b->getVolume());
    }
    else if (h)
    {
        ui->lblType->setText("Loại: Đồ gia dụng");
        ui->labelExpiryDate->setVisible(false);
        ui->dateEdit->setVisible(false);
        ui->labelVolume->setVisible(false);
        ui->Volume->setVisible(false);
        ui->labelWarranty->setVisible(true);
        ui->Warranty->setVisible(true);
        ui->Warranty->setValue(h->getWarrantyMonths());
    }
}

void EditProductDialog::validateForm()
{
    bool isValid = true;

    if (ui->Name->text().trimmed().isEmpty())
        isValid = false;

    if (ui->ImportPrice->value() <= 0)
        isValid = false;

    Food* f = dynamic_cast<Food*>(m_product);
    Beverage* b = dynamic_cast<Beverage*>(m_product);
    HouseholdItem* h = dynamic_cast<HouseholdItem*>(m_product);

    if ((f || b) && ui->dateEdit->date() < QDate::currentDate())
        isValid = false;

    if (b && ui->Volume->value() <= 0)
        isValid = false;

    if (h && ui->Warranty->value() < 0)
        isValid = false;

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}

QString EditProductDialog::getName() const
{
    return ui->Name->text().trimmed();
}

double EditProductDialog::getPrice() const
{
    return ui->Price->value();
}

int EditProductDialog::getQuantity() const
{
    return ui->Quantity->value();
}

QString EditProductDialog::getExpiryDate() const
{
    return ui->dateEdit->date().toString("dd/MM/yyyy");
}

double EditProductDialog::getVolume() const
{
    return ui->Volume->value();
}

int EditProductDialog::getWarranty() const
{
    return ui->Warranty->value();
}

void EditProductDialog::calculateSellingPrice()
{
    double importPrice = ui->ImportPrice->value();
    double profitMargin = ui->ProfitMargin->value();
    double sellingPrice = importPrice + (importPrice * profitMargin / 100.0);
    ui->Price->setValue(sellingPrice);
}

double EditProductDialog::getImportPrice() const
{
    return ui->ImportPrice->value();
}

double EditProductDialog::getProfitMargin() const
{
    return ui->ProfitMargin->value();
}
