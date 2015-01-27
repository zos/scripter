#ifndef OVERLORD_H
#define OVERLORD_H

#include <QObject>
#include <QTcpServer>

class Overlord : public QObject
{
    Q_OBJECT
public:
    explicit Overlord(QObject *parent = 0);

signals:

public slots:
    void startServer();

private:
    QTcpServer *m_server;


};

#endif // OVERLORD_H
