#include "Menu.h"
#include "ui_Menu.h"

Menu::Menu(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);
}

Menu::~Menu()
{
    delete ui;
}

void Menu::on_pushButton_join_clicked()
{
    QString ip = ui->lineEdit_ip->text();
    client = new Client(ip, 2323, this);
    connect(client, SIGNAL(mapReceived(QVector <QString>)), this, SLOT(startGame(QVector<QString>)));
}

void __attribute__((noreturn)) Menu::on_pushButton_exit_clicked()
{
    exit(0);
}

void Menu::startGame(QVector<QString> map)
{
//    GameView* gameView = new GameView(.........); ++++++++++++++++++++++++++++++++
}
