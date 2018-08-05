#include <QCoreApplication>
#include "CPR.h"

CPR	cprTest;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    double srclon = 117;
    double srclat = 31;
    int lon,lat = 0;
    cprTest.CPREncoder(&srclon,&srclat,0,17,1,&lon,&lat);
    return a.exec();
}
