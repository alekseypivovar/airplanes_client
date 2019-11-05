#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include "gameview.h"
#include "client.h"

namespace Ui {
class Menu;
}

class Menu : public QMainWindow
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();

private slots:
    void on_pushButton_join_clicked();

    void on_pushButton_exit_clicked();
    void startGame(idAndMap info);

private:
    Ui::Menu *ui;
    Client* client;
};

#endif // MENU_H
