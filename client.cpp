#include "client.h"

Client::Client(const QString &strHost, int nPort, QObject *parent)
{
    pTcpSpcket = new QTcpSocket(this);
    pTcpSpcket->connectToHost(strHost, quint16(nPort));
    blockSize = 0;
    connect(pTcpSpcket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(pTcpSpcket, SIGNAL(readyRead()), this, SLOT(SlotReadIdAndMap()));
    connect(pTcpSpcket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

}


void Client::SendToServer(PlayerInfo player)
{
    QByteArray block;
    QDataStream out (&block, QIODevice::WriteOnly);
    out << quint32(0) << player;
    out.device()->seek(0);
    out << quint32(block.size() - sizeof (quint32));
    pTcpSpcket->write(block);
}

void Client::SlotReadIdAndMap()
{
    QDataStream in(pTcpSpcket);

    if (blockSize == 0){
        if (pTcpSpcket->bytesAvailable() < int(sizeof (quint32)))
            return;
        in >> blockSize;
    }

    if (pTcpSpcket->bytesAvailable() < blockSize)
        return;

    blockSize = 0;
    idAndMap info;
    in >> info;

    if (!info.map.isEmpty()) {
        disconnect(pTcpSpcket, SIGNAL(readyRead()), this, SLOT(SlotReadIdAndMap()));

        emit mapAndIdReceived(info);

        connect(pTcpSpcket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    }
}

void Client::SlotReadyRead()
{
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

    emit coordsReceived(players);
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



