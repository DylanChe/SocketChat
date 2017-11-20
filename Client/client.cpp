#include "client.h"
#include <QtNetwork>
#include <QThread>
#include <iostream>
#include <sstream>

Client::Client()
    :   networkSession(0)
{
    m_num_ordre = 0;
    m_tcpSocket = new QTcpSocket(this);

    // La méthode lireTexte sera appelée sur le signal readyRead
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(lireTexte()));

    // La méthode ServeurTrouve sera appelée sur le signal hostFound
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(connected()));

    // La méthode ServeurTrouve sera appelée sur le signal disconnected
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(deconnected()));

    // La méthode afficherErreur sera appelée sur le signal error
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(afficherErreur(QAbstractSocket::SocketError)));

    QNetworkConfigurationManager manager;
    QNetworkConfiguration config;

    std::cout << "Ouverture session" << std::endl;
    networkSession = new QNetworkSession(config, this);

    networkSession->open();

    m_nbOctetsLus = 0;
    m_tcpSocket->abort();

    connexion();
}

void Client::connexion()
{
    std::cout << "-----------------------------------" << std::endl;
    m_num_ordre++;
    std::cout << "Client numero " << m_num_ordre << std::endl;

    // demande de connexion au serveur
    m_tcpSocket->connectToHost( QHostAddress(QHostAddress::LocalHost).toString(),53000 );
}

void Client::lireTexte()
{
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    if (m_nbOctetsLus == 0) {
        if (m_tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;


        in >> m_nbOctetsLus;
    }

    if (m_tcpSocket->bytesAvailable() < m_nbOctetsLus)
        return;

    QString texte;
    in >> texte;

    std::cout << "3- Reception de : " ;
    std::cout << texte.toStdString() << std::endl;
    m_nbOctetsLus = 0;

    m_tcpSocket->close();
}


void Client::envoiTexte( const std::string& s )
{
    std::string s1 = formater_string(s);
    std::cout << "2- Envoi de : " << s1 << std::endl;
    QString texte = QString::number(m_num_ordre) + "|" + tr(s1.c_str());
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << texte;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    m_tcpSocket->write(block);
}

void Client::deconnected()
{
    std::cout << "4- deconnexion" << std::endl;
    std::cout << std::endl;

    std::cout << "5- fermeture dans 3 secondes..." << std::endl;
    QThread::sleep(3);
    QCoreApplication::exit();
}

void Client::afficherErreur(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        std::cout << "Le serveur n'a pa ete trouve. Verifiez le parametrage du serveur et du port." << std::endl;
        break;
    case QAbstractSocket::ConnectionRefusedError:
        std::cout << "La communication a ete refusee. Verifiez que le serveur est pret, ainsi que le parametrage du serveur et du port." << std::endl;
        break;
    default:
        std::cout << "L'erreur suivante s'est produite : " << m_tcpSocket->errorString().toStdString() << std::endl;
    }
}

void Client::connected()
{
    std::cout << "1- Connexion au serveur effectuee" << std::endl;
    std::cout << "Que voulez-vous envoyer ?" << std::endl;
    std::string s;
    getline(std::cin, s);

    envoiTexte("AFFICHER|" + s);
}

std::string Client::formater_string( const std::string & s ) const
{
    std::string result( s );
    for ( unsigned int i = 0; i < result.size(); ++i )
        std::cout << "\t[" << i << "] = " << (int)result[i] << std::endl;

    // A faire

    return result;
}
