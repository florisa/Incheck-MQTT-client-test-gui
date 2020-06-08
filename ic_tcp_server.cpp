#include "ic_tcp_server.h"

Ic_tcp_server::Ic_tcp_server(QObject *parent)
    : QObject(parent)
{
    server_ = new QTcpServer(this);
    socket_ = nullptr;
    complete_package_array_ = new QByteArray();
    size_complete_package_ = 0;

    connect(server_, SIGNAL(newConnection()), this, SLOT(new_connection()));
}

Ic_tcp_server::~Ic_tcp_server()
{
    clean_up();
}

void Ic_tcp_server::clean_up()
{
    if (complete_package_array_)
    {
        delete complete_package_array_;
        complete_package_array_ = nullptr;
    }
}

void Ic_tcp_server::new_connection()
{
    if (socket_)
    {
        if (socket_->state() == QAbstractSocket::UnconnectedState)
        {
            socket_->close();
            socket_->deleteLater();
            socket_ = server_->nextPendingConnection();
            connect(socket_, SIGNAL(readyRead()), this, SLOT(my_ready_read()));
        }
    }
    else
    {
        socket_ = server_->nextPendingConnection();
        connect(socket_, SIGNAL(readyRead()), this, SLOT(my_ready_read()));
    }

}

void Ic_tcp_server::my_ready_read()
{

    int numBytesAvailable = socket_->bytesAvailable();
    QByteArray parcel;

    if (numBytesAvailable > 0)
    {
        parcel.clear();
        parcel = socket_->readAll();
        complete_package_array_->append(parcel);

        reloop:
        if ((size_complete_package_ == 0 && complete_package_array_->size() >= 4) ||
            (size_complete_package_ > 0 && complete_package_array_->size() >= size_complete_package_)) //While can process data, process it
        {
            if (size_complete_package_ == 0 && complete_package_array_->size() >= 4) //if size of data has received completely, then store it on our global variable
            {
                size_complete_package_ = array_to_int(complete_package_array_->mid(0, 4));
                complete_package_array_->remove(0, 4);
            }
            if (size_complete_package_ > 0 && complete_package_array_->size() >= size_complete_package_) // If data has received completely, then emit our SIGNAL with the data
            {
                QByteArray data = complete_package_array_->mid(0, size_complete_package_);
                complete_package_array_->remove(0, size_complete_package_);
                emit complete_package_received(data);
                size_complete_package_ = 0;
                if (complete_package_array_->size() > 0) goto reloop;

            }
        }
    }
}

void Ic_tcp_server::listen_to_port(qint16 port)
{
    server_->listen(QHostAddress::Any, port);
}

void Ic_tcp_server::send_out_package(QByteArray package)
{
    write_data(package);
}

bool Ic_tcp_server::write_data(QByteArray data)
{
    if (!socket_) return false;

    if (socket_->state() == QAbstractSocket::ConnectedState)
    {
        socket_->write(int_to_array(data.size())); //write size of data
        socket_->write(data); //write the data itself
        socket_->flush();
        return true;
    }
    else
    {
        return false;
    }
}

void Ic_tcp_server::close()
{
    if (server_)
    {
        server_->close();
        server_->deleteLater();
    }

    if (socket_)
    {
        socket_->disconnectFromHost();
        //Sleep(200);
        socket_->close();
        socket_->deleteLater();
    }
}

//--------------------------------------------------------------------
// HELPERS
//--------------------------------------------------------------------
QByteArray Ic_tcp_server::int_to_array(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

qint32 Ic_tcp_server::array_to_int(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}
