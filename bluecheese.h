#ifndef BLUECHEESE_H
#define BLUECHEESE_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
class bluecheese;
}
QT_END_NAMESPACE

class bluecheese : public QMainWindow
{
    Q_OBJECT

public:
    bluecheese(QWidget *parent = nullptr);
    ~bluecheese();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_actionCipher_triggered();

    void on_actionDecipher_triggered();

    void on_actionSave_triggered();

    void on_actionAbout_triggered();

    void on_actionQuit_triggered();

private:
    Ui::bluecheese *ui;
};
#endif // BLUECHEESE_H
