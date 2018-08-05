/****************************************************************************************/
/*********************************Implement of CPR.cpp************************************/
/******************************************************** *******************************/

/*
	Surface : be attention that the CPREncoderLoc() only use 17 bits to encode the lat/lon ,
			when decode, it needs special dispose... its Nb = 19 , k = 1 ;
*/
#include "stdio.h"
#include "math.h"

#include "CPR.h"

int CPR::floorx(double *x)
{
	int result;
	result = (int) *x ;
	if((*x < 0) && (*x != result)) result -- ;
	return result;
}

////////////////////////////////////////////////////////////////
// Function : Modulus(double *x,double *y,double *r,short flag)
// *x : input parameter ;
// *y : input parameter ;
// *r : output parameter , return the result ;
// flag : input parameter ; 
// flag = 1 , then *x & *y are angle ;
// flag = 0	, numerical case ;
///////////////////////////////////////////////////////////////
bool CPR::Modulus(double *xa,double *ya,double *r,short flag)
{
	double x,y,z ;
	x = *xa;
	y = *ya;
	if(!(y>0))
	{
		printf("Parameter *ya is invalid !\n");
		return false;
	}
	else if(x>=0)
	{
		z = x / y ;
		*r = x - y * floorx(&z);
	}
	else if(flag == 1)
	{
		x = x + 360.0 ; // angle case
		z = x / y ;
		*r = x - y * floorx(&z);
	}
	else 
	{
		z = x / y ; //mod(-3.5,1.7) = 1.6 ,fmod(-3.5,1.7)=-0.1
		*r = x - y * floorx(&z);	
	}
	return true;
}

int CPR::Modulus(int x,int y)
{
	int r;
	double z ;
	if( ! (y > 0) )
	{
		printf("Parameter y is invalid !\n");
		return false;
	}
	else
	{
#if 0
		r = x % y;
#else
		z = double(x) / y ;
		r = x - y * floorx(&z);
#endif
	}

	return r;
}

int CPR::NL(double *lat)
{
	int nl;
	double z = 0.0 ;
	if(*lat == 0)
		nl = 59;	//the equator
	else if((*lat == 87) || (*lat == -87))
		nl = 2;
	else if((*lat > 87) || (* lat < -87))
		nl = 1;
	else
	{
		z = (2 * $PI_CPR) / acos(1 - (1 - cos($PI_CPR / (2 * NZ))) / pow(cos($PI_CPR * fabs(*lat) / 180) , 2)) ;
		nl = floorx(&z);
	}
	return nl;
}

