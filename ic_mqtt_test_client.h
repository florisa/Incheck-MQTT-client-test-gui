#ifndef IC_MQTT_TEST_CLIENT_H
#define IC_MQTT_TEST_CLIENT_H

#include<QMainWindow>
#include<QDataStream>
#include<QObject>
#include<QTcpSocket>
#include<QtCore/QString>

QT_BEGIN_NAMESPACE
namespace Ui { class Ic_mqtt_test_client; }
QT_END_NAMESPACE

class Ic_mqtt_test_client : public QMainWindow
{
    Q_OBJECT

public:
    Ic_mqtt_test_client(QWidget *parent = nullptr);
    ~Ic_mqtt_test_client();

    bool write_data(QByteArray data);
    QByteArray int_to_array(qint32 source);
    qint32 array_to_int(QByteArray source);

    void close();
    bool gateway_confirmation;

public slots:
    void send_out_handshake_package();
    void send_out_double_value();
    void my_ready_read();
    void process_tcp_package(QByteArray package);

signals:
    void complete_package_received(QByteArray);

private slots:
    void on_btn_connect_gateway_clicked();

    void on_btn_send_handshake_clicked();

private:
    QTcpSocket *socket_;
    QByteArray* complete_package_array_;
    qint32 size_complete_package_;
    Ui::Ic_mqtt_test_client *ui;

};
#endif // IC_MQTT_TEST_CLIENT_H
