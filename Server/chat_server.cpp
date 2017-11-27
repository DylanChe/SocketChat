#include "chat_server.h"
#include <QtNetwork>
#include <stdlib.h>
#include <iostream>


ChatServer::ChatServer()
    : m_tcpServer(0), m_networkSession(0)
{
    m_nbOctetsLus = 0;
    QNetworkConfigurationManager manager;
    QNetworkConfiguration config = manager.defaultConfiguration();
    m_networkSession = new QNetworkSession(config, this);

    connect(m_networkSession, SIGNAL(opened()), this, SLOT(sessionOuverte()));

    m_networkSession->open();

    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(connexionClient()));
}

void ChatServer::sessionOuverte()
{
    std::cout << "Ouverture de la session..." << std::endl;

    m_tcpServer = new QTcpServer(this);

    if(!m_tcpServer->listen(QHostAddress::Any, 53000))
    {
        std::cout << "Le serveur n'a pas ete correctement lancee." << std::endl;
    } else {
        std::cout << "Le serveur est pret :" << std::endl;
        //std::cout << " - IP : " << m_tcpServer->serverAddress().toString().toStdString() << std::endl;
        QStringList addresses;
        foreach (QHostAddress address, QNetworkInterface::allAddresses()) {
            if(address != QHostAddress::LocalHostIPv6
                    && address != QHostAddress::LocalHost
                    && !address.isInSubnet(QHostAddress::parseSubnet("192.168.0.0/16"))
                    && !address.isInSubnet(QHostAddress::parseSubnet("FE80::/64")))
            {
                addresses << address.toString();
            }
        }
        foreach (QString address, addresses) {
            std::cout << " - IP : " << address.toStdString() << std::endl;
        }
        std::cout << " - Port : " << m_tcpServer->serverPort() << std::endl;
        std::cout << "En attente de connexion client..." << std::endl;
    }
}

void ChatServer::connexionClient()
{
    std::cout << "[1] - Connexion d'un nouveau client." << std::endl;

    m_socketClient = m_tcpServer->nextPendingConnection();

    connect(m_socketClient, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));
    connect(m_socketClient, SIGNAL(readyRead()), this, SLOT(lireTexte()));

}

void ChatServer::deconnexionClient()
{
    std::cout << "[2] - Deconnexion d'un client." << std::endl;

    disconnect(m_socketClient, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));
    disconnect(m_socketClient, SIGNAL(readyRead()), this, SLOT(lireTexte()));

    m_socketClient->deleteLater();
}

void ChatServer::lireTexte()
{
    QDataStream in(m_socketClient);
    in.setVersion(QDataStream::Qt_4_0);

    if (m_nbOctetsLus == 0) {
        if (m_socketClient->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> m_nbOctetsLus;
    }

    if (m_socketClient->bytesAvailable() < m_nbOctetsLus)
        return;

    QString texte;
    in >> texte;

    std::cout << "2- Reception de : " ;
    std::cout << texte.toStdString() << std::endl;
    m_nbOctetsLus = 0;

    envoiTexte("Reponse envoyee par le serveur.");
}

void ChatServer::envoiTexte(const std::string& s)
{
    std::cout << "3- Envoi de : " << s << std::endl;
    QString texte = tr(s.c_str());
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << texte;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    m_socketClient->write(block);
}

void ChatServer::afficherErreur(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        std::cout << "[ERROR] RemoteHostClosedError" << std::endl;
        break;
    case QAbstractSocket::HostNotFoundError:
        std::cout << "[ERROR] HostNotFoundError" << std::endl;
        break;
    case QAbstractSocket::ConnectionRefusedError:
        std::cout << "[ERROR] ConnectionRefusedError" << std::endl;
        break;
    default:
        std::cout << "[ERROR] Unknown error" << std::endl;
        break;
    }
}
