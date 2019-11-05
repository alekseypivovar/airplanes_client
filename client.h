#ifndef CLIENT_H
#define CLIENT_H
#include <QWidget>
#include <QTcpSocket>
#include "playerinfo.h"

class Client : public QObject
{
    Q_OBJECT
private:
    QTcpSocket* pTcpSpcket;
    quint16 blockSize;


public:
    explicit Client(const QString &strHost, int nPort, QObject *parent = nullptr);
    void SendToServer (PlayerInfo player);

public slots:
    void SlotReadIdAndMap();
    void SlotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotConnected();
signals:
    void mapAndIdReceived(idAndMap info);
    void coordsReceived(QVector<PlayerInfo> players);
};

#endif // CLIENT_H
