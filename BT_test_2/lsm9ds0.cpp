#include "lsm9ds0.h"
#include <QDebug>

LSM9DS0::LSM9DS0()
{
    //initate vector with constant size
    RawDataAccel = QVector<int>(3);
    RawDataGyro  = QVector<int>(3);
    Angles       = QVector<double>(3, 0);
    AnglesAccel  = QVector<double>(3, 0);
    AnglesGyro   = QVector<double>(3, 0);

    //initiate Accel and Gyro settings
    //default
    SenAccel = ACCEL_2G;
    SenGyro  = GYRO_245;

    //Gyro OFFSET -> need to have calibration procedure here.
    //temporary
    OffSetGyro = QVector<double>(3, 0);
    count=0;
    W = 20;

}

LSM9DS0::~LSM9DS0()
{

}

//Update rawdata from Bluetooth module.
void LSM9DS0::Update_Data(QVector<int> data)
{
    QVector<int>::iterator iter_RawAcc;
    QVector<int>::iterator iter_RawGyro;
    iter_RawAcc  = RawDataAccel.begin();
    iter_RawGyro = RawDataGyro.begin();
    for(QVector<int>::iterator iter_data = data.begin(); iter_data != data.end(); ++iter_data)
    {
        if(iter_RawAcc != RawDataAccel.end())
        {
            *iter_RawAcc = *iter_data;
            iter_RawAcc++;
        }
        else if (iter_RawGyro != RawDataGyro.end())
        {
            *iter_RawGyro = *iter_data;
            iter_RawGyro++;
        }
    }
    //qDebug() << SenGyro;
}

//Calculate angle
void LSM9DS0::Cal_Angle()
{
    //conver accel to g, gyro to deg/s
    QVector<double> Accel, Gyro;

    for(QVector<int>::iterator iter_Acc = RawDataAccel.begin(); iter_Acc != RawDataAccel.end(); ++iter_Acc)
    {
        Accel.push_back(*iter_Acc * SenAccel / 1000);
    }

    QVector<double>::iterator iter_OffG = OffSetGyro.begin();
    for(QVector<int>::iterator iter_Gyro = RawDataGyro.begin(); iter_Gyro != RawDataGyro.end(); ++iter_Gyro)
    {
        Gyro.push_back((*iter_Gyro - *iter_OffG) * SenGyro / 1000);
        ++iter_OffG;
    }
    Accel[2] = -1 * Accel.at(2);

    // axis x -> 0 ,axis y -> 1, axis z -> 2

    //    double root_x = sqrt(Accel.at(1)*Accel.at(1)+Accel.at(2)*Accel.at(2));
    //    double root_y = sqrt(Accel.at(0)*Accel.at(0)+Accel.at(2)*Accel.at(2));
    //    double root_z = sqrt(Accel.at(0)*Accel.at(0)+Accel.at(1)*Accel.at(1));

    //    AnglesAccel[0] = atan2(Accel.at(0), root_x) / PI * 180;
    //    AnglesAccel[1] = atan2(Accel.at(1), root_y) / PI * 180;
    //    AnglesAccel[2] = atan2(Accel.at(2), root_z) / PI * 180;


    AnglesAccel[0] = atan2(Accel.at(1), Accel.at(2)) / PI * 180;
    AnglesAccel[1] = atan2(Accel.at(0), Accel.at(2)) / PI * 180;
    AnglesAccel[2] = atan2(Accel.at(0), Accel.at(1)) / PI * 180;

    AnglesGyro[0] = Gyro.at(0)*DT + Angles.at(0);
    AnglesGyro[1] = Gyro.at(1)*DT + Angles.at(1);
    AnglesGyro[2] = Gyro.at(2)*DT + Angles.at(2);

    Angles[0] = (W*AnglesGyro.at(0) + AnglesAccel.at(0))/(1+W);
    Angles[1] = (W*AnglesGyro.at(1) + AnglesAccel.at(1))/(1+W);
    Angles[2] = (W*AnglesGyro.at(2) + AnglesAccel.at(2))/(1+W);

    //qDebug() << Angles << OffSetGyro;
}


void LSM9DS0::Set_OffSetGyro(QVector<double> data)
{
    count++;
    QVector<double>::iterator iter_OffG = OffSetGyro.begin();
    for(QVector<double>::iterator iter = data.begin(); iter != data.end(); ++iter)
    {
        if(iter_OffG != OffSetGyro.end())
        {
            *iter_OffG += *iter/500;
            ++iter_OffG;
        }
    }
    //qDebug() << OffSetGyro;
}
