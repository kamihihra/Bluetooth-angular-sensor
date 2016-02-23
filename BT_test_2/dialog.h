#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>

#include <qbluetoothuuid.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothservicediscoveryagent.h>
#include <qbluetoothsocket.h>
#include <qbluetoothhostinfo.h>
#include <QBluetoothLocalDevice>

#include "chatserver.h"
#include "chatclient.h"
#include "lsm9ds0.h"
#define ACCEL_2G 0.061 //mg/LSB
#define ACCEL_4G 0.122
#define ACCEL_6G 0.183
#define ACCEL_9G 0.244
#define ACCEL_16G 0.732
#define GYRO_245 8.75 //mdps/digit
#define GYRO_500 17.50
#define GYRO_2000 70

QT_FORWARD_DECLARE_CLASS(QModelIndex)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)

QT_USE_NAMESPACE

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void startDiscovery();
    void stopDiscovery();

    QBluetoothServiceInfo service() const;

signals:
    void sendMessage(const QString &message);

private slots:
    void on_ButtonSearch_clicked();
    void on_ButtonClose_clicked();

    void serviceDiscovered(const QBluetoothServiceInfo &serviceInfo);
    void discoveryFinished();

    void on_ListDevices_itemDoubleClicked(QListWidgetItem *item);

    void clientConnected(const QString &name);
    void clientDisconnected(const QString &name);
    void clientDisconnected();
    void connected(const QString &name);

    void showMessage(const QString &sender, const QString &message);

    void angle();

    void Write();

    void on_caliButton_clicked();

    void on_acc0Button_clicked();
    void on_acc1Button_clicked();
    void on_acc2Button_clicked();

    void on_gyro0Button_clicked();
    void on_gyro1Button_clicked();
    void on_gyro2Button_clicked();

    void on_recordButton_clicked();

private:
    Ui::Dialog *ui;


    ChatServer *server;
    QList<ChatClient *> clients;
    QBluetoothLocalDevice *localDevice;
    QBluetoothAddress adapterAddress;

    QString localName;

    QBluetoothServiceDiscoveryAgent *m_discoveryAgent;
    QBluetoothServiceInfo m_service;
    QMap<QListWidgetItem *, QBluetoothServiceInfo> m_discoveredServices;

    QString DeviceLeftName;
    QString DeviceRightName;

    LSM9DS0 RH;
    LSM9DS0 LH;

    QVector<double> offGyro_21A6;
    QVector<double> offGyro_21A7;
    QVector<int> rowData_21A6;
    QVector<int> rowData_21A7;
    int flag;
    int count;
    QString timestamp;
    QString fileInfo;
    void offSet();

    QTimer *Interval;
};

#endif // DIALOG_H
