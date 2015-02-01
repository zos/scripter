#include "Config.h"

#include <QSettings>

#include <QList>
#include <QString>

#include <log/Log.h>

namespace Config {
const std::string configPath = "/home/zosia/scripter_conf.ini";
}

void Configuration::loadFromFile(const std::string &file) {
    QSettings settings(QString::fromStdString(file), QSettings::IniFormat);

    settings.beginGroup("HOSTS");
    const QList<QString> allNodes = settings.childKeys();
    for (const QString& node : allNodes) {
        LOG(node.toStdString() << " : " << settings.value(node).toString().toStdString());
        m_hosts.push_back(HostInfo(node, settings.value(node).toString()));
    }
    settings.endGroup();

    m_address = settings.value("NODE/address").toString();
    m_hostPort = settings.value("NODE/hostPort").toUInt();
    m_nodePort = settings.value("NODE/selfPort").toUInt();
    m_overlordPort = settings.value("NODE/controllerPort").toUInt();
    LOG("hostPort: " << m_hostPort << ", nodePort: " << m_nodePort << ", m_overlordPort: " << m_overlordPort);
}

