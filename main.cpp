#include "ic_mqtt_gateway.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Ic_mqtt_gateway w;
    w.show();
    return a.exec();
}
