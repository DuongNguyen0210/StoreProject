#ifndef THONGKE_H
#define THONGKE_H

#include <QDialog>

namespace Ui
{
    class ThongKe;
}

class ThongKe : public QDialog
{
    Q_OBJECT

public:
    explicit ThongKe(double revenue, QWidget *parent = nullptr);
    ~ThongKe();

private:
    Ui::ThongKe *ui;
};

#endif
