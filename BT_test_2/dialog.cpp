#include "dialog.h"
#include "ui_dialog.h"
//#include <QBluetoothLocalDevice>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothaddress.h>
#include <qbluetoothlocaldevice.h>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QFile>

QT_USE_NAMESPACE

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    /*
     * find default local adapter
    */

    localDevice = new QBluetoothLocalDevice;
    adapterAddress = localDevice->address();

    m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAddress);

    connect(m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
    connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished()));
    connect(m_discoveryAgent, SIGNAL(canceled()), this, SLOT(discoveryFinished()));


    //! [Create Chat Server
    server = new ChatServer(this);
    connect(server, SIGNAL(clientConnected(QString)), this, SLOT(clientConnected(QString)));
    connect(server, SIGNAL(clientDisconnected(QString)), this, SLOT(clientDisconnected(QString)));
    connect(server, SIGNAL(messageReceived(QString,QString)),
            this, SLOT(showMessage(QString,QString)));
    connect(this, SIGNAL(sendMessage(QString)), server, SLOT(sendMessage(QString)));
    server->startServer();
    //! [Create Chat Server]

    //! [Get local device name]
    localName = QBluetoothLocalDevice().name();
    //! [Get local device name]

    ui->leftLabel->setText("");
    ui->rightLabel->setText("");
    ui->window1->setText("");
    ui->window2->setText("");

    flag=0;
    Interval = new QTimer(this);
    connect(Interval, SIGNAL(timeout()), this, SLOT(angle()));
    Interval->start(10);
    ui->leftLabel->setText("21A6");
    ui->rightLabel->setText("21A7");
}

Dialog::~Dialog()
{
    delete ui;
    delete m_discoveryAgent;
    delete server;
}

void Dialog::on_ButtonSearch_clicked()
{
    qDebug() << "Search Button Clicked";
    startDiscovery();
}

void Dialog::startDiscovery()
{
    qDebug() << "Searching";
    ui->Statue->setText(tr("Scanning Available Devices"));
    if (m_discoveryAgent->isActive())
        m_discoveryAgent->stop();
    m_discoveryAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);
}

void Dialog::stopDiscovery()
{
    if (m_discoveryAgent){
        m_discoveryAgent->stop();
    }
}

void Dialog::serviceDiscovered(const QBluetoothServiceInfo &serviceInfo)
{
#if 0
    qDebug() << "Discovered service on"
             << serviceInfo.device().name() << serviceInfo.device().address().toString();
    qDebug() << "\tService name:" << serviceInfo.serviceName();
    qDebug() << "\tDescription:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
    qDebug() << "\tProvider:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceProvider).toString();
    qDebug() << "\tL2CAP protocol service multiplexer:"
             << serviceInfo.protocolServiceMultiplexer();
    qDebug() << "\tRFCOMM server channel:" << serviceInfo.serverChannel();
#endif
    //Found QBluetoothServiceInfo, display as a item in the list
    //    qDebug() << "Find";
    QMapIterator<QListWidgetItem *, QBluetoothServiceInfo> i(m_discoveredServices);
    while (i.hasNext()){
        i.next();
        if (serviceInfo.device().address() == i.value().device().address()){
            return;
        }
    }

    QString remoteName;
    if (serviceInfo.device().name().isEmpty())
        remoteName = serviceInfo.device().address().toString();
    else
        remoteName = serviceInfo.device().name();

    QListWidgetItem *item =
            new QListWidgetItem(QString::fromLatin1("%1 %2").arg(remoteName, serviceInfo.serviceName()));

    m_discoveredServices.insert(item, serviceInfo);
    ui->ListDevices->addItem(item);
}

void Dialog::discoveryFinished()
{
    qDebug() << "Disscovery Finished";
    ui->Statue->setText(tr("Scan Finished"));
}

void Dialog::on_ButtonClose_clicked()
{
    if (m_discoveryAgent){
        m_discoveryAgent->stop();
    }
    reject();
}

void Dialog::on_ListDevices_itemDoubleClicked(QListWidgetItem *item)
{
    m_service = m_discoveredServices.value(item);

    //Connect the client (m_service) to the server.
    qDebug() << "Connecting to" << m_service.serviceName()
             << "on" << m_service.device().name();

    // Create client
    qDebug() << "Going to create client";
    ChatClient *client = new ChatClient(this);
    qDebug() << "Connecting...";

    connect(client, SIGNAL(messageReceived(QString,QString)),
            this, SLOT(showMessage(QString,QString)));
    connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(client, SIGNAL(connected(QString)), this, SLOT(connected(QString)));
    connect(this, SIGNAL(sendMessage(QString)), client, SLOT(sendMessage(QString)));
    qDebug() << "Start client";
    client->startClient(m_service);
    clients.append(client);

    if(ui->radioButton_Left->isChecked())
    {
        DeviceLeftName = m_service.device().name();
    }
    else if (ui->radioButton_Right->isChecked())
    {
        DeviceRightName = m_service.device().name();
    }
}

QBluetoothServiceInfo Dialog::service() const
{
    return m_service;
}

void Dialog::clientDisconnected(const QString &name)
{
    if(QString::compare(DeviceLeftName, name, Qt::CaseInsensitive) == 0)
    {
        ui->radioButton_Left->setDisabled(false);
    }
    else if (QString::compare(DeviceRightName, name, Qt::CaseInsensitive) == 0)
    {
        ui->radioButton_Right->setDisabled(false);
    }
}

void Dialog::clientConnected(const QString &name)
{
    qDebug() << "connected" << name;
}

