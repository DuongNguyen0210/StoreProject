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

    // Cấu hình cho Giá gốc (Import Price)
    ui->ImportPrice->setMinimum(0.0);
    ui->ImportPrice->setMaximum(1000000.0);
    ui->ImportPrice->setSuffix(" đ");
    
    // Cấu hình cho % Lợi nhuận (Profit Margin)
    ui->ProfitMargin->setMinimum(0.0);
    ui->ProfitMargin->setMaximum(1000.0);  // Cho phép lợi nhuận lên đến 1000%
    ui->ProfitMargin->setSuffix(" %");
    ui->ProfitMargin->setDecimals(1);
    
    // Cấu hình cho Giá bán (Price) - chỉ đọc, tự động tính
    ui->Price->setMinimum(0.0);
    ui->Price->setMaximum(10000000.0);
    ui->Price->setSuffix(" đ");
    ui->Price->setReadOnly(true);
    ui->Price->setButtonSymbols(QAbstractSpinBox::NoButtons);  // Ẩn nút tăng/giảm

    ui->Quantity->setMinimum(0);
    ui->Quantity->setMaximum(10000);

    ui->Volume->setMinimum(0.0);
    ui->Volume->setMaximum(100000.0);
    ui->Volume->setSuffix(" ml");

    ui->dateEdit->setDisplayFormat("dd/MM/yyyy");
    QDate minDate(2025, 11, 17);
    ui->dateEdit->setMinimumDate(minDate.addDays(1));
    ui->dateEdit->setDate(minDate.addDays(1));

    // Kết nối tự động tính giá bán khi nhập Giá gốc hoặc % Lợi nhuận
    connect(ui->ImportPrice, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
            this, &AddProductToStore::calculateSellingPrice);
    connect(ui->ProfitMargin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
            this, &AddProductToStore::calculateSellingPrice);

    connect(ui->Name, &QLineEdit::textChanged, this, &AddProductToStore::validateForm);
    connect(ui->Quantity, QOverload<int>::of(&QSpinBox::valueChanged), this, &AddProductToStore::validateForm);
    connect(ui->ImportPrice, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &AddProductToStore::validateForm);
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
    // Kiểm tra Giá gốc thay vì Giá bán
    if(ui->ImportPrice->value() == 0)
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

    if (ui->buttonBox)
    {
        // Xử lý nút OK: Chỉ sáng khi isValid = true
        QPushButton* btnOk = ui->buttonBox->button(QDialogButtonBox::Ok);
        if (btnOk) {
            btnOk->setEnabled(isValid);
        }

        // Xử lý nút Cancel: Luôn luôn sáng
        QPushButton* btnCancel = ui->buttonBox->button(QDialogButtonBox::Cancel);
        if (btnCancel) {
            btnCancel->setEnabled(true);
        }
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
    // Hạn sử dụng (Đồ ăn, Thức uống)
    ui->labelAddProductExpiryDate->setVisible(isFoodOrBeverage);
    ui->dateEdit->setVisible(isFoodOrBeverage);
    ui->dateEdit->setEnabled(isFoodOrBeverage); // Giữ lại enabled để phục vụ validate
    ui->ImportPrice->setEnabled(generalEnabled);
    ui->ProfitMargin->setEnabled(generalEnabled);
    // Price luôn disabled vì tự động tính

    // Thể tích (Thức uống)
    ui->labelAddProductVolume->setVisible(isBeverage);
    ui->Volume->setVisible(isBeverage);
    ui->Volume->setEnabled(isBeverage); // Giữ lại enabled để phục vụ validate

    // Thời hạn bảo hành (Đồ gia dụng)
    ui->labelAddProducWarranty->setVisible(isHousehold);
    ui->Warranty->setVisible(isHousehold);
    ui->Warranty->setEnabled(isHousehold); // Giữ lại enabled để phục vụ validate

    // Gọi lại validateForm để cập nhật trạng thái nút OK
    validateForm();
}

// Hàm tự động tính giá bán từ giá gốc và % lợi nhuận
void AddProductToStore::calculateSellingPrice()
{
    double importPrice = ui->ImportPrice->value();
    double profitMargin = ui->ProfitMargin->value();
    
    // Công thức: Giá bán = Giá gốc + (Giá gốc * % Lợi nhuận / 100)
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
