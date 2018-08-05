/****************************************************************************
*	This file is used for the simulate test of ads-b system.
*/
//#include	<msgQLib.h>
#include	<math.h>
#include	<string.h>
#include	<fcntl.h>
#include	<stdlib.h>
//#include	<ioLib.h>
//#include	<tickLib.h>
//#include	<logLib.h>


#include	"cpr.h"

CPR	cprTest;

void	testCprFuncition()
{
	double	lons;
	double	lats;
	int	XZ;
	int	YZ;
	short	i;
	short	Nb;
	short	k;
	double	Rlon;
	double	Rlat;

	int	XZ0;
	int	YZ0;
	int	XZ1;
	int	YZ1;


	XZ0 = 0x17D91;
	YZ0 = 0x7E;
	XZ1 = 0xe99c;
	YZ1 = 0x1053;
	i = 1;
	cprTest.CPRDecoderGbl(&XZ0,&YZ0,&XZ1,&YZ1,i,&Rlon,&Rlat);

	return;
	/*cprTest.CPRDecoderLoc(double * lons, double * lats, int * XZ, int * YZ, short i, short Nb, short k, double * Rlon, double * Rlat);*/
	
}