void Dialog::connected(const QString &name)
{
    // Disable the buttons after successful connection
    if(QString::compare(DeviceLeftName, name, Qt::CaseInsensitive) == 0)
    {
        ui->radioButton_Left->setDisabled(true);
        //ui->leftLabel->setText(DeviceLeftName);
    }
    else if (QString::compare(DeviceRightName, name, Qt::CaseInsensitive) == 0)
    {
        ui->radioButton_Right->setDisabled(true);
        //ui->rightLabel->setText(DeviceRightName);
    }
}

void Dialog::clientDisconnected()
{
    ChatClient *client = qobject_cast<ChatClient *>(sender());
    if (client) {
        clients.removeOne(client);
        client->deleteLater();
    }
}

void Dialog::showMessage(const QString &sender, const QString &message)
{
    //qDebug() << sender<<message;
    if(sender == "RNBT-21A7"){
        QStringList line_spilt = message.split(",");
        rowData_21A7= QVector<int>(6,0);
        for(int i=0; i<=5; i++){
            rowData_21A7[i]=(int16_t) line_spilt.at(i+2).toInt();
        }
        QString str1 = QString::number(rowData_21A7[2]); //check whether signal sent properly
        ui->window1->setText(str1);
    }else if(sender == "RNBT-21A6"){
        QStringList line_spilt = message.split(",");
        rowData_21A6 = QVector<int>(6,0);
        for(int i=0; i<=5; i++){
            rowData_21A6[i]=(int16_t) line_spilt.at(i+2).toInt();
        }
        QString str2 = QString::number(rowData_21A6[2]); //check whether signal sent properly
        ui->window2->setText(str2);
    }
    if(flag==2){

        qDebug() << RH.Get_AnglesAccel()<< LH.Get_Angles() << ":";
        Write();
    }
    if(flag==1){
        offSet();
    }
}

void Dialog::Write(){
    //QFile file("/home/lin/Data/data.txt");

    QFile file(fileInfo);

    if(file.open(QIODevice::WriteOnly| QIODevice::Append | QIODevice::Text)){
        QTextStream data(&file);

        data << LH.Get_Angles().at(0) <<",";
        data << LH.Get_Angles().at(1) <<",";
        data << LH.Get_Angles().at(2) <<",";
        data << LH.Get_AnglesAccel().at(0) <<",";
        data << LH.Get_AnglesAccel().at(1) <<",";
        data << LH.Get_AnglesAccel().at(2);
        data << "\r\n";

        file.flush();
        file.close();
        //qDebug() << "file wrote";
    }

}

void Dialog::offSet(){
    offGyro_21A6 = QVector<double>(3,0);
    offGyro_21A7 = QVector<double>(3,0);
    count++;
    //qDebug() << "21A6" << rowData_21A6[3] << ":" << rowData_21A6[4] << ":"  << rowData_21A6[5] << "21A7" << rowData_21A7[3] << ":" << rowData_21A7[4] << ":"  << rowData_21A7[5];
    offGyro_21A6[0] = rowData_21A6[3];
    offGyro_21A6[1] = rowData_21A6[4];
    offGyro_21A6[2] = rowData_21A6[5];
    LH.Set_OffSetGyro(offGyro_21A6);
    offGyro_21A7[0] = rowData_21A7[3];
    offGyro_21A7[1] = rowData_21A7[4];
    offGyro_21A7[2] = rowData_21A7[5];
    RH.Set_OffSetGyro(offGyro_21A7);
    qDebug() << "count: " << count;
    if(count>500){
        qDebug() << "stop";
        qDebug() << "21A6" << rowData_21A6[3] << ":" << rowData_21A6[4] << ":"  << rowData_21A6[5] << "21A7" << rowData_21A7[3] << ":" << rowData_21A7[4] << ":"  << rowData_21A7[5];
        flag=0;
    }

}
void Dialog::angle(){
    //timestamp = QTime::currentTime().toString("ss.zzz");
    //qDebug() << timestamp;
    LH.Update_Data(rowData_21A6);
    LH.Cal_Angle();
    RH.Update_Data(rowData_21A7);
    RH.Cal_Angle();
}


void Dialog::on_caliButton_clicked()
{
    qDebug() << "calibration";
    count=0;
    flag =1;
}

void Dialog::on_recordButton_clicked()
{    QString pass = "/home/lin/Data/";
     QString time = QTime::currentTime().toString("HH_mm_ss");
     QString date = QDate::currentDate().toString("yyyy_MM_dd");
     QString filename = date + "_" + time;
     QString extemsion = ".txt";
     fileInfo = pass + filename + extemsion;
    qDebug() << "Angles";
    flag =2;
}

void Dialog::on_acc0Button_clicked()
{
    QString command = "a=0";
    emit sendMessage(command);
    LH.SenAccel = ACCEL_2G;
    RH.SenAccel = ACCEL_2G;
}
void Dialog::on_acc1Button_clicked()
{
    QString command = "a=1";
    emit sendMessage(command);
    LH.SenAccel = ACCEL_4G;
    RH.SenAccel = ACCEL_4G;
}
void Dialog::on_acc2Button_clicked()
{
    QString command = "a=2";
    emit sendMessage(command);
    LH.SenAccel = ACCEL_6G;
    RH.SenAccel = ACCEL_6G;
}

void Dialog::on_gyro0Button_clicked()
{
    QString command = "g=0";
    emit sendMessage(command);
    LH.SenGyro = GYRO_245;
    RH.SenGyro = GYRO_245;
}
void Dialog::on_gyro1Button_clicked()
{
    QString command = "g=1";
    emit sendMessage(command);
    LH.SenGyro = GYRO_500;
    RH.SenGyro = GYRO_500;
}
void Dialog::on_gyro2Button_clicked()
{
    QString command = "g=2";
    emit sendMessage(command);
    LH.SenGyro = GYRO_2000;
    RH.SenGyro = GYRO_2000;
}


