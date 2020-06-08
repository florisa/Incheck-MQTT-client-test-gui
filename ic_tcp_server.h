#ifndef IC_TCP_SERVER_H
#define IC_TCP_SERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>

class Ic_tcp_server:public QObject
{
    Q_OBJECT
public:
    Ic_tcp_server(QObject *parent);
    ~Ic_tcp_server();

    void clean_up();
    void listen_to_port(qint16 port);
    bool write_data(QByteArray data);
    QByteArray int_to_array(qint32 source);
    qint32 array_to_int(QByteArray source);
    void close();

signals:
    void complete_package_received(QByteArray);

public slots:
    void new_connection();
    void my_ready_read();
    void send_out_package(QByteArray);

private:
    QTcpServer* server_;
    QTcpSocket* socket_;
    QByteArray* complete_package_array_;
    qint32 size_complete_package_;
};

#endif // IC_TCP_SERVER_H
