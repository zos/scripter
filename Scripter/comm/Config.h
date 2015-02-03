#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>

#include <cstddef>

#include <QString>

namespace Config {
extern const std::string configPath;
}

class Configuration {
public:
    struct HostInfo {
        HostInfo() {}
        HostInfo(const QString &n, const QString &a, uint16_t p) : name(n), address(a), port(p) {}
        QString name;
        QString address;
        uint16_t port;
    };

    Configuration() {}
    typedef uint16_t PortNumber;
    typedef std::string HostAddress;

    void loadFromFile(const std::string &file);
    PortNumber getNodePort() {
        return m_nodePort;
    }

    QString getAddress() {
        return m_address;
    }

    PortNumber getHostPort() {
        return m_hostPort;
    }

    PortNumber getOverlordPort() {
        return m_overlordPort;
    }

    std::vector<HostInfo> getHosts() {
        return m_hosts;
    }

private:
    PortNumber m_nodePort;
    PortNumber m_hostPort;
    PortNumber m_overlordPort;
    QString m_address;
    typedef std::vector<HostInfo> HostInfos;

    HostInfos m_hosts;

};

#endif // CONFIG_H
