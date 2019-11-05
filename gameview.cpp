#include "gameview.h"


GameView::GameView(Client *client, idAndMap &info)
{
    this->client = client;
    this->id     = info.id;

    drawMap(info.map);

    connect(client, SIGNAL(coordsReceived(QVector<PlayerInfo>)), this, SLOT(updatePlayersCoords(QVector<PlayerInfo>)));
}

void GameView::drawMap(QVector<QString>& map) const
{
    // Устанавливаем размер сцены на весь мир
    this->scene()->setSceneRect(0,0, map.at(0).length() * TILE_SIZE, map.length() * TILE_SIZE);

    // Перебор и расстановка тайлов. j - номер тайла по оси Х, i - номер по оси Y
    for (int i = 0; i < map.length(); i++) {
        for (int j = 0; j < map.at(0).length(); j++) {
            QPixmap tile;
            if (map.at(i).at(j) == "0"){
                tile = QPixmap(":/images/0.png");
            }
            // else if остальные тайлы ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


            //           tile = tile.scaled(TILE_SIZE, TILE_SIZE);
            QGraphicsPixmapItem* pItem = this->scene()->addPixmap(tile);
            pItem->setPos(QPointF(j * TILE_SIZE, i * TILE_SIZE));
        }
    }
}

void GameView::exitProgramm()
{
    exit(0);
}

void GameView::rotateLeft()
{
    players[id].setAngleSpeed(ANGLE_SPEED);
}

void GameView::rotateRight()
{
    players[id].setAngleSpeed(-ANGLE_SPEED);
}

void GameView::noRotation()
{
    players[id].setAngleSpeed(0);
}

void GameView::fire()
{
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Left)
        rotateLeft();
    else if (event->key() == Qt::Key::Key_Right)
        rotateRight();
    else if (event->key() == Qt::Key::Key_Space)
        fire();
    else if (event->key() == Qt::Key::Key_Escape)
        exitProgramm();
}

void GameView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Left || event->key() == Qt::Key::Key_Right)
        noRotation();
}


void GameView::updatePlayersCoords(QVector<PlayerInfo>& players)
{
    this->players = players;
    // Добавить всякие проверки на мертвость и так далее
}
