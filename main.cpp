#include "ic_mqtt_test_client.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Ic_mqtt_test_client w;
    w.show();
    return a.exec();
}
