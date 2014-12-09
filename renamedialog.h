#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class renameDialog;
}

class renameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit renameDialog(QWidget *parent = 0);
    ~renameDialog();

signals:
    void rename(QString newName);

private slots:
    void on_pushButton_clicked();

private:
    Ui::renameDialog *ui;
};

#endif // RENAMEDIALOG_H
