#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <QString>

class AppException : public std::runtime_error
{
public:
    explicit AppException(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

class InvalidDateException : public AppException
{
public:
    explicit InvalidDateException(const QString& msg)
        : AppException("Nhập sai định dạng ngày tháng năm: " + msg) {}
};

class InvalidInputException : public AppException
{
public:
    explicit InvalidInputException(const QString& msg)
        : AppException("Nhập sai định dạng đầu vào: " + msg) {}
};

class OutOfStockException : public AppException
{
public:
    explicit OutOfStockException(const QString& msg)
        : AppException("Hết Hàng rồi: " + msg) {}
};

class NotFoundException : public AppException
{
public:
    explicit NotFoundException(const QString& msg)
        : AppException("Không tìm thấy: " + msg) {}
};

class DuplicateException : public AppException
{
public:
    explicit DuplicateException(const QString& msg)
        : AppException("Đã tồn tại: " + msg) {}
};



namespace Validation {

inline bool isLeapYear(int year)
{
    return (year % 400 == 0) ||
           (year % 4 == 0 && year % 100 != 0);
}

inline bool isValidDate(int d, int m, int y)
{
    if (y < 1 || m < 1 || m > 12 || d < 1)
        return false;

    int maxDay;
    switch (m)
    {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        maxDay = 31; break;
    case 4: case 6: case 9: case 11:
        maxDay = 30; break;
    case 2:
        maxDay = isLeapYear(y) ? 29 : 28; break;
    default:
        return false;
    }
    return d <= maxDay;
}

inline void validateDateDDMMYYYY(const QString& dateStr)
{
    if (dateStr.size() != 10 || dateStr[2] != QChar('/') || dateStr[5] != QChar('/'))
        throw InvalidDateException("Sai định dạng (mong muốn dd/mm/yyyy): " + dateStr);

    bool okD, okM, okY;


    int d = dateStr.mid(0, 2).toInt(&okD);
    int m = dateStr.mid(3, 2).toInt(&okM);
    int y = dateStr.mid(6, 4).toInt(&okY);

    if (!okD || !okM || !okY)
    {
        throw InvalidDateException("Không thể phân tích : " + dateStr);
    }

    if (!isValidDate(d, m, y))
        throw InvalidDateException("Không tồn tại ngày này" + dateStr);
}
}

#endif
