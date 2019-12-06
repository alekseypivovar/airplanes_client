#include "gameview.h"


GameView::GameView(Client *client, idAndMap &info) :
    machinegunSound(":/sounds/machinegun.wav"),
    explosionSound(":/sounds/explosion.wav"),
    planeSound(":/sounds/airplane.wav")
{
    this->client = client;
    this->id     = info.id;
    /*this->players = QVector<PlayerInfo>(32)*/
    QGraphicsScene* scene = new QGraphicsScene();
    this->setScene(scene);

    drawMap(info.map);

    connect(client, SIGNAL(coordsReceived(QVector<PlayerInfo>)),
            this, SLOT(updatePlayersCoords(QVector<PlayerInfo>)), Qt::QueuedConnection);
    connect(this, SIGNAL(SendToServer(PlayerInfo, SendInfoType)),
            client, SLOT(SendToServer(PlayerInfo, SendInfoType)), Qt::QueuedConnection);
    connect(client, SIGNAL(bulletReceived(BulletInfo)), this, SLOT(createBullet(BulletInfo)), Qt::QueuedConnection);

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
    if (player.getId() == this-> id) {
        connect(plane, SIGNAL(planeMoved(Plane*)), this, SLOT(updatePlanePos(Plane*)));
        planeSound.setLoops(QSound::Infinite);
        planeSound.play();
    }
    connect(plane, SIGNAL(planeAndBulletCollided(Plane*, Bullet*)),
            this, SLOT(planeAndBulletCollided(Plane*, Bullet*)));
//    connect(plane, SIGNAL(planeAndPlaneCollided(Plane*, Plane*)),
//            this, SLOT(planeAndPlaneCollided(Plane*, Plane*)));
}

void GameView::updatePlayerParams(PlayerInfo &player)
{
    qint32 number = player.getId();
    QPointF oldPos = planes[number]->scenePos();
    if (players.at(number).getSpeed() == 0 && player.getSpeed() > 0) {
        planes [number]->show();
        controlsBlocked = false;
        if (number == id)
            planeSound.play();
    }

    players[number]. setSpeed(player.getSpeed());
    players[number]. setAngle(player.getAngle());
    players[number]. setAngleSpeed(player.getAngleSpeed());
    planes [number]->setSpeed(player.getSpeed());
    planes [number]->setAngleSpeed(player.getAngleSpeed());
    planes [number]->setAngle(player.getAngle());
    planes [number]->setPos(player.getPos());

    quint8 previousHealth = players[number].getHealth();
    players[number]. setHealth(player.getHealth());
    if (previousHealth > 0 && players[number].getHealth() <= 0) {
        planes [number]->hide();
        showExplosion(oldPos);
        if (number == id) {
            controlsBlocked = true;
            planeSound.stop();
            explosionSound.play();
        }
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


void GameView::updatePlayersCoords(QVector<PlayerInfo> players_in)
{
    for (PlayerInfo& playerInfo : players_in) {
        if (playerInfo.getId() < this->players.size()) {
            updatePlayerParams(playerInfo);
        }
        else {
            createNewPlayer(playerInfo);
        }

    }

//    this->players = players;

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
    if (QLineF((bullet.startPos), players.at(id).getPos()).length() < 700)
    {
        machinegunSound.play();
            //    QSound::play(":/sounds/machinegun.wav");
    }
}

void GameView::planeAndBulletCollided(Plane *plane, Bullet *bullet)
{
    Q_UNUSED(plane)
    this->scene()->removeItem(bullet);
    delete bullet;
//    if (players.at(plane->getId()).getHealth() <= 0) {
//        showExplosion(players.at(plane->getId()).getPos());
//    }
}

//void GameView::planeAndPlaneCollided(Plane* plane1, Plane* plane2)
//{
//    showExplosion(plane1->scenePos());
//    showExplosion(plane2->scenePos());
//}

void GameView::showExplosion(QPointF pos)
{
    pos = QPointF(pos.x() - IMAGE_SIZE / 2, pos.y() - IMAGE_SIZE / 2);
    QLabel *gif_anim = new QLabel();
    gif_anim->setStyleSheet("background: transparent");
    //gif_anim->setGeometry(0, 0, 72, 96);
    gif_anim->move(qint32(pos.x()), qint32(pos.y()));
    QMovie *movie = new QMovie(":/images/boom.gif");
    movie->setScaledSize(QSize(72, 96));
    gif_anim->setMovie(movie);
    // ЗВУК ВЗРЫВА +++++++++++++++++++++++++++++++++++++++++++++++++++++++
    movie->start();
    explosionSound.play();
    QGraphicsProxyWidget *proxy = scene()->addWidget(gif_anim);
    proxy->setPos(pos);
    QTimer::singleShot(800, [=]() {
        movie->stop();
        movie->deleteLater();
        gif_anim->deleteLater();
    } );
}
