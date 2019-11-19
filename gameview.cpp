#include "gameview.h"


GameView::GameView(Client *client, idAndMap &info)
{
    this->client = client;
    this->id     = info.id;
    /*this->players = QVector<PlayerInfo>(32)*/
    QGraphicsScene* scene = new QGraphicsScene();
    this->setScene(scene);

    drawMap(info.map);

    connect(client, SIGNAL(coordsReceived(QVector<PlayerInfo>)), this, SLOT(updatePlayersCoords(QVector<PlayerInfo>)));
    connect(this, SIGNAL(SendToServer(PlayerInfo, SendInfoType)), client, SLOT(SendToServer(PlayerInfo, SendInfoType)));
    connect(client, SIGNAL(bulletReceived(BulletInfo)), this, SLOT(createBullet(BulletInfo)));

    animationTimer = new QTimer;
    animationTimer->start(1000 / FRAMES_PER_SEC);
    if (scene != nullptr)
        connect(animationTimer, SIGNAL(timeout()), this->scene(), SLOT(advance()));

    this->setRenderHint(QPainter::Antialiasing, true);

    controlsBlocked = false;
}

void GameView::drawMap(QVector<QString>& map) const
{
    // Устанавливаем размер сцены на весь мир
    this->scene()->setSceneRect(0,0, (map.at(0).length() - 1) * TILE_SIZE, map.length() * TILE_SIZE);

    // Перебор и расстановка тайлов. j - номер тайла по оси Х, i - номер по оси Y
    for (int i = 0; i < map.length(); i++) {
        for (int j = 0; j < map.at(0).length(); j++) {
            QPixmap tile;
            if (map.at(i).at(j) == "0"){
                tile = QPixmap(":/images/0.bmp");
            }
            else if (map.at(i).at(j) == "1"){
                tile = QPixmap(":/images/1.bmp");
            }
            else if (map.at(i).at(j) == "2"){
                tile = QPixmap(":/images/2.bmp");
            }
            else if (map.at(i).at(j) == "3"){
                tile = QPixmap(":/images/3.bmp");
            }
            else if (map.at(i).at(j) == "4"){
                tile = QPixmap(":/images/4.bmp");
            }
            else if (map.at(i).at(j) == "5"){
                tile = QPixmap(":/images/5.bmp");
            }
            else if (map.at(i).at(j) == "6"){
                tile = QPixmap(":/images/6.bmp");
            }
            else if (map.at(i).at(j) == "7"){
                tile = QPixmap(":/images/7.bmp");
            }
            else if (map.at(i).at(j) == "8"){
                tile = QPixmap(":/images/8.bmp");
            }else if (map.at(i).at(j) == "A"){
                tile = QPixmap(":/images/A.bmp");
            }
            else if (map.at(i).at(j) == "B"){
                tile = QPixmap(":/images/B.bmp");
            }
            else if (map.at(i).at(j) == "C"){
                tile = QPixmap(":/images/C.bmp");
            }
            else if (map.at(i).at(j) == "D"){
                tile = QPixmap(":/images/D.bmp");
            }
            else {
                tile = QPixmap(":/images/E.bmp");
            }


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
    qDebug() << "Rotate LEFT!";
    players[id].setAngleSpeed(ANGLE_SPEED);
    emit SendToServer(players.at(id), COORDS);
}

void GameView::rotateRight()
{
    qDebug() << "Rotate RIGHT!";
    players[id].setAngleSpeed(-ANGLE_SPEED);
    emit SendToServer(players.at(id), COORDS);
}

void GameView::noRotation()
{
    qDebug() << "Rotation STOP!";
    players[id].setAngleSpeed(0);
    emit SendToServer(players.at(id), COORDS);
}

void GameView::fire()
{
     emit SendToServer(players.at(id), BULLET);
}

void GameView::createNewPlayer(PlayerInfo &player)
{
    players << player;

    Plane* plane = new Plane(id, player.getType(), player.getAngle(), player.getSpeed(), player.getAngleSpeed(), player.getHealth());
    this->scene()->addItem(plane);
    plane->setPos(player.getPos());
    planes << plane;
    if (player.getId() == this-> id)
        connect(plane, SIGNAL(planeMoved(Plane*)), this, SLOT(updatePlanePos(Plane*)));
    connect(plane, SIGNAL(planeAndBulletCollided(Plane*, Bullet*)),
            this, SLOT(planeAndBulletCollided(Plane*, Bullet*)));
}

void GameView::updatePlayerParams(PlayerInfo &player)
{
    qint32 number = player.getId();

    if (players.at(number).getSpeed() == 0 && player.getSpeed() > 0) {
        planes [number]->show();
        controlsBlocked = false;
    }

    players[number]. setSpeed(player.getSpeed());
    players[number]. setAngle(player.getAngle());
    players[number]. setAngleSpeed(player.getAngleSpeed());
    planes [number]->setSpeed(player.getSpeed());
    planes [number]->setAngleSpeed(player.getAngleSpeed());
    planes [number]->setAngle(player.getAngle());
    planes [number]->setPos(player.getPos());

    players[number]. setHealth(player.getHealth());
    if (players[number].getHealth() <= 0) {
        planes [number]->hide();
        controlsBlocked = true;
    }
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        event->ignore();
    }
    else {
        if (!controlsBlocked && event->key() == Qt::Key::Key_Left) {
            rotateLeft();
        }
        else if (!controlsBlocked && event->key() == Qt::Key::Key_Right)
        {
            rotateRight();
        }
        else if (!controlsBlocked && event->key() == Qt::Key::Key_Space)
            fire();
        else if (event->key() == Qt::Key::Key_Escape)
            exitProgramm();

        event->accept();
    }

}


void GameView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat() ) {
        event->ignore();
    } else {
        if (event->key() == Qt::Key::Key_Left || event->key() == Qt::Key::Key_Right){
            noRotation();
        }
        event->accept();
    }
}


void GameView::updatePlayersCoords(QVector<PlayerInfo> players)
{
    for (PlayerInfo& playerInfo : players) {
        if (playerInfo.getId() < this->players.size()) {
            updatePlayerParams(playerInfo);
        }
        else {
            createNewPlayer(playerInfo);
        }

    }

    this->players = players;

}

void GameView::updatePlanePos(Plane* plane)
{
    players[plane->getId()].setAngle(plane->getAngle());
    players[plane->getId()].setPos  (plane->scenePos());
    this->centerOn(plane->scenePos());

//    checkCollisions(plane);
}

void GameView::checkCollisions(Plane *plane)
{
    QList<QGraphicsItem *> items = scene()->collidingItems(plane);
    if (items.isEmpty()) {
        return;
    }

    for (int i = 0; i < items.length(); i++) {
        Bullet* bullet = qgraphicsitem_cast<Bullet*>(items.at(i));
        if (bullet) {
            scene()->removeItem(bullet);
            delete bullet;
        }
    }
}


void GameView::createBullet(BulletInfo bullet)
{
    Bullet* sceneBullet = new Bullet(bullet.startPos, bullet.angle);
    this->scene()->addItem(sceneBullet);
    sceneBullet->setPos(bullet.startPos);
}

void GameView::planeAndBulletCollided(Plane *plane, Bullet *bullet)
{
    Q_UNUSED(plane)
    this->scene()->removeItem(bullet);
    delete bullet;
}
