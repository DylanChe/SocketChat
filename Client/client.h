#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QObject>
#include <string>

class QTcpSocket;
class QNetworkSession;

class Client : public QObject
{
    Q_OBJECT

public:
    Client();

private slots:
    void lireTexte();
    void afficherErreur(QAbstractSocket::SocketError socketError);
    void connected();
    void deconnected();

private:
    void envoiTexte( const std::string& s);
    void connexion();
    std::string formater_string( const std::string & s ) const;

private:
    QTcpSocket *m_tcpSocket;

    quint16 m_nbOctetsLus;

    QNetworkSession *networkSession;

    int m_num_ordre;
};

#endif
