#ifndef GAMEVIEW_H
#define GAMEVIEW_H
#include <QGraphicsView>
#include <QTcpSocket>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include "playerinfo.h"
#include "client.h"
#include "plane.h"


class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    GameView(Client* client, idAndMap& info);

private:
    qint16 id;
    QVector<PlayerInfo> players;
    Client* client;
    QVector<Plane*> planes;
    QTimer* animationTimer;

    void drawMap(QVector <QString>& map) const;

    void exitProgramm();
    void rotateLeft();
    void rotateRight();
    void noRotation();
    void fire();

    void createNewPlayer(PlayerInfo& player);
    void updatePlayerParams(PlayerInfo& player);

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

private slots:
    void updatePlayersCoords(QVector <PlayerInfo> players);
    void updatePlanePos(Plane* plane);
};

#endif // GAMEVIEW_H
