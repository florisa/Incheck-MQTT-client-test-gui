#include "ic_mqtt_test_client.h"
#include "ui_ic_mqtt_test_client.h"

Ic_mqtt_test_client::Ic_mqtt_test_client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Ic_mqtt_test_client)
{
    ui->setupUi(this);
}

Ic_mqtt_test_client::~Ic_mqtt_test_client()
{
    delete ui;
}


void Ic_mqtt_test_client::on_btn_connect_gateway_clicked()
{
    gateway_confirmation=false;
    complete_package_array_ = new QByteArray();
    size_complete_package_ = 0;
    socket_ = new QTcpSocket(this);
    //socket_->connectToHost("localhost",4321);
    socket_->connectToHost(ui->lineEdit_address->text(),ui->spinBox_port->value());
    connect(socket_,SIGNAL(readyRead()), this, SLOT(my_ready_read()));
    connect(this,SIGNAL(complete_package_received(QByteArray)), this, SLOT(process_tcp_package(QByteArray)));
}


void Ic_mqtt_test_client::on_btn_send_handshake_clicked()
{

    send_out_handshake_package();
}


void Ic_mqtt_test_client::my_ready_read()
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

void Ic_mqtt_test_client::process_tcp_package(QByteArray package)
{
    // Get Message Type:
    QDataStream ds(&package, QIODevice::ReadOnly);

    QString messageType;
    ds >> messageType;

    //Handshake
    if (messageType == "RETURN_HANDSHAKE")
    {
        gateway_confirmation = true;
        send_out_double_value();
        return;
    }

}

void Ic_mqtt_test_client::send_out_handshake_package()
{

    // Generate Package:
    QByteArray package;
    QDataStream ds(&package, QIODevice::WriteOnly);

    // Message Type:
    QString packageType = "FIRST_HANDSHAKE";
    ds << packageType;

    write_data(package);

}

void Ic_mqtt_test_client::send_out_double_value()
{

    if(gateway_confirmation)
    {
        // Generate Package:
        QByteArray double_value_package;
        QDataStream ds(&double_value_package, QIODevice::WriteOnly);

        // Message Type:
        QString packageType    = "PUBLISH_DV";
        QString parameterName  = "STM/DIAMETER";
        QString parameterValue = "3.8";
        ds << packageType;
        ds << parameterName;
        ds << parameterValue;

        write_data(double_value_package);
    }
}

bool Ic_mqtt_test_client::write_data(QByteArray data)
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

void Ic_mqtt_test_client::close()
{
    if (socket_)
    {
        socket_->close();
        socket_->deleteLater();
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
QByteArray Ic_mqtt_test_client::int_to_array(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

qint32 Ic_mqtt_test_client::array_to_int(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}
