#include <QCoreApplication>
#include <chat_server.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ChatServer serveur;

    return a.exec();
}
