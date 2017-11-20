#include <QCoreApplication>
#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // création du serveur
    Client c1;

    // on attend un événement quit
    return a.exec();
}
