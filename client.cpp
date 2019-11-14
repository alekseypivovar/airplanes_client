#include "client.h"

Client::Client(const QString &strHost, int nPort, QObject *parent)
{
    mapReceived = false;
    pTcpSpcket = new QTcpSocket(this);
    pTcpSpcket->connectToHost(strHost, quint16(nPort));
    blockSize = 0;
    blockSize_map = 0;
    connect(pTcpSpcket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(pTcpSpcket, SIGNAL(readyRead()), this, SLOT(SlotReadIdAndMap()));
    //connect(pTcpSpcket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(pTcpSpcket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

}


void Client::SendToServer(const PlayerInfo player)
{
    QByteArray block;
    QDataStream out (&block, QIODevice::WriteOnly);
    out << quint16(0) << player;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof (quint16));
    pTcpSpcket->write(block);
    qDebug() << "Data SEND!";
}

void Client::SlotReadIdAndMap()
{
    qDebug() << "Read map!";
    QDataStream in(pTcpSpcket);

    if (blockSize_map == 0){
        if (pTcpSpcket->bytesAvailable() < int(sizeof (quint32)))
            return;
        in >> blockSize_map;
    }

    if (pTcpSpcket->bytesAvailable() < blockSize_map)
        return;

    blockSize_map = 0;
    idAndMap info;
    in >> info;

    if (!info.map.isEmpty()) {
        qDebug() << "Map RECEIVED";
        disconnect(pTcpSpcket, SIGNAL(readyRead()), this, SLOT(SlotReadIdAndMap()));
        mapReceived = true;
        pTcpSpcket->readAll();
        emit mapAndIdReceived(info);

        connect(pTcpSpcket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    }
}

void Client::SlotReadyRead()
{
    //qDebug() << "Read data...";
    QDataStream in(pTcpSpcket);

    if (blockSize == 0){
        if (pTcpSpcket->bytesAvailable() < int(sizeof (quint16)))
            return;
        in >> blockSize;
    }

    if (pTcpSpcket->bytesAvailable() < blockSize)
        return;

    blockSize = 0;
    QVector<PlayerInfo> players;
    in >> players;

    if (mapReceived)
        emit coordsReceived(players);
    qDebug() << "Data RECEIVED!";
}


void Client::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
            "Error: "+ (err == QAbstractSocket::HostNotFoundError ?
                            "The host was not found." :
                            err == QAbstractSocket::RemoteHostClosedError ?
                                "The remote host is closed." :
                                err == QAbstractSocket::ConnectionRefusedError?
                                    "The connection was refused." :
                                    QString(pTcpSpcket->errorString())
                                    );

    qDebug() << "Error: " << strError;
}

void Client::slotConnected()
{
    qDebug() << "Client connected!";
}



