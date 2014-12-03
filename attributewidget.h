#ifndef ATTRIBUTEWIDGET_H
#define ATTRIBUTEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QLayout>
#include <QPushButton>

namespace Ui {
class attributeWidget;
}

class attributeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit attributeWidget(QWidget *parent = 0);
    ~attributeWidget();

    void viewable(bool noSel, bool selObj, bool btn, bool top, bool bot, bool h, bool facen, bool r, bool dens);

signals:
    void reshape();

private slots:
    void sendReshape();

private:
    Ui::attributeWidget *ui;

public:
    QVBoxLayout *mainLayout;
    QPushButton *button;

    QHBoxLayout *layout1;
    QLabel *topRLabel;
    QDoubleSpinBox *topR;

    QHBoxLayout *layout2;
    QLabel *bottomRLabel;
    QDoubleSpinBox *botR;

    QHBoxLayout *layout3;
    QLabel *heightLabel;
    QDoubleSpinBox *height;

    QHBoxLayout *layout4;
    QLabel *faceNumLabel;
    QDoubleSpinBox *faceNum;

    QHBoxLayout *layout5;
    QLabel *RLabel;
    QDoubleSpinBox *R;

    QHBoxLayout *layout6;
    QLabel *densityLabel;
    QSpinBox *density;

    QLabel *noSelect;
    QLabel *selectObj;
};

#endif // ATTRIBUTEWIDGET_H
