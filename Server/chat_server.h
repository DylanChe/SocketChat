#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <QObject>
#include <QTcpSocket>

class QTcpServer;
class QTcpSocket;
class QNetworkSession;

class ChatServer : public QObject
{
    Q_OBJECT
public:
    ChatServer();

private:
    QTcpServer *m_tcpServer;
    QTcpSocket *m_socketClient;
    QNetworkSession *m_networkSession;
    quint16 m_nbOctetsLus;

private slots:
    void sessionOuverte();
    void connexionClient();
    void lireTexte();
    void deconnexionClient();
    void afficherErreur(QAbstractSocket::SocketError socketError);
private:
    void envoiTexte( const std::string& s);
};

#endif // CHAT_SERVER_H
