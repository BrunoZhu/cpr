
/******************************************* CPR.h *************************************************/

#ifndef _COMPACT_POSITION_REPORT_H_
#define _COMPACT_POSITION_REPORT_H_


#define	NZ	15
#define	$PI_CPR	3.14159265358979

class CPR
{

	/*	全球解码返回值，longitude : [0 , 360] , 算法没有要求矫正，
		当测试经度小于 0 度时 ，返回经度与其相差360度*/
	
	private:
		int floorx(double *x);
		bool Modulus(double *xa,double *ya,double *r,short flag);
		int Modulus(int x,int y);
		int NL(double *lat);	/* return the "number of longitude zones" */
	public:
		CPR();
		~CPR();
		void CPREncoder(double *lon,double *lat,short i,short Nb,short k,int *E_lon,int *E_lat);		
		void CPRDecoderLoc(double *lons,double *lats,int *XZ,int *YZ,short i,short Nb,short k,double *Rlon,double *Rlat);
		void CPRDecoderGbl(int *XZ0,int *YZ0,int *XZ1,int *YZ1,short i,double *Rlon,double *Rlat);
};

#endif
