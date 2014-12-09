#include "renamedialog.h"
#include "ui_renamedialog.h"

renameDialog::renameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::renameDialog)
{
    ui->setupUi(this);
}

renameDialog::~renameDialog()
{
    delete ui;
}

void renameDialog::on_pushButton_clicked()
{
    if(ui->lineEdit->text() == ""){
        ui->lineEdit->setText("Empty name is not allowed");
        return;
    }
    else{
        emit rename(ui->lineEdit->text());
        this->close();
    }
}
