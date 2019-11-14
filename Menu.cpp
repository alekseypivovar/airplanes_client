#include "Menu.h"
#include "ui_Menu.h"

Menu::Menu(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);
    ui->pushButton_join->setFocus();
}

Menu::~Menu()
{
    delete ui;
}

void Menu::on_pushButton_join_clicked()
{
    QString ip = ui->lineEdit_ip->text();
    QThread* thread = new QThread;
    client = new Client(ip, 2323, this);
    client->moveToThread(thread);
    connect(client, SIGNAL(mapAndIdReceived(idAndMap&)), this, SLOT(startGame(idAndMap&)));
    thread->start();
}

void __attribute__((noreturn)) Menu::on_pushButton_exit_clicked()
{
    exit(0);
}

void Menu::startGame(idAndMap& info)
{
    GameView* gameView = new GameView(client, info);
    gameView->show(); // FULL SCREEN!
}
