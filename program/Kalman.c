
#include "stdbool.h"
#include "Kalman.h"
#include <math.h>

double X_last=0; //��һʱ�̵����Ž��
double X_mid=0;  //��ǰʱ�̵�Ԥ����
double X_now=0;  //��ǰʱ�̵����Ž��
double P_mid=0;  //��ǰʱ��Ԥ������Э����
double P_now=0;  //��ǰʱ�����Ž����Э����
double P_last=0; //��һʱ�����Ž����Э����
double kg=0;     //kalman����
double Kalman_Q=1.0;
double R=2;
extern bool Speedflag;


double KalmanFilter(double dat)
{
    X_mid = X_last;                //x(k|k-1) = AX(k-1|k-1)+BU(k)
    P_mid = P_last+Kalman_Q;       //p(k|k-1) = Ap(k-1|k-1)A'+Q
    kg = P_mid/(P_mid+R);          //kg(k) = p(k|k-1)H'/(Hp(k|k-1)'+R)
    X_now=X_mid+kg*(dat-X_mid);    //x(k|k) = X(k|k-1)+kg(k)(Z(k)-HX(k|k-1))
    P_now=(1.0f-kg)*P_mid;         //p(k|k) = (I-kg(k)H)P(k|k-1)
    P_last=P_now;                  //״̬����
    X_last=X_now;
    return X_now;
}