////////////////////////////////////////////////////////////////////////////////////////////
//	Function: CPR Encoder
//	lon/lat: input longitude/latitude to be encoded
//	i:	even=0,odd=1
//	Nb: airborne=17,surface=19,intent=14,TIS-B=12
//	k:	default=1,TIS-b(surface)=4
//	E_lon/E_lat:	output the encoded longitude/latitude
////////////////////////////////////////////////////////////////////////////////////////////
void CPR::CPREncoder(double *lon,double *lat,short i,short Nb,short k,int *E_lon,int *E_lat) 
{
	double Dlat , Rlat , Dlon ;
	double remainderlat , remainderlon ;
	int pow_2_Nb ,pow_2_17 ;/*pow_2_17  for surface */
	bool test;

	pow_2_Nb = (int)pow(2,Nb) ;
	pow_2_17 = (int)pow(2,17) ;

	Dlat = 360.0 / (4 * NZ - i) ;/*//a	// NZ=15,i=0,get Dlat=6;;;NZ=15,i=1, get Dlat=6.1016949152542370£»*/

	test = Modulus(lat,&Dlat,&remainderlat,1) ;/*//b	 angle*/

	double lat_1 ;
	lat_1 = k * pow(2 , Nb) * remainderlat / Dlat + 0.5 ;
	*E_lat = floorx(&lat_1) ;//obtain YZ  //f

	double lat_2;
	lat_2 = *lat / Dlat ;
	Rlat = Dlat * (*E_lat / pow(2,Nb) + floorx(&lat_2)) ; //c

	if((NL(&Rlat)-i) == 0)  //d
		Dlon = 360.0 ;
	else
		Dlon = 360.0 / (NL(&Rlat)-i) ;

	test = Modulus(lon,&Dlon,&remainderlon,1) ;   //e	angle

	double lon_1 ;
	lon_1 = k * pow_2_Nb * remainderlon / Dlon + 0.5 ;
	*E_lon = floorx(&lon_1) ;//obtain XZ
	if(Nb == 19 )
	{
		*E_lon = Modulus(*E_lon , pow_2_17) ; //f
		*E_lat = Modulus(*E_lat , pow_2_17) ;
	}
	else
	{
		*E_lon = Modulus(*E_lon, pow_2_Nb) ;
		*E_lat = Modulus(*E_lat , pow_2_Nb) ;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
//	Function:	Local Decoder
//	XZ/YZ : even or odd encoded lon / lat to be decoded;	
//	i : the most recently recieved encode type value;
//  Nb: aiborne=17; surface=19;intent=14;Tis-B=12;
//	k : default=1; TIS-B(Surface)=4;
//	*Rlon/*Rlat : output , the local decoded parameter;
///////////////////////////////////////////////////////////////////////////////////////////
void CPR::CPRDecoderLoc(double *lons,double *lats,int *XZ,int *YZ,short i,short Nb,short k,double *Rlon,double *Rlat)
{
	double Dlat , Dlon ;
	Dlat = 360.0 / k / (4 * NZ - i) ;//a
	if( Nb == 19 )
		Dlat = Dlat / 4 ;
	
	int j , nl , m ;
	double lat_0 , lat_1 , lon_0 , lon_1 ,remainderlat , remainderlon ;


	lat_0 = *lats / Dlat;
	Modulus(lats,&Dlat,&remainderlat,1);	//	angle

	if( Nb == 19 )
		lat_1 = remainderlat / Dlat - *YZ / pow(2,17) + 0.5 ;
	else
		lat_1 = remainderlat / Dlat - *YZ / pow(2,Nb) + 0.5 ;
	
	j = floorx(&lat_0) + floorx(&lat_1) ;//b
	if( Nb == 19 )
		*Rlat = Dlat * (j + *YZ / pow(2,17)) ;
	else
		*Rlat = Dlat * (j + *YZ / pow(2,Nb)) ;	//c		//obtain *Rlat
	
	nl = NL(Rlat) ;
	
	if(nl-i > 0)
		Dlon = 360.0 / k / (nl - i) ;	//d
	else if(nl-i == 0)
		Dlon = 360.0 / k ;
	if( Nb == 19 )
		Dlon = Dlon / 4 ;
	
	lon_0 = *lons / Dlon ;
	Modulus(lons,&Dlon,&remainderlon,1) ;	//	angle
	
	if( Nb == 19 )
		lon_1 = remainderlon / Dlon - *XZ / pow(2,17) + 0.5 ;
	else
		lon_1 = remainderlon / Dlon - *XZ / pow(2,Nb) + 0.5 ;
	
	m = floorx(&lon_0) + floorx(&lon_1) ;//e

	if( Nb == 19 )
		*Rlon = Dlon*(m + *XZ / pow(2,17)) ;
	else	
		*Rlon = Dlon*(m + *XZ / pow(2,Nb)) ;	//f		//obtain *Rlon
		
}

///////////////////////////////////////////////////////////////////////////////////////////////
//	Function:	Global Decoder
//	XZ0/YZ0 : even encoded lon / lat to be decoded;
//	XZ1/YZ1 : odd encoded lon / lat to be decoded;	
//	i : the most recently recieved encode type value;
//  Nb: aiborne=17; surface=19;intent=14;Tis-B=12;
//	k : default=1; TIS-B(Surface)=4;
//	*Rlon/*Rlat : output , the global decoded parameter; 
//  if return *Rlon=*Rlon=9999 , then the two input latitude coordinates straddle a transition;
///////////////////////////////////////////////////////////////////////////////////////////////
void CPR::CPRDecoderGbl(int *XZ0,int *YZ0,int *XZ1,int *YZ1,short i,double *Rlon,double *Rlat)
{
	double Dlat0 , Dlat1 , Rlat0 , Rlat1 , Dlon ;
	int j , nl0 , nl1 , m ;
	double j_index , m_index ;

	Dlat0 = 360.0 / ( 4 * NZ) ;//a  Dlat0=6
	Dlat1 = 360.0 / (4 * NZ-1) ;// Dlat1=6.1016949152542370£»

	j_index = (*YZ0 * 59 - *YZ1 * 60) / pow(2,17) + 0.5 ; 
	j = floorx(&j_index);//b
	
	Rlat0 = Dlat0 * (Modulus(j,60) + *YZ0 / pow(2,17));//c  //this place using % ,ok ?
	Rlat1 = Dlat1 * (Modulus(j,59) + *YZ1 / pow(2,17));

	if((Rlat0 >= 270 )&& (Rlat0 <= 360)) Rlat0 = Rlat0 - 360.0 ;
	if((Rlat1 >= 270 )&& (Rlat1 <= 360)) Rlat1 = Rlat1 - 360.0 ;

	nl0 = NL(&Rlat0) ;
	nl1 = NL(&Rlat1) ;
	if(nl0 != nl1)//d
	{
		*Rlon = 9999;
		*Rlat = 9999;
		return;
	}
	else if(nl0-i > 1)
	{
		Dlon = 360.0 / (nl0-i);//e

		m_index = ( *XZ0 * (nl0-1) - *XZ1 * nl0 ) / pow(2,17) + 0.5 ;
		m = floorx(&m_index);//f
		if(i == 0)
		{	
			//according to the most recently recieved even/odd flat , store Rlon,Rlat;
			*Rlon = Dlon * (Modulus(m,(nl0-i)) + *XZ0 / pow(2,17));//g	
			*Rlat = Rlat0;
		}
		if(i == 1)
		{
			*Rlon = Dlon * (Modulus(m,(nl0-i)) + *XZ1 / pow(2,17));
			*Rlat = Rlat1;
		}
	}
	else 
	{
		// here ni=1 
		Dlon = 360.0 ;//e
		//m=floor((*XZ0*(nl0-1)-*XZ1*nl0)/pow(2,17)+1/2);   //f  
		//	m is a integer £¬ni = 1 , Mod(m,ni)=0 ; so we can easily get the value of Rlon ;
		*Rlon = Dlon * ( *XZ0 / pow(2,17)); //g
		if(i == 0) *Rlat = Rlat0 ;	//	according to the most recently recieved type flag i, 
		if(i == 1) *Rlat = Rlat1 ;	//	store Rlat
	}
}


CPR::CPR()
{
	
}
CPR::~CPR()
{
	
}
