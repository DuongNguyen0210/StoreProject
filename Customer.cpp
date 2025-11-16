#include "Customer.h"
#include <QDebug> // QDebug có thể cần cho Qt::CaseInsensitive, hoặc đã có trong QString

int Customer::nextId = 1; // Luôn bắt đầu từ 1

QString Customer::generateId()
{
    return QString("C%1").arg(nextId++);
}

// ----- HÀM ĐÃ SỬA LỖI LOGIC -----
Customer::Customer(const QString& id, const QString& name, const QString& phone, int points)
    : name(name), phone(phone), points(points)
{
    if (id.isEmpty())
    {
        // 1. ID rỗng: Tự động tạo ID mới
        this->id = generateId(); // Sẽ tạo "C1", "C2", "C3"...
    }
    else
    {
        // 2. ID có sẵn (ví dụ: "C1", "C2" từ mainwindow.cpp):
        this->id = id;

        // (PHẦN TỰ ĐỘNG CẬP NHẬT BỘ ĐẾM)
        // Kiểm tra xem ID có phải là dạng "C" không
        if (id.startsWith('C', Qt::CaseInsensitive))
        {
            bool ok = false;
            // Lấy phần SỐ từ ID (ví dụ: "C1" -> 1, "C2" -> 2)
            int idNum = id.mid(1).toInt(&ok);

            // Nếu lấy số thành công VÀ số đó lớn hơn hoặc bằng bộ đếm
            if (ok && idNum >= nextId)
            {
                // Cập nhật bộ đếm: ID tiếp theo phải là số này + 1
                // Ví dụ: khi "C2" được tạo, nextId sẽ thành 3
                nextId = idNum + 1;
            }
        }
    }
}
// ----- KẾT THÚC SỬA ĐỔI -----

const QString& Customer::getId() const
{
    return id;
}

const QString& Customer::getName() const
{
    return name;
}

const QString& Customer::getPhone() const
{
    return phone;
}

int Customer::getPoints() const
{
    return points;
}

void Customer::setName(const QString& n)
{
    name = n;
}

void Customer::setPhone(const QString& p)
{
    phone = p;
}

void Customer::setPoints(int p)
{
    points = p;
}

void Customer::addPoints(int p)
{
    points += p;
}
