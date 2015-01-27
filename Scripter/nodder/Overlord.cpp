#include "Overlord.h"

Overlord::Overlord(QObject *parent) :
    QObject(parent)
{
    connect(m_server, SIGNAL(newConnection())
}
