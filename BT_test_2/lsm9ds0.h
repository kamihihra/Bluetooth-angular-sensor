#ifndef LSM9DS0_H
#define LSM9DS0_H

#include <QVector>
#include "math.h"
#define PI 3.1415926
#define ACCEL_2G 0.061 //mg/LSB
#define ACCEL_4G 0.122
#define ACCEL_6G 0.183
#define ACCEL_9G 0.244
#define ACCEL_16G 0.732
#define GYRO_245 8.75 //mdps/digit
#define GYRO_500 17.50
#define GYRO_2000 70
#define DT 0.01


class LSM9DS0
{
public:
    LSM9DS0();
    ~LSM9DS0();

    //Functions
    void Update_Data(QVector<int> data);
    void Cal_Angle();
    void Set_OffSetGyro(QVector<double> data);
    QVector<double> Get_Angles(){return Angles;}
    QVector<double> Get_AnglesAccel(){return AnglesAccel;}
    // Sensitivity for accel and gyro according to the current setting
    double SenAccel;
    double SenGyro;


private:

    QVector<int> RawDataAccel;
    QVector<int> RawDataGyro;

    QVector<double> OffSetGyro;

    QVector<double> Angles;
    QVector<double> AnglesAccel;
    QVector<double> AnglesGyro;

    // W:weight for the compliantary filter.
    int W;

    //calucurate offset of Gyro
    int sumx, sumy, sumz;
    int count;

};

#endif // LSM9DS0_H
