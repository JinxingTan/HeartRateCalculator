

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>


#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "CalculateHeartRate.h"


#define NONE		 "\033[m" 
#define RED 		 "\033[0;32;31m" 
#define LIGHT_RED	 "\033[1;31m" 
#define GREEN		 "\033[0;32;32m" 
#define LIGHT_GREEN  "\033[1;32m" 
#define BLUE		 "\033[0;32;34m" 
#define LIGHT_BLUE	 "\033[1;34m" 
#define DARY_GRAY	 "\033[1;30m" 
#define CYAN		 "\033[0;36m" 
#define LIGHT_CYAN	 "\033[1;36m" 
#define PURPLE		 "\033[0;35m" 
#define LIGHT_PURPLE "\033[1;35m" 
#define BROWN		 "\033[0;33m" 
#define YELLOW		 "\033[1;33m" 
#define LIGHT_GRAY	 "\033[0;37m" 
#define WHITE		 "\033[1;37m"


//#define dNoGSensorData

int pXImgPoint[1024]={0};
int tXPointCnt=0;

void cal_heart(int *pInputHrpData, int tInputLength)
{
#if 1
printf("damon ====> cal heart !\n");

	// find first spie
	int i=0;
	char tTempUp=0, tTempDown=0, tTempFalseValue=0, tTempFindFirstLow=0;
	HeartValue pTempMinHeartVal[7];
	HeartValue pTempMaxHeartVal[7];
	int tTempMinIdx=0, tTempMaxIdx=0;
	int tTempPrevVal;
	int tTempCurVal=0;
	tTempPrevVal= pInputHrpData[0];
	unsigned int tTempTotalHeartValue=0;
	int tTempTotalHeartCnt=0;
	for(i=1; i<tInputLength; i++)
	{
		tTempCurVal= pInputHrpData[i];

	//	tTempTotalHeartValue += tTempCurVal;
	//	tTempTotalHeartCnt++;
		
		if(tTempCurVal>tTempPrevVal)
		{
			if(tTempUp==0 && tTempDown==0)
			{
				pTempMinHeartVal[tTempMinIdx].index=i;
				pTempMinHeartVal[tTempMinIdx].value=tTempPrevVal;
				tTempMinIdx++;
				tTempPrevVal=tTempCurVal;
				tTempUp=1;
			}else if(tTempUp==1)
			{
				if(tTempFalseValue>0)
				{
					if(tTempMaxIdx<=0)
						printf("damon ===> max idx error : %d \n", tTempMaxIdx);
				
					if( tTempCurVal>pTempMaxHeartVal[tTempMaxIdx-1].value /*|| (i-pTempMaxHeartVal[tTempMaxIdx-1].index>8)*/)
					{
						if(i-pTempMaxHeartVal[tTempMaxIdx-1].index>8)
							printf("damon ===> up adjuest value max val =  %d %d -%d %d \n", i, tTempCurVal, pTempMaxHeartVal[tTempMaxIdx-1].index, pTempMaxHeartVal[tTempMaxIdx-1].value);
					
						tTempUp=1;
						tTempDown=0;
						tTempMaxIdx--;
					
						tTempFalseValue=0;
					}
				}
			
				tTempPrevVal=tTempCurVal;
			}else if(tTempDown==1)
			{
				if(tTempFalseValue==0)
				{
					tTempFalseValue=1;

					if(tTempMinIdx<7)
					{

				#if 1
						// filter  max value
						if(tTempMinIdx>=2)
						{
							int m=0, n=0;
							for(m=0; m<tTempMinIdx-1; m++)
							{
								if(pTempMinHeartVal[m].value>45000)
								{
									for(n=m; n<tTempMinIdx-1; n++)
									{
										pTempMinHeartVal[n].index=pTempMinHeartVal[n+1].index;
										pTempMinHeartVal[n].value=pTempMinHeartVal[n+1].value;
									}

									m--;
									tTempMinIdx--;
								}else
								{
									if(/*m>0 &&*/ m<tTempMinIdx-2)
									{
										if(pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index>100)
										{//printf("damon ===> error delete max value %d : %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index);
											for(n=0; n<m+2; n++)
											{
												if(n+m+1<tTempMinIdx)
												{
													pTempMinHeartVal[n]=pTempMinHeartVal[n+m+2];
												}
											}
											
											tTempMinIdx=tTempMinIdx-(m+1);
											m=0;
										//	printf("damon ===> value : %d \n", tTempMaxIdx);
										}else
										if(((pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index<20) || (pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index>80)) && 
											(pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index>20))
										{
									if(pTempMinHeartVal[m].index>1932 && pTempMinHeartVal[m].index<1994)
										printf("damon xxxxxxx> delete value %d : %d %d %d \n", tTempMinIdx, pTempMinHeartVal[m].index, pTempMinHeartVal[m+1].index, pTempMinHeartVal[m+2].index);

											for(n=m; n<tTempMinIdx-1; n++)
											{
												pTempMinHeartVal[n].index=pTempMinHeartVal[n+1].index;
												pTempMinHeartVal[n].value=pTempMinHeartVal[n+1].value;
											}
											
											m--;
											tTempMinIdx--;
										}else if((pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index>20) &&
											((pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index<20) || (pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index>80)))
										{
								//		printf("damon ==> error delete value %d : %d %d %d \n", tTempMinIdx, pTempMinHeartVal[m].index, pTempMinHeartVal[m+1].index, pTempMinHeartVal[m+2].index);
											for(n=m+2; n<tTempMinIdx-1; n++)
											{
												pTempMinHeartVal[n].index=pTempMinHeartVal[n+1].index;
												pTempMinHeartVal[n].value=pTempMinHeartVal[n+1].value;
											}
											
											m+=2;
											tTempMinIdx--;											
										}else if(((pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index<20) || (pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index>80)) &&
											((pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index<20) || (pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index>80)))
										{
											for(n=m; n<tTempMinIdx-2; n++)
											{
												pTempMinHeartVal[n].index=pTempMinHeartVal[n+2].index;
												pTempMinHeartVal[n].value=pTempMinHeartVal[n+2].value;
											}
											
											m--;
											tTempMinIdx-=2; 										
										}
									}
								}
							}							
						}
				#endif

						if(tTempMinIdx<0)
							printf("damon ===> error min idx : %d \n", tTempMinIdx);
					
						pTempMinHeartVal[tTempMinIdx].index=i;
						pTempMinHeartVal[tTempMinIdx].value=tTempPrevVal;
						tTempMinIdx++;
					}
				}else if((i-pTempMinHeartVal[tTempMinIdx-1].index>8) && (tTempCurVal>pTempMinHeartVal[tTempMinIdx-1].value))
				{
					tTempDown=0;
					tTempUp=1;
					tTempFalseValue=0;

		/*	{	// filter average value
				if(tTempTotalHeartCnt>0)
				{
					int m=0, n=0;
					int tTempAvgVal=tTempTotalHeartValue/tTempTotalHeartCnt;
					for(m=0; m<tTempMinIdx; m++)
					{
						if(pTempMinHeartVal[m].value>=tTempAvgVal-10)
						{
							for(n=m; n<tTempMinIdx-1; n++)
							{
								pTempMinHeartVal[n]=pTempMinHeartVal[n+1];
							}
							tTempMinIdx--;
						}
					}
				}
				tTempTotalHeartCnt=0;
				tTempTotalHeartValue=0;
			}*/
			
					if(tTempMinIdx>=7)
					{
						printf("damon ===> min : (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d) (%d, %d) (%d, %d)\n", 
							pTempMinHeartVal[0].index, pTempMinHeartVal[0].value,
							pTempMinHeartVal[1].index, pTempMinHeartVal[1].value,
							pTempMinHeartVal[2].index, pTempMinHeartVal[2].value,
							pTempMinHeartVal[3].index, pTempMinHeartVal[3].value,
							pTempMinHeartVal[4].index, pTempMinHeartVal[4].value,
							pTempMinHeartVal[5].index, pTempMinHeartVal[5].value,
							pTempMinHeartVal[6].index, pTempMinHeartVal[6].value);


						int m=0;
						int tTempPointCount=pTempMinHeartVal[5].index-pTempMinHeartVal[0].index;
						int tTempMaxPoint=0, tTempMinPoint=1000;
						for(m=0; m<5; m++)
						{
							int tTempSubIdx=pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index;
							if(tTempSubIdx>tTempMaxPoint)
								tTempMaxPoint=tTempSubIdx;
							if(tTempSubIdx<tTempMinPoint)
								tTempMinPoint=tTempSubIdx;
						}
						// delete max and min value 
						tTempPointCount -= (tTempMaxPoint+tTempMinPoint);
						int tTempRate=(60*1000)/((tTempPointCount*20)/3);
						for(m=0; m<5; m++)
						{

							if(m+5<10)
							{
								pTempMinHeartVal[m].index=pTempMinHeartVal[m+5].index;
								pTempMinHeartVal[m].value=pTempMinHeartVal[m+5].value;

								pTempMinHeartVal[m+5].index=0;
								pTempMinHeartVal[m+5].value=0;
							}
						}
						tTempMinIdx-=5;

						printf("damon ===> heart rate : %d \n", tTempRate);

					}
				}

				tTempPrevVal=tTempCurVal;
			}

		}else if(tTempCurVal < tTempPrevVal)
		{
			if(tTempUp==0 && tTempDown==0)
			{//printf("damon ===> start max idx : %d %d %d \n", tTempMaxIdx, i, tTempPrevVal);
				pTempMaxHeartVal[tTempMaxIdx].index=i;
				pTempMaxHeartVal[tTempMaxIdx].value=tTempPrevVal;
				tTempMaxIdx++;
				tTempPrevVal=tTempCurVal;
				tTempDown=1;
			}else if(tTempDown==1)
			{
				if(tTempFalseValue>0)
				{
					if(tTempMinIdx<=0)
						printf("damon ===> min idx error : %d \n", tTempMinIdx);
				
					if(tTempCurVal<pTempMinHeartVal[tTempMinIdx-1].value /*|| (i-pTempMinHeartVal[tTempMinIdx-1].index>8)*/)
					{
						if(i-pTempMinHeartVal[tTempMinIdx-1].index>8)
							printf("damon ===> down adjuest value min value = %d %d \n", pTempMinHeartVal[tTempMinIdx-1].index, pTempMinHeartVal[tTempMinIdx-1].value);
					
						tTempUp=0;
						tTempDown=1;
						tTempMinIdx--;
					
						tTempFalseValue=0;
					}					
				}
				
				tTempPrevVal=tTempCurVal;
			}else if(tTempUp==1)
			{
				if(tTempFalseValue==0)
				{
					tTempFalseValue=1;
			//	printf("damon ===> max idx : %d \n", tTempMaxIdx);
					if(tTempMaxIdx<7)
					{
			//		printf("damon ===> max idx = %d \n", tTempMaxIdx);
						// filter value
						if(tTempMaxIdx>=2)
						{
							int m=0, n=0;
							for(m=0; m<tTempMaxIdx-1; m++)
							{
								if(pTempMaxHeartVal[m].value<15000)
								{
									for(n=m; n<tTempMaxIdx-1; n++)
									{
										pTempMaxHeartVal[n].index=pTempMaxHeartVal[n+1].index;
										pTempMaxHeartVal[n].value=pTempMaxHeartVal[n+1].value;
									}

									m--;
									tTempMaxIdx--;
								}else
								{

									if(/*m>0 &&*/ m<tTempMaxIdx-2)
									{
									
										if(pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index>100)
										{//printf("damon ===> error delete max value %d : %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index);
											for(n=0; n<m+2; n++)
											{
												if(n+m+1<tTempMaxIdx)
												{
													pTempMaxHeartVal[n]=pTempMaxHeartVal[n+m+2];
												}
											}
											tTempMaxIdx=tTempMaxIdx-(m+1);
											m=0;
										//	printf("damon ===> value : %d \n", tTempMaxIdx);
										}else
										if(((pTempMaxHeartVal[m+1].index-pTempMaxHeartVal[m].index<20) || (pTempMaxHeartVal[m+1].index-pTempMaxHeartVal[m].index>80))&& 
											(pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index>20))
										{										
								//		printf("damon ==> delete max value %d : %d %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index, pTempMaxHeartVal[m+2].index);
											for(n=m; n<tTempMaxIdx-1; n++)
											{
												pTempMaxHeartVal[n].index=pTempMaxHeartVal[n+1].index;
												pTempMaxHeartVal[n].value=pTempMaxHeartVal[n+1].value;
											}
											
											m--;
											tTempMaxIdx--;
										}else if((pTempMaxHeartVal[m+1].index-pTempMaxHeartVal[m].index>20) &&
											((pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index<20) || (pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index>80)))
										{
											printf("damon ==> 1 delete max value %d : %d %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index, pTempMaxHeartVal[m+2].index);
											for(n=m+2; n<tTempMaxIdx-1; n++)
											{
												pTempMaxHeartVal[n].index=pTempMaxHeartVal[n+1].index;
												pTempMaxHeartVal[n].value=pTempMaxHeartVal[n+1].value;
											}
											
											m+=2;
											tTempMaxIdx--;
										}else if(((pTempMaxHeartVal[m+1].index-pTempMaxHeartVal[m].index<20) || (pTempMaxHeartVal[m+1].index-pTempMaxHeartVal[m].index>80) )&& 
											((pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index<20) || (pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index>80)))
										{
								//			printf("damon ==> 2 delete max value %d : %d %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index, pTempMaxHeartVal[m+2].index);										
											for(n=m; n<tTempMaxIdx-2; n++)
											{
												pTempMaxHeartVal[n].index=pTempMaxHeartVal[n+2].index;
												pTempMaxHeartVal[n].value=pTempMaxHeartVal[n+2].value;
											}
											
											m--;
											tTempMaxIdx-=2;
										}
									}
								}
							}							
						}

						if(tTempMaxIdx<0)
							printf("damon ===> error max idx : %d \n", tTempMaxIdx);
					
						pTempMaxHeartVal[tTempMaxIdx].index=i;
						pTempMaxHeartVal[tTempMaxIdx].value=tTempPrevVal;
						tTempMaxIdx++;		
				//	printf("damon ===> max idx = %d \n", tTempMaxIdx);
					}
				}else if((i-pTempMaxHeartVal[tTempMaxIdx-1].index>8) && (tTempCurVal<pTempMaxHeartVal[tTempMaxIdx-1].value))
				{
					tTempUp=0;
					tTempDown=1;
					tTempFalseValue=0;


			/*	{	// filter average value
					if(tTempTotalHeartCnt>0)
					{
						int m=0, n=0;
						int tTempAvgVal=tTempTotalHeartValue/tTempTotalHeartCnt;
						for(m=0; m<tTempMaxIdx; m++)
						{
							if(pTempMaxHeartVal[m].value<tTempAvgVal+10)
							{
								for(n=m; n<tTempMaxIdx-1; n++)
								{
									pTempMaxHeartVal[n]=pTempMaxHeartVal[n+1];
								}
								tTempMaxIdx--;
							}
						}
					}
					tTempTotalHeartCnt=0;
					tTempTotalHeartValue=0;
				}*/


				//	printf("damon ===> cal max val : %d \n", tTempMaxIdx);
					if(tTempMaxIdx>=7)
					{
						printf("damon ===> max : (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d) \n", 
							pTempMaxHeartVal[0].index, pTempMaxHeartVal[0].value,
							pTempMaxHeartVal[1].index, pTempMaxHeartVal[1].value,
							pTempMaxHeartVal[2].index, pTempMaxHeartVal[2].value,
							pTempMaxHeartVal[3].index, pTempMaxHeartVal[3].value,
							pTempMaxHeartVal[4].index, pTempMaxHeartVal[4].value);

						int m=0;
						for(m=0; m<5; m++)
						{
							if(m+5<10)
							{
								pTempMaxHeartVal[m].index=pTempMaxHeartVal[m+5].index;
								pTempMaxHeartVal[m].value=pTempMaxHeartVal[m+5].value;

								pTempMaxHeartVal[m+5].index=0;
								pTempMaxHeartVal[m+5].value=0;
							}
						}
						tTempMaxIdx-=5;
					}
				}

				tTempPrevVal=tTempCurVal;
			}
		}

	}
#endif

}


int* draw_gsensor_data(int *pInputData, int tInputLength)
{
	int i=0, j=0;
	int *pTempOutData=malloc(tInputLength*sizeof(int));
	memset(pTempOutData, 0, tInputLength*sizeof(int));

	int pTempValue[5]={0};
	
	// 中值滤波
	for(i=0; i<tInputLength-5; i++)
	{
		for(j=0; j<5; j++)
		{
			pTempValue[j]=pInputData[i+j];
		}

	//	printf("damon ===> start : %d %d %d %d %d \n", pTempValue[0], pTempValue[1], pTempValue[2], pTempValue[3], pTempValue[4]);
		int m=0, n=0;
		for(m=0; m<4; m++)
		{
			for(n=m+1; n<5; n++)
			{
				if(pTempValue[m]>pTempValue[n])
				{
					int tTempSwap=pTempValue[m];
					pTempValue[m]=pTempValue[n];
					pTempValue[n]=tTempSwap;
				}
			}	
		}

	
	//	printf("damon ===> end : %d %d %d %d %d \n", pTempValue[0], pTempValue[1], pTempValue[2], pTempValue[3], pTempValue[4]);
		pTempOutData[i]=800-pTempValue[2];
	//	sleep(1);
	}


	// 均值滤波
	int tTempTotalVal=0;
	for(i=0; i<tInputLength-5; i++)
	{
		tTempTotalVal=0;
		for(j=0; j<5; j++)
		{
			tTempTotalVal += pTempOutData[i+j];
		}

		pTempOutData[i] = tTempTotalVal/5;
	}


#if 1
	IplImage *pTempRgbImg=cvCreateImage(cvSize(tInputLength, 800), IPL_DEPTH_8U, 3);
        memset(pTempRgbImg->imageData, 0, pTempRgbImg->widthStep*pTempRgbImg->height);

        unsigned char *pTempImgData=pTempRgbImg->imageData;
        int m,n;
        CvPoint tTempPrevPoint, tTempCurPoint;
        tTempPrevPoint.x=0;
        tTempPrevPoint.y=pTempOutData[0];
        for(m=1; m<pTempRgbImg->width; m ++)
        {
        //        int tTempY=pTempOutData[m];
                tTempCurPoint.x=m;
                tTempCurPoint.y=800-pTempOutData[m];
        //      cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8);

		if(m>=1229 && m<1317 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 0), 1, 8, 0);
		else if(m>=1317 && m<1335 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 0, 255), 1, 8, 0);
		else if(m>=1356 && m<1404 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 255, 0), 1, 8, 0);
		else if(m>=1404 && m<1435 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 255), 1, 8, 0);	
		else
	                cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);
//      printf("damon ====> point : %d %d \n", tTempCurPoint.x, tTempCurPoint.y);
                tTempPrevPoint.x=tTempCurPoint.x;
                tTempPrevPoint.y=tTempCurPoint.y;
	}

	cvShowImage("滤波后图形", pTempRgbImg);
#endif

	return pTempOutData;
}


void filter_noise_data(int tInputStartIndex, int *original_data, int *noise_data, int *filtered_data, int tInputLength)
{
	int tTempSampleCnt=(1000/20)*60/60;

	int i=0, j=0;
	int pTempSaveMinIndex[1000]={0};
	int tTempSaveMinCnt=0;

	int tTempMaxVal=0, tTempMinVal=0;
	unsigned int tTempTotalSampleVal=0;
	int tTempCnt=0;
	int tTempStartSingleIndex=tInputStartIndex;

	int tTempAvgVal=0;


	char tTempUp=0, tTempDown=0, tTempFalseValue=0, tTempFindFirstLow=0;
	HeartValue pTempMinHeartVal[5];
	HeartValue pTempMaxHeartVal[5];
	int tTempMinIdx=0, tTempMaxIdx=0;
	int tTempPrevVal;
	int tTempCurVal=0;
	
	for(i=tTempStartSingleIndex; i<tInputLength; i++)
	{
		tTempTotalSampleVal += original_data[i];
		if((i-tTempStartSingleIndex)>=(tTempCnt+1)*tTempSampleCnt)
		{
			int tTempAvgVal=tTempTotalSampleVal/tTempSampleCnt;
			for(j=0; j<tTempSampleCnt; j++)
			{
			if(original_data[i-tTempSampleCnt+j]<10000)
				printf("damon ===> 00 error data : %d \n", original_data[i-tTempSampleCnt+j]);

				original_data[i-tTempSampleCnt+j] -= tTempAvgVal;

				if(tTempMaxVal<original_data[i-tTempSampleCnt+j])
					tTempMaxVal=original_data[i-tTempSampleCnt+j];
				if(tTempMinVal>original_data[i-tTempSampleCnt+j])
					tTempMinVal=original_data[i-tTempSampleCnt+j];

			}

			tTempCnt++;
			tTempTotalSampleVal=0;
		}
	}

printf("damon ===> sample : %d %d %d \n", i-tTempStartSingleIndex, tTempCnt, tTempSampleCnt*tTempCnt);
	if((i-tTempStartSingleIndex)>tTempCnt*tTempSampleCnt && (i-tTempStartSingleIndex)<(tTempCnt+1)*tTempSampleCnt)
	{
		int tTempCurCnt=(i-tTempStartSingleIndex)-tTempCnt*tTempSampleCnt;
		if(tTempCurCnt>0)
		{
			int tTempAvgVal=tTempTotalSampleVal/tTempCurCnt;
			for(j=0; j<tTempCurCnt; j++)
			{
			if(original_data[i-tTempCurCnt+j]<10000)
				printf("damon ===> 11 error data : %d  %d \n", i-tTempCurCnt+j, original_data[i-tTempCurCnt+j]);

				original_data[i-tTempCurCnt+j] -= tTempAvgVal;
				
				if(tTempMaxVal<original_data[i-tTempCurCnt+j])
					tTempMaxVal=original_data[i-tTempCurCnt+j];
				if(tTempMinVal>original_data[i-tTempCurCnt+j])
					tTempMinVal=original_data[i-tTempCurCnt+j];
			}
		}
	}

printf("damon ===> %d %d \n", tTempMinVal, tTempMaxVal);


	tTempUp=0;
	tTempDown=0;
	tTempFalseValue=0;
	tTempMinIdx=0;
	tTempMaxIdx=0;
	HeartValue pTempMaxSportData[5]={0};
	HeartValue pTempMinSportData[5]={0};

	unsigned int tTempTotalSportVal=0;
	int tTempTotalSportCnt=0;

	tTempPrevVal=noise_data[tTempStartSingleIndex-1];
	for(i=tTempStartSingleIndex; i<tInputLength; i++)
	{
		tTempCurVal=noise_data[i];


		tTempTotalSportVal += tTempCurVal;
		tTempTotalSportCnt++;
		tTempAvgVal=tTempTotalSportVal/tTempTotalSportCnt;

		if(tTempCurVal>tTempPrevVal)
		{
			if(tTempUp==0 && tTempDown==0)
			{
				tTempUp=1;
				pTempMinSportData[tTempMinIdx].index=i;
				pTempMinSportData[tTempMinIdx].value=tTempPrevVal;
				tTempMinIdx++;
			}else if(tTempUp==1)
			{					
				if(tTempFalseValue==1)
				{
					if(tTempCurVal>pTempMaxSportData[tTempMaxIdx-1].value)
					{
						tTempFalseValue=0;
						tTempMaxIdx--;
					}else if(i-pTempMaxSportData[tTempMaxIdx-1].index>10 && tTempCurVal-tTempAvgVal>10)
					{
						tTempFalseValue=0;
						tTempMinIdx--;
					}
				}
			}else if(tTempDown==1)
			{
		//	printf("damon ===> %d %d %d \n", tTempMinIdx, tTempAvgVal, pTempMinSportData[tTempMinIdx].value);
				if(tTempFalseValue==0)
				{
					tTempFalseValue=1;

					pTempMinSportData[tTempMinIdx].index=i;
					pTempMinSportData[tTempMinIdx].value=tTempPrevVal;
					tTempMinIdx++;

					
					tTempTotalSportVal=0;
					tTempTotalSportCnt=0;

				}else if(i-pTempMinSportData[tTempMinIdx-1].index>5)
				{
					tTempDown=0;
					tTempUp=1;
					tTempFalseValue=0;


					if(tTempAvgVal-pTempMinSportData[tTempMinIdx-1].value<5)
					{
						tTempMinIdx--;
					}else
					{
					//	tTempTotalSportVal=0;
					//	tTempTotalSportCnt=0;
					}

					if(tTempMinIdx>=3)
					{
						// cal Hz

				if(tTempSaveMinCnt<1000)
					pTempSaveMinIndex[tTempSaveMinCnt++]=pTempMinSportData[0].index;
						
						printf("damon ===> cal : (%d , %d), (%d, %d) \n", pTempMinSportData[0].index, pTempMinSportData[0].value,
												pTempMinSportData[1].index, pTempMinSportData[1].value);

						pTempMinSportData[0].index=pTempMinSportData[1].index;
						pTempMinSportData[0].value=pTempMinSportData[1].value;							
						pTempMinSportData[1].index=pTempMinSportData[2].index;
						pTempMinSportData[1].value=pTempMinSportData[2].value;
						tTempMinIdx -= 1;
					}
				}
			}
		}else
		{
			if(tTempUp==0 && tTempDown==0)
			{
				tTempDown=1;
				pTempMaxSportData[tTempMaxIdx].index=i;
				pTempMaxSportData[tTempMaxIdx].value=tTempPrevVal;
				tTempMaxIdx++;
			}else if(tTempDown==1)
			{
				if(tTempFalseValue==1)
				{
			//	printf("damon ===> delete value  %d : %d  %d %d  %d %d \n", 
			//		i,  tTempMinIdx, tTempCurVal, pTempMinSportData[tTempMinIdx-1].value, pTempMinSportData[tTempMinIdx-1].index, tTempAvgVal);
			//	sleep(1);
					if(tTempCurVal<pTempMinSportData[tTempMinIdx-1].value)
					{
						tTempFalseValue=0;
						tTempMinIdx--;
					}else if(i-pTempMinSportData[tTempMinIdx-1].index>10 && tTempAvgVal-tTempCurVal>10)
					{
						tTempFalseValue=0;
						tTempMinIdx--;
					}
				}
			}else if(tTempUp==1)
			{
				if(tTempFalseValue==0)
				{
					tTempFalseValue=1;

					pTempMaxSportData[tTempMaxIdx].index=i;
					pTempMaxSportData[tTempMaxIdx].value=tTempPrevVal;
					tTempMaxIdx++;

					
					tTempTotalSportVal=0;
					tTempTotalSportCnt=0;
				}else if(i-pTempMaxSportData[tTempMaxIdx-1].index>5)
				{
		//		printf("damon ==> max idx : %d \n", tTempMaxIdx);
				
					tTempDown=1;
					tTempUp=0;
					tTempFalseValue=0;

					if(pTempMaxSportData[tTempMaxIdx-1].value-tTempAvgVal<5)
					{
						tTempMaxIdx--;
					}else
					{
					//	tTempTotalSportVal=0;
					//	tTempTotalSportCnt=0;
					}

					if(tTempMaxIdx>=3)
					{
						pTempMaxSportData[0].index=pTempMaxSportData[1].index;
						pTempMaxSportData[0].value=pTempMaxSportData[1].value;
						pTempMaxSportData[1].index=pTempMaxSportData[2].index;
						pTempMaxSportData[1].value=pTempMaxSportData[2].value;
						tTempMaxIdx -= 1;
					}

				}
			}
		}

		tTempPrevVal=tTempCurVal;
	}


printf("damon ===> test draw gsensor data \n");

	// test draw data 
	{
		int r=0, g=0, b=0, c=0;
		IplImage *pTempRgbImg=cvCreateImage(cvSize(tInputLength, 800), IPL_DEPTH_8U, 3);
			memset(pTempRgbImg->imageData, 0, pTempRgbImg->widthStep*pTempRgbImg->height);
	
			unsigned char *pTempImgData=pTempRgbImg->imageData;
			int m,n;
			CvPoint tTempPrevPoint, tTempCurPoint;
			tTempPrevPoint.x=0;
			tTempPrevPoint.y=800-noise_data[m]+250;
			for(m=1; m<pTempRgbImg->width; m ++)
			{
			//		  int tTempY=pTempOutData[m];
					tTempCurPoint.x=m;
					tTempCurPoint.y=800-noise_data[m]+250;
			//		cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8);

				if(c<tTempSaveMinCnt-1 && m>=pTempSaveMinIndex[c])
				{
				
			//	printf("damon ===> save cnt : %d %d %d %d %d \n", c, tTempSaveMinCnt, m, pTempSaveMinIndex[c], pTempSaveMinIndex[c+1]);
					r=abs(rand()%255);
					g=abs(rand()%255);
					b=abs(rand()%255);
				
					for(i=pTempSaveMinIndex[c]; i<pTempSaveMinIndex[c+1]; i++)
					{
						tTempCurPoint.x=m;
						tTempCurPoint.y=800-noise_data[m]+250;
						cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(r, g, b), 1, 8, 0);

						m++;
						
						tTempPrevPoint.x=tTempCurPoint.x;
						tTempPrevPoint.y=tTempCurPoint.y;
					}
					
					c++;
					m=pTempSaveMinIndex[c]-1;

				//	printf("damon ===> index : %d \n", m);
				}else
				{
					cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);

					
					tTempPrevPoint.x=tTempCurPoint.x;
					tTempPrevPoint.y=tTempCurPoint.y;
				}
	
	//		printf("damon ====> point : %d %d \n", tTempCurPoint.x, tTempCurPoint.y);
		}
	
		cvShowImage("滤波后图形", pTempRgbImg);
	}


	{
		int k=0, c=0;
		int tTempFilterCnt=tInputLength; //tInputLength-(pTempSaveMinIndex[tTempSaveMinCnt-1]-pTempSaveMinIndex[0])+tTempSaveMinCnt;
		int *pTempFilterData=(int *)malloc(sizeof(int)*(tTempFilterCnt+1));
		for(i=0; i<tInputLength; i++)
		{
	//	printf("damon ===> index = %d , %d \n", i, tInputLength);
			if(i>=pTempSaveMinIndex[0] && i<=pTempSaveMinIndex[tTempSaveMinCnt-1])
			{
				c=0;
				while(pTempSaveMinIndex[c]<i)
				{
					c++;
					if(c>=tTempSaveMinCnt)
						break;
				}
			
			
				int tTempPrevIdx=c;
				while(pTempSaveMinIndex[c+1]-pTempSaveMinIndex[tTempPrevIdx]<200)
				{
					c++;
					if(c>=tTempSaveMinCnt-1)
						break;
				}

			printf("damon ===> c = %d %d %d , prev idx = %d %d \n", c, pTempSaveMinIndex[c], tTempSaveMinCnt, tTempPrevIdx, pTempSaveMinIndex[tTempPrevIdx]);
	//		sleep(1);
				int tTempDiv=c-tTempPrevIdx;
				if(c<tTempSaveMinCnt && c>tTempPrevIdx && tTempDiv!=1)
				{
		//		printf("damon ===> c- prev idx = %d  %d \n", c-tTempPrevIdx, tTempDiv);
					int tTempWinWidth=50*4/(c-tTempPrevIdx);
					float x=(float)(1.0*4/(c-tTempPrevIdx));
					float A=pow(x,3)-pow(x, 2)+1.01;
		//		printf("damon ===> x= %f, A= %f \n", x, A);
					int tTempHTotalData=0;
					int m=0;
				printf("damon === > win width : %d \n", tTempWinWidth);
			//	sleep(1);
					for(j=i; j<i+200-tTempWinWidth; j++)
					{
						tTempHTotalData=0;
						for(m=j; m<j+tTempWinWidth; m++)
							tTempHTotalData += original_data[m];

						if(k<tTempFilterCnt)
							pTempFilterData[k++]=(tTempHTotalData/tTempWinWidth)/A;
						else 
							printf("damon ===> error data  out of range ! \n");
						
						tTempHTotalData += original_data[m];
					}
					i+=(200-tTempWinWidth)-1;
			//	printf("damon ===>  i = %d \n", i);
				}else
				{
					if(k<tTempFilterCnt)
						pTempFilterData[k++]=original_data[i];
					else 
						printf("damon ===> error data  out of range ! \n");
				}
			}else 
			{
				if(k<tTempFilterCnt)
					pTempFilterData[k++]=original_data[i];
				else 
					printf("damon ===> error data  out of range ! \n");
			}
		}

cal_heart(pTempFilterData, tTempFilterCnt);


		IplImage *pTempRgbImg=cvCreateImage(cvSize(tTempFilterCnt, 1000), IPL_DEPTH_8U, 3);
       		memset(pTempRgbImg->imageData, 0, pTempRgbImg->widthStep*pTempRgbImg->height);
		unsigned char *pTempImgData=pTempRgbImg->imageData;
 	        int m,n;
        	CvPoint tTempPrevPoint, tTempCurPoint;
       		tTempPrevPoint.x=0;
        	tTempPrevPoint.y=pTempFilterData[0];
        	for(m=1; m<pTempRgbImg->width; m ++)
        	{
                	int tTempY=pTempFilterData[m];
                	tTempCurPoint.x=m;
               		tTempCurPoint.y= 1000-(pTempFilterData[m]+30000-10000)*1000/60000; //pTempFilterData[m];

	              	cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);
		//      printf("damon ====> point : %d %d \n", tTempCurPoint.x, tTempCurPoint.y);
        	        tTempPrevPoint.x=tTempCurPoint.x;
        	        tTempPrevPoint.y=tTempCurPoint.y;
	        }
		cvShowImage("my cal bitmap", pTempRgbImg);
		
	}

	printf("damon ===> end of filter noise data \n");
}


void MyTestFilter(int *original_data, int *noise_data, int *filtered_data, int tInputLength)
{
	// 20ms samples
	int tTempSampleCnt=(1000/20)*60/60;


	int i=0, j=0;
	int tTempAvgVal=0;
	unsigned int tTempTotalVal=0;
	int tTempStartSingleIndex=0;

	int pTempValue[7];
	for(i=0; i<tInputLength-7; i++)
	{
		int t=0;
		for(j=0; j<7; j++)
		{
			pTempValue[j]=original_data[i+j];
			t += pTempValue[j];
		}

		int m=0, n=0;
		for(m=0; m<6; m++)
		{
			for(n=m+1; n<7; n++)
			{
				if(pTempValue[m]>pTempValue[n])
				{
					int tTempSwap=pTempValue[m];
					pTempValue[m]=pTempValue[n];
					pTempValue[n]=tTempSwap;
				}
			}	
		}

		filtered_data[i]=original_data[i];//pTempValue[2];
	}


#if 1
	// find first spie
	char tTempUp=0, tTempDown=0, tTempFalseValue=0, tTempFindFirstLow=0;
	HeartValue pTempMinHeartVal[7];
	HeartValue pTempMaxHeartVal[7];
	int tTempMinIdx=0, tTempMaxIdx=0;
	int tTempPrevVal;
	int tTempCurVal=0;
	tTempPrevVal= filtered_data[0];//original_data[0];
	unsigned int tTempTotalHeartValue=0;
	int tTempTotalHeartCnt=0;
	for(i=1; i<tInputLength; i++)
	{
		tTempCurVal= filtered_data[i];//original_data[i];

	//	tTempTotalHeartValue += tTempCurVal;
	//	tTempTotalHeartCnt++;
		
		if(tTempCurVal>tTempPrevVal)
		{
			if(tTempUp==0 && tTempDown==0)
			{
				pTempMinHeartVal[tTempMinIdx].index=i;
				pTempMinHeartVal[tTempMinIdx].value=tTempPrevVal;
				tTempMinIdx++;
				tTempPrevVal=tTempCurVal;
				tTempUp=1;
			}else if(tTempUp==1)
			{
				if(tTempFalseValue>0)
				{
					if(tTempMaxIdx<=0)
						printf("damon ===> max idx error : %d \n", tTempMaxIdx);
				
					if( tTempCurVal>pTempMaxHeartVal[tTempMaxIdx-1].value /*|| (i-pTempMaxHeartVal[tTempMaxIdx-1].index>8)*/)
					{
						if(i-pTempMaxHeartVal[tTempMaxIdx-1].index>8)
							printf("damon ===> up adjuest value max val =  %d %d -%d %d \n", i, tTempCurVal, pTempMaxHeartVal[tTempMaxIdx-1].index, pTempMaxHeartVal[tTempMaxIdx-1].value);
					
						tTempUp=1;
						tTempDown=0;
						tTempMaxIdx--;
					
						tTempFalseValue=0;
					}
				}
			
				tTempPrevVal=tTempCurVal;
			}else if(tTempDown==1)
			{
				if(tTempFalseValue==0)
				{
					tTempFalseValue=1;

		//		printf("damon ===> min idx 00 = %d \n", tTempMinIdx);
					if(tTempMinIdx<7)
					{

					#if 1
						// filter  max value
						if(tTempMinIdx>=2)
						{
							if(i-pTempMinHeartVal[tTempMinIdx-1].index>100 || pTempMinHeartVal[tTempMinIdx-1].value<12000)
							{
								tTempMinIdx=0;
								tTempMaxIdx=0;

								
								tTempFalseValue=0;
								tTempUp=0;
								tTempDown=0;
							}
						
							int m=0, n=0;
							for(m=0; m<tTempMinIdx-1; m++)
							{
								if(pTempMinHeartVal[m].value>45000)
								{
									for(n=m; n<tTempMinIdx-1; n++)
									{
										pTempMinHeartVal[n].index=pTempMinHeartVal[n+1].index;
										pTempMinHeartVal[n].value=pTempMinHeartVal[n+1].value;
									}

									m--;
									tTempMinIdx--;
								}else
								{
									if(/*m>0 &&*/ m<tTempMinIdx-2)
									{
										if(pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index>100)
										{//printf("damon ===> error delete max value %d : %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index);
											for(n=0; n<m+2; n++)
											{
												if(n+m+1<tTempMinIdx)
												{
													pTempMinHeartVal[n].index=pTempMinHeartVal[n+m+2].index;
													pTempMinHeartVal[n].value=pTempMinHeartVal[n+m+2].value;
												}
											}
											tTempMinIdx=tTempMinIdx-(m+1);
											m=0;
										//	printf("damon ===> value : %d \n", tTempMaxIdx);
										}else
										if(((pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index<20) || (pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index>80)) && 
											(pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index>20))
										{
									if(pTempMinHeartVal[m].index>1932 && pTempMinHeartVal[m].index<1994)
										printf("damon xxxxxxx> delete value %d : %d %d %d \n", tTempMinIdx, pTempMinHeartVal[m].index, pTempMinHeartVal[m+1].index, pTempMinHeartVal[m+2].index);

											for(n=m; n<tTempMinIdx-1; n++)
											{
												pTempMinHeartVal[n].index=pTempMinHeartVal[n+1].index;
												pTempMinHeartVal[n].value=pTempMinHeartVal[n+1].value;
											}
											
											m--;
											tTempMinIdx--;
										}else if((pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index>20) &&
											((pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index<20) || (pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index>80)))
										{
								//		printf("damon ==> error delete value %d : %d %d %d \n", tTempMinIdx, pTempMinHeartVal[m].index, pTempMinHeartVal[m+1].index, pTempMinHeartVal[m+2].index);
											for(n=m+2; n<tTempMinIdx-1; n++)
											{
												pTempMinHeartVal[n].index=pTempMinHeartVal[n+1].index;
												pTempMinHeartVal[n].value=pTempMinHeartVal[n+1].value;
											}
											
											m+=2;
											tTempMinIdx--;											
										}else if(((pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index<20) || (pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index>80)) &&
											((pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index<20) || (pTempMinHeartVal[m+2].index-pTempMinHeartVal[m+1].index>80)))
										{
											for(n=m; n<tTempMinIdx-2; n++)
											{
												pTempMinHeartVal[n].index=pTempMinHeartVal[n+2].index;
												pTempMinHeartVal[n].value=pTempMinHeartVal[n+2].value;
											}
											
											m--;
											tTempMinIdx-=2;											
										}
									}
								}
							}							
						}
					#endif


						printf("damon ===> min idx 11 = %d \n", tTempMinIdx);
					
						pTempMinHeartVal[tTempMinIdx].index=i;
						pTempMinHeartVal[tTempMinIdx].value=tTempPrevVal;
						tTempMinIdx++;
					}
				}else if((i-pTempMinHeartVal[tTempMinIdx-1].index>8) && (tTempCurVal>pTempMinHeartVal[tTempMinIdx-1].value))
				{
					tTempDown=0;
					tTempUp=1;
					tTempFalseValue=0;

		/*	{	// filter average value
				if(tTempTotalHeartCnt>0)
				{
					int m=0, n=0;
					int tTempAvgVal=tTempTotalHeartValue/tTempTotalHeartCnt;
					for(m=0; m<tTempMinIdx; m++)
					{
						if(pTempMinHeartVal[m].value>=tTempAvgVal-10)
						{
							for(n=m; n<tTempMinIdx-1; n++)
							{
								pTempMinHeartVal[n]=pTempMinHeartVal[n+1];
							}
							tTempMinIdx--;
						}
					}
				}
				tTempTotalHeartCnt=0;
				tTempTotalHeartValue=0;
			}*/

				printf("damon ===> test max val : (%d, %d)  - (%d, %d) \n", pTempMaxHeartVal[tTempMaxIdx-1].index, pTempMaxHeartVal[tTempMaxIdx-1].value,
					pTempMinHeartVal[tTempMinIdx-1].index, pTempMinHeartVal[tTempMinIdx-1].value);
		
				if(tTempMinIdx>2)
				{
					if(pTempMaxHeartVal[tTempMaxIdx-1].value-pTempMinHeartVal[tTempMinIdx-1].value<2000)
					{
					printf(LIGHT_RED"damon ===> delete min val : %d %d - %d %d \n"NONE, pTempMinHeartVal[tTempMinIdx-1].index, pTempMinHeartVal[tTempMinIdx-1].value, pTempMaxHeartVal[tTempMaxIdx-1].index, pTempMaxHeartVal[tTempMaxIdx-1].value);
						tTempMinIdx--;
					}
				}
		
			
					if(tTempMinIdx>=7)
					{
						printf("damon ===> min : (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d) (%d, %d) (%d, %d)\n", 
							pTempMinHeartVal[0].index, pTempMinHeartVal[0].value,
							pTempMinHeartVal[1].index, pTempMinHeartVal[1].value,
							pTempMinHeartVal[2].index, pTempMinHeartVal[2].value,
							pTempMinHeartVal[3].index, pTempMinHeartVal[3].value,
							pTempMinHeartVal[4].index, pTempMinHeartVal[4].value,
							pTempMinHeartVal[5].index, pTempMinHeartVal[5].value,
							pTempMinHeartVal[6].index, pTempMinHeartVal[6].value);						

						if(tTempStartSingleIndex==0)
							tTempStartSingleIndex=pTempMinHeartVal[0].index;


						int m=0;
						int tTempPointCount=pTempMinHeartVal[5].index-pTempMinHeartVal[0].index;
						int tTempMaxPoint=0, tTempMinPoint=1000;
						for(m=0; m<5; m++)
						{
							int tTempSubIdx=pTempMinHeartVal[m+1].index-pTempMinHeartVal[m].index;
							if(tTempSubIdx>tTempMaxPoint)
								tTempMaxPoint=tTempSubIdx;
							if(tTempSubIdx<tTempMinPoint)
								tTempMinPoint=tTempSubIdx;
						}
						// delete max and min value 
						tTempPointCount -= (tTempMaxPoint+tTempMinPoint);
						int tTempRate=(60*1000)/((tTempPointCount*20)/3);
						for(m=0; m<5; m++)
						{
						pXImgPoint[tXPointCnt++]=pTempMinHeartVal[m].index;

							if(m+5<7)
							{
								pTempMinHeartVal[m].index=pTempMinHeartVal[m+5].index;
								pTempMinHeartVal[m].value=pTempMinHeartVal[m+5].value;

								pTempMinHeartVal[m+5].index=0;
								pTempMinHeartVal[m+5].value=0;
							}else
							{
								pTempMinHeartVal[m].index=0;
								pTempMinHeartVal[m].value=0;
							}
						}
						tTempMinIdx-=5;

						printf(CYAN"damon ===> heart rate : %d  %d  %d \n"NONE, tTempRate, tTempStartSingleIndex, tTempMinIdx);
				//		goto start_ok;

					}
				}

				tTempPrevVal=tTempCurVal;
			}

		}else if(tTempCurVal < tTempPrevVal)
		{
			if(tTempUp==0 && tTempDown==0)
			{//printf("damon ===> start max idx : %d %d %d \n", tTempMaxIdx, i, tTempPrevVal);
				pTempMaxHeartVal[tTempMaxIdx].index=i;
				pTempMaxHeartVal[tTempMaxIdx].value=tTempPrevVal;
				tTempMaxIdx++;
				tTempPrevVal=tTempCurVal;
				tTempDown=1;
			}else if(tTempDown==1)
			{
				if(tTempFalseValue>0)
				{
					if(tTempMinIdx<=0)
						printf("damon ===> min idx error : %d \n", tTempMinIdx);

					if(tTempCurVal<pTempMinHeartVal[tTempMinIdx-1].value /*|| (i-pTempMinHeartVal[tTempMinIdx-1].index>8)*/)
					{
						if(i-pTempMinHeartVal[tTempMinIdx-1].index>8)
							printf("damon ===> down adjuest value min value = %d %d \n", pTempMinHeartVal[tTempMinIdx-1].index, pTempMinHeartVal[tTempMinIdx-1].value);

					printf("damon ===> min idx 111 = %d \n", tTempMinIdx);
						tTempUp=0;
						tTempDown=1;
						tTempMinIdx--;
					
						tTempFalseValue=0;
					}					
				}
				
				tTempPrevVal=tTempCurVal;
			}else if(tTempUp==1)
			{
				if(tTempFalseValue==0)
				{
					tTempFalseValue=1;
			//	printf("damon ===> max idx : %d \n", tTempMaxIdx);
					if(tTempMaxIdx<7)
					{
						// filter value
						if(tTempMaxIdx>=2)
						{
							if(i-pTempMaxHeartVal[tTempMaxIdx-1].index>100 || pTempMaxHeartVal[tTempMaxIdx-1].value>50000)
							{
							printf("damon ===> min idx 222 = %d - %d %d %d\n", tTempMinIdx, i, pTempMaxHeartVal[tTempMaxIdx-1].index, pTempMaxHeartVal[tTempMaxIdx-1].value);
								tTempMaxIdx=0;
								tTempMinIdx=0;

								tTempFalseValue=0;
								tTempUp=0;
								tTempDown=0;
							}
						
							int m=0, n=0;
							for(m=0; m<tTempMaxIdx-1; m++)
							{
								if(pTempMaxHeartVal[m].value<15000)
								{
									for(n=m; n<tTempMaxIdx-1; n++)
									{
										pTempMaxHeartVal[n].index=pTempMaxHeartVal[n+1].index;
										pTempMaxHeartVal[n].value=pTempMaxHeartVal[n+1].value;
									}

									m--;
									tTempMaxIdx--;
								}else
								{

									if(/*m>0 &&*/ m<tTempMaxIdx-2)
									{
									
										if(pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index>100)
										{//printf("damon ===> error delete max value %d : %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index);
											for(n=0; n<m+2; n++)
											{
												if(n+m+1<tTempMaxIdx)
												{
													pTempMaxHeartVal[n].index=pTempMaxHeartVal[n+m+2].index;
													pTempMaxHeartVal[n].value=pTempMaxHeartVal[n+m+2].value;
												}
											}
											
											tTempMaxIdx=tTempMaxIdx-(m+1);
											m=0;
										//	printf("damon ===> value : %d \n", tTempMaxIdx);
										}else
										if(((pTempMaxHeartVal[m+1].index-pTempMaxHeartVal[m].index<20) || (pTempMaxHeartVal[m+1].index-pTempMaxHeartVal[m].index>80))&& 
											(pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index>20))
										{										
								//		printf("damon ==> delete max value %d : %d %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index, pTempMaxHeartVal[m+2].index);
											for(n=m; n<tTempMaxIdx-1; n++)
											{
												pTempMaxHeartVal[n].index=pTempMaxHeartVal[n+1].index;
												pTempMaxHeartVal[n].value=pTempMaxHeartVal[n+1].value;
											}
											
											m--;
											tTempMaxIdx--;
										}else if((pTempMaxHeartVal[m+1].index-pTempMaxHeartVal[m].index>20) &&
											((pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index<20) || (pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index>80)))
										{
											printf("damon ==> 1 delete max value %d : %d %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index, pTempMaxHeartVal[m+2].index);
											for(n=m+2; n<tTempMaxIdx-1; n++)
											{
												pTempMaxHeartVal[n].index=pTempMaxHeartVal[n+1].index;
												pTempMaxHeartVal[n].value=pTempMaxHeartVal[n+1].value;
											}
											
											m+=2;
											tTempMaxIdx--;
										}else if(((pTempMaxHeartVal[m+1].index-pTempMaxHeartVal[m].index<20) || (pTempMaxHeartVal[m+1].index-pTempMaxHeartVal[m].index>80) )&& 
											((pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index<20) || (pTempMaxHeartVal[m+2].index-pTempMaxHeartVal[m+1].index>80)))
										{
								//			printf("damon ==> 2 delete max value %d : %d %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index, pTempMaxHeartVal[m+2].index);										
											for(n=m; n<tTempMaxIdx-2; n++)
											{
												pTempMaxHeartVal[n].index=pTempMaxHeartVal[n+2].index;
												pTempMaxHeartVal[n].value=pTempMaxHeartVal[n+2].value;
											}
											
											m--;
											tTempMaxIdx-=2;
										}
									}
								}
							}							
						}
					
						pTempMaxHeartVal[tTempMaxIdx].index=i;
						pTempMaxHeartVal[tTempMaxIdx].value=tTempPrevVal;
						tTempMaxIdx++;		
				//	printf("damon ===> max idx = %d \n", tTempMaxIdx);
					}
				}else if((i-pTempMaxHeartVal[tTempMaxIdx-1].index>8) && (tTempCurVal<pTempMaxHeartVal[tTempMaxIdx-1].value))
				{
					tTempUp=0;
					tTempDown=1;
					tTempFalseValue=0;


			/*	{	// filter average value
					if(tTempTotalHeartCnt>0)
					{
						int m=0, n=0;
						int tTempAvgVal=tTempTotalHeartValue/tTempTotalHeartCnt;
						for(m=0; m<tTempMaxIdx; m++)
						{
							if(pTempMaxHeartVal[m].value<tTempAvgVal+10)
							{
								for(n=m; n<tTempMaxIdx-1; n++)
								{
									pTempMaxHeartVal[n]=pTempMaxHeartVal[n+1];
								}
								tTempMaxIdx--;
							}
						}
					}
					tTempTotalHeartCnt=0;
					tTempTotalHeartValue=0;
				}*/


				//	printf("damon ===> cal max val : %d \n", tTempMaxIdx);
					if(tTempMaxIdx>=7)
					{
						printf("damon ===> max : (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d) (%d, %d) (%d, %d)\n", 
							pTempMaxHeartVal[0].index, pTempMaxHeartVal[0].value,
							pTempMaxHeartVal[1].index, pTempMaxHeartVal[1].value,
							pTempMaxHeartVal[2].index, pTempMaxHeartVal[2].value,
							pTempMaxHeartVal[3].index, pTempMaxHeartVal[3].value,
							pTempMaxHeartVal[4].index, pTempMaxHeartVal[4].value,
							pTempMaxHeartVal[5].index, pTempMaxHeartVal[5].value,
							pTempMaxHeartVal[6].index, pTempMaxHeartVal[6].value);

						int m=0;
						for(m=0; m<5; m++)
						{
							if(m+5<7)
							{
								pTempMaxHeartVal[m].index=pTempMaxHeartVal[m+5].index;
								pTempMaxHeartVal[m].value=pTempMaxHeartVal[m+5].value;

								pTempMaxHeartVal[m+5].index=0;
								pTempMaxHeartVal[m+5].value=0;
							}else
							{
								pTempMaxHeartVal[m].index=0;
								pTempMaxHeartVal[m].value=0;
							}
						}
						tTempMaxIdx-=5;
					}
				}

				tTempPrevVal=tTempCurVal;
			}
		}

	}
#endif



start_ok:
	
	if(tTempStartSingleIndex>0)
	{
#ifndef dNoGSensorData
	#if 1
	//	filter_noise_data(tTempStartSingleIndex, original_data, noise_data, filtered_data, tInputLength);
	#else
		int pTempSaveMinIndex[1000]={0};
		int tTempSaveMinCnt=0;
	
		int tTempMaxVal=0, tTempMinVal=0;
		unsigned int tTempTotalSampleVal=0;
		int tTempCnt=0;
		for(i=tTempStartSingleIndex; i<tInputLength; i++)
		{
			tTempTotalSampleVal += original_data[i];
			if((i-tTempStartSingleIndex)>=(tTempCnt+1)*tTempSampleCnt)
			{
				int tTempAvgVal=tTempTotalSampleVal/tTempSampleCnt;
				for(j=0; j<tTempSampleCnt; j++)
				{
				if(original_data[i-tTempSampleCnt+j]<10000)
					printf("damon ===> 00 error data : %d \n", original_data[i-tTempSampleCnt+j]);

					original_data[i-tTempSampleCnt+j] -= tTempAvgVal;

					if(tTempMaxVal<original_data[i-tTempSampleCnt+j])
						tTempMaxVal=original_data[i-tTempSampleCnt+j];
					if(tTempMinVal>original_data[i-tTempSampleCnt+j])
						tTempMinVal=original_data[i-tTempSampleCnt+j];

				}

				tTempCnt++;
				tTempTotalSampleVal=0;
			}
		}

	printf("damon ===> sample : %d %d %d \n", i-tTempStartSingleIndex, tTempCnt, tTempSampleCnt*tTempCnt);
		if((i-tTempStartSingleIndex)>tTempCnt*tTempSampleCnt && (i-tTempStartSingleIndex)<(tTempCnt+1)*tTempSampleCnt)
		{
			int tTempCurCnt=(i-tTempStartSingleIndex)-tTempCnt*tTempSampleCnt;
			if(tTempCurCnt>0)
			{
				int tTempAvgVal=tTempTotalSampleVal/tTempCurCnt;
				for(j=0; j<tTempCurCnt; j++)
				{
				if(original_data[i-tTempCurCnt+j]<10000)
					printf("damon ===> 11 error data : %d  %d \n", i-tTempCurCnt+j, original_data[i-tTempCurCnt+j]);

					original_data[i-tTempCurCnt+j] -= tTempAvgVal;
					
					if(tTempMaxVal<original_data[i-tTempCurCnt+j])
						tTempMaxVal=original_data[i-tTempCurCnt+j];
					if(tTempMinVal>original_data[i-tTempCurCnt+j])
						tTempMinVal=original_data[i-tTempCurCnt+j];
				}
			}
		}

	printf("damon ===> %d %d \n", tTempMinVal, tTempMaxVal);


		tTempUp=0;
		tTempDown=0;
		tTempFalseValue=0;
		tTempMinIdx=0;
		tTempMaxIdx=0;
		HeartValue pTempMaxSportData[5]={0};
		HeartValue pTempMinSportData[5]={0};

		unsigned int tTempTotalSportVal=0;
		int tTempTotalSportCnt=0;
	
		tTempPrevVal=noise_data[tTempStartSingleIndex-1];
		for(i=tTempStartSingleIndex; i<tInputLength; i++)
		{
			tTempCurVal=noise_data[i];


			tTempTotalSportVal += tTempCurVal;
			tTempTotalSportCnt++;
			tTempAvgVal=tTempTotalSportVal/tTempTotalSportCnt;

			if(tTempCurVal>tTempPrevVal)
			{
				if(tTempUp==0 && tTempDown==0)
				{
					tTempUp=1;
					pTempMinSportData[tTempMinIdx].index=i;
					pTempMinSportData[tTempMinIdx].value=tTempPrevVal;
					tTempMinIdx++;
				}else if(tTempUp==1)
				{					
					if(tTempFalseValue==1)
					{
						if(tTempCurVal>pTempMaxSportData[tTempMaxIdx-1].value)
						{
							tTempFalseValue=0;
							tTempMaxIdx--;
						}else if(i-pTempMaxSportData[tTempMaxIdx-1].index>10 && tTempCurVal-tTempAvgVal>10)
						{
							tTempFalseValue=0;
							tTempMinIdx--;
						}
					}
				}else if(tTempDown==1)
				{
			//	printf("damon ===> %d %d %d \n", tTempMinIdx, tTempAvgVal, pTempMinSportData[tTempMinIdx].value);
					if(tTempFalseValue==0)
					{
						tTempFalseValue=1;

						pTempMinSportData[tTempMinIdx].index=i;
						pTempMinSportData[tTempMinIdx].value=tTempPrevVal;
						tTempMinIdx++;

						
						tTempTotalSportVal=0;
						tTempTotalSportCnt=0;

					}else if(i-pTempMinSportData[tTempMinIdx-1].index>5)
					{
						tTempDown=0;
						tTempUp=1;
						tTempFalseValue=0;


						if(tTempAvgVal-pTempMinSportData[tTempMinIdx-1].value<5)
						{
							tTempMinIdx--;
						}else
						{
						//	tTempTotalSportVal=0;
						//	tTempTotalSportCnt=0;
						}

						if(tTempMinIdx>=3)
						{
							// cal Hz

					if(tTempSaveMinCnt<1000)
						pTempSaveMinIndex[tTempSaveMinCnt++]=pTempMinSportData[0].index;
							
							printf("damon ===> cal : (%d , %d), (%d, %d) \n", pTempMinSportData[0].index, pTempMinSportData[0].value,
													pTempMinSportData[1].index, pTempMinSportData[1].value);

							pTempMinSportData[0].index=pTempMinSportData[1].index;
							pTempMinSportData[0].value=pTempMinSportData[1].value;							
							pTempMinSportData[1].index=pTempMinSportData[2].index;
							pTempMinSportData[1].value=pTempMinSportData[2].value;
							tTempMinIdx -= 1;
						}
					}
				}
			}else
			{
				if(tTempUp==0 && tTempDown==0)
				{
					tTempDown=1;
					pTempMaxSportData[tTempMaxIdx].index=i;
					pTempMaxSportData[tTempMaxIdx].value=tTempPrevVal;
					tTempMaxIdx++;
				}else if(tTempDown==1)
				{
					if(tTempFalseValue==1)
					{
				//	printf("damon ===> delete value  %d : %d  %d %d  %d %d \n", 
				//		i,  tTempMinIdx, tTempCurVal, pTempMinSportData[tTempMinIdx-1].value, pTempMinSportData[tTempMinIdx-1].index, tTempAvgVal);
				//	sleep(1);
						if(tTempCurVal<pTempMinSportData[tTempMinIdx-1].value)
						{
							tTempFalseValue=0;
							tTempMinIdx--;
						}else if(i-pTempMinSportData[tTempMinIdx-1].index>10 && tTempAvgVal-tTempCurVal>10)
						{
							tTempFalseValue=0;
							tTempMinIdx--;
						}
					}
				}else if(tTempUp==1)
				{
					if(tTempFalseValue==0)
					{
						tTempFalseValue=1;

						pTempMaxSportData[tTempMaxIdx].index=i;
						pTempMaxSportData[tTempMaxIdx].value=tTempPrevVal;
						tTempMaxIdx++;

						
						tTempTotalSportVal=0;
						tTempTotalSportCnt=0;
					}else if(i-pTempMaxSportData[tTempMaxIdx-1].index>5)
					{
			//		printf("damon ==> max idx : %d \n", tTempMaxIdx);
					
						tTempDown=1;
						tTempUp=0;
						tTempFalseValue=0;

						if(pTempMaxSportData[tTempMaxIdx-1].value-tTempAvgVal<5)
						{
							tTempMaxIdx--;
						}else
						{
						//	tTempTotalSportVal=0;
						//	tTempTotalSportCnt=0;
						}

						if(tTempMaxIdx>=3)
						{
							pTempMaxSportData[0].index=pTempMaxSportData[1].index;
							pTempMaxSportData[0].value=pTempMaxSportData[1].value;
							pTempMaxSportData[1].index=pTempMaxSportData[2].index;
							pTempMaxSportData[1].value=pTempMaxSportData[2].value;
							tTempMaxIdx -= 1;
						}

					}
				}
			}

			tTempPrevVal=tTempCurVal;
		}



		// test draw data 
		{
			int r=0, g=0, b=0, c=0;
			IplImage *pTempRgbImg=cvCreateImage(cvSize(tInputLength, 800), IPL_DEPTH_8U, 3);
				memset(pTempRgbImg->imageData, 0, pTempRgbImg->widthStep*pTempRgbImg->height);
		
				unsigned char *pTempImgData=pTempRgbImg->imageData;
				int m,n;
				CvPoint tTempPrevPoint, tTempCurPoint;
				tTempPrevPoint.x=0;
				tTempPrevPoint.y=800-noise_data[m]+300;
				for(m=1; m<pTempRgbImg->width; m ++)
				{
				//		  int tTempY=pTempOutData[m];
						tTempCurPoint.x=m;
						tTempCurPoint.y=800-noise_data[m]+300;
				//		cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8);

					if(c<tTempSaveMinCnt-1 && m>=pTempSaveMinIndex[c])
					{
					
				//	printf("damon ===> save cnt : %d %d %d %d %d \n", c, tTempSaveMinCnt, m, pTempSaveMinIndex[c], pTempSaveMinIndex[c+1]);
						r=abs(rand()%255);
						g=abs(rand()%255);
						b=abs(rand()%255);
					
						for(i=pTempSaveMinIndex[c]; i<pTempSaveMinIndex[c+1]; i++)
						{
							tTempCurPoint.x=m;
							tTempCurPoint.y=800-noise_data[m]+300;
							cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(r, g, b), 1, 8, 0);

							m++;
							
							tTempPrevPoint.x=tTempCurPoint.x;
							tTempPrevPoint.y=tTempCurPoint.y;
						}
						
						c++;
						m=pTempSaveMinIndex[c]-1;

					//	printf("damon ===> index : %d \n", m);
					}else
					{
						cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);

						
						tTempPrevPoint.x=tTempCurPoint.x;
						tTempPrevPoint.y=tTempCurPoint.y;
					}
		
		//		printf("damon ====> point : %d %d \n", tTempCurPoint.x, tTempCurPoint.y);
			}
		
			cvShowImage("滤波后图形", pTempRgbImg);
		}


		{
			int k=0, c=0;
			int tTempFilterCnt=tInputLength; //tInputLength-(pTempSaveMinIndex[tTempSaveMinCnt-1]-pTempSaveMinIndex[0])+tTempSaveMinCnt;
			int *pTempFilterData=(int *)malloc(sizeof(int)*(tTempFilterCnt+1));
			for(i=0; i<tInputLength; i++)
			{
				if(i>=pTempSaveMinIndex[0] && i<=pTempSaveMinIndex[tTempSaveMinCnt-1])
				{
					c=0;
					while(pTempSaveMinIndex[c]<i)
					{
						c++;
						if(c>=tTempSaveMinCnt)
							break;
					}
				
				
					int tTempPrevIdx=c;
					while(pTempSaveMinIndex[c+1]-pTempSaveMinIndex[tTempPrevIdx]<200)
					{
						c++;
						if(c>=tTempSaveMinCnt-1)
							break;
					}

				printf("damon ===> c = %d , prev idx = %d \n", c, tTempPrevIdx);
				//	int tTempDiv=c-tTempPrevIdx;
					if(c<tTempSaveMinCnt && c>tTempPrevIdx)
					{
					printf("damon ===> c- prev idx = %d  \n", c-tTempPrevIdx);
					//	int tTempWinWidth=50/(c-tTempPrevIdx);
						int tTempHTotalData=0;
						int m=0;
				//	printf("damon === > win width : %d \n", tTempWinWidth);
					sleep(1);
					/*	for(j=i; j<i+200-tTempWinWidth; j++)
						{
							tTempHTotalData=0;
							for(m=j; m<tTempWinWidth; m++)
								tTempHTotalData += original_data[m];

							if(k<tTempFilterCnt)
								pTempFilterData[k++]=tTempHTotalData/tTempWinWidth;
							else 
								printf("damon ===> error data  out of range ! \n");
						}*/
					//	i += (200-tTempWinWidth);
					printf("damon ===> i = %d \n", i);
					}else
					{
						if(k<tTempFilterCnt)
							pTempFilterData[k++]=original_data[i];
						else 
							printf("damon ===> error data  out of range ! \n");
					}
				}else 
				{
					if(k<tTempFilterCnt)
						pTempFilterData[k++]=original_data[i];
					else 
						printf("damon ===> error data  out of range ! \n");
				}
			}


			IplImage *pTempRgbImg=cvCreateImage(cvSize(tTempFilterCnt, 1000), IPL_DEPTH_8U, 3);
	       		memset(pTempRgbImg->imageData, 0, pTempRgbImg->widthStep*pTempRgbImg->height);
			unsigned char *pTempImgData=pTempRgbImg->imageData;
	 	        int m,n;
	        	CvPoint tTempPrevPoint, tTempCurPoint;
	       		tTempPrevPoint.x=0;
	        	tTempPrevPoint.y=pTempFilterData[0];
	        	for(m=1; m<pTempRgbImg->width; m ++)
	        	{
	                	int tTempY=pTempFilterData[m];
	                	tTempCurPoint.x=m;
	               		tTempCurPoint.y= 1000-(pTempFilterData[m]+30000-10000)*1000/60000; //pTempFilterData[m];

		              	cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);
			//      printf("damon ====> point : %d %d \n", tTempCurPoint.x, tTempCurPoint.y);
	        	        tTempPrevPoint.x=tTempCurPoint.x;
	        	        tTempPrevPoint.y=tTempCurPoint.y;
		        }
			cvShowImage("my cal bitmap", pTempRgbImg);
			
		}
	#endif
#endif
	}

	
	
	for(i=0; i<tInputLength; i++)
	{	
	//	filtered_data[i]=1000-(original_data[i]-10000)*1000/60000;
		filtered_data[i]=1000-(filtered_data[i]-10000)*1000/60000;
	//	filtered_data[i]=1000-(original_data[i]+30000-10000)*1000/60000;

	#if 0	
		if(tTempStartSingleIndex>0)
		{
		
			tTempTotalVal += original_data[i];
			if(j++>=tTempSampleCnt)
			{
				tTempAvgVal=tTempTotalVal/tTempSampleCnt;

				for(j=0; j<tTempSampleCnt; j++)
				{
					original_data[i+1-tTempSampleCnt+j] -= tTempAvgVal;


					printf("damon ===> %d - %d  %d \n", i+1-tTempSampleCnt+j, original_data[i+1-tTempSampleCnt+j], tTempAvgVal);
					
				/*	filtered_data[i+1-tTempSampleCnt+j]=1000-(original_data[i+1-tTempSampleCnt+j]+40000)*1000/60000;
					if(filtered_data[i+1-tTempSampleCnt+j]>=1000)
						filtered_data[i+1-tTempSampleCnt+j]=1000-1;
					if(filtered_data[i+1-tTempSampleCnt+j]<0)
						filtered_data[i+1-tTempSampleCnt+j]=0;*/
				}
				j=0;
				tTempTotalVal=0;
				printf("\n");
				sleep(10);
			}else if(i==(tInputLength-1))
			{
				int tTempStartIndex=tInputLength-(tInputLength%tTempSampleCnt);
				tTempAvgVal=tTempTotalVal/(tInputLength%tTempSampleCnt);
				for(j=0; j<tInputLength-tTempStartIndex; j++)
				{
					original_data[tTempStartIndex+j] -= tTempAvgVal;

					printf("damon ===> %d - %d  %d \n", tTempStartIndex+j, original_data[tTempStartIndex+j], tTempAvgVal);

					
				/*	filtered_data[i+1-tTempSampleCnt+j]=1000-(original_data[tTempStartIndex+j]+40000)*1000/60000;
					if(filtered_data[i+1-tTempSampleCnt+j]>=1000)
						filtered_data[i+1-tTempSampleCnt+j]=1000-1;
					if(filtered_data[i+1-tTempSampleCnt+j]<0)
						filtered_data[i+1-tTempSampleCnt+j]=0;*/
				}
				
				printf("\n");
			}
		}
	#endif
	}

}

#define M 5
void adaptLmsFilter(int *original_data, int *noise_data, int *filtered_data, int tInputLength)
{
	float w[M]={0};
	float buffer[M]={0};
	float u=0.00001;

	int i=0, j=0;

	for(i=0; i<tInputLength; i++)
	{
		noise_data[i] -= 660;
	}

	float e[tInputLength];
	float yn;

#if 0
	for(i=0; i<tInputLength; i++)
	{
		buffer[0]=noise_data[i];
		yn=0;
		for(j=0; j<M; j++)
		{
			yn += w[j]*buffer[M-1-j];
		}

		e[i]=original_data[i]-yn;
		filtered_data[i]=e[i];
	printf("damon ===> %d %d %d - %f \n", original_data[i], noise_data[i], filtered_data[i], yn);
	usleep(1000*300);
		for(j=0; j<M; j++)
			w[j] += (u*e[i]*buffer[i]);
		for(j=0; j<M-1; j++)
			buffer[j+1]=buffer[j];
	}
#else


	for(i=0; i<tInputLength; i++)
	{
		yn=0;
		for(j=0; j<M; j++)
		{
			buffer[j]=noise_data[i+M-j-1];
		}

		for(j=0; j<M; j++)
		{
			yn = yn+w[j]*buffer[j];
	//	printf("damon ===> %d : %f %f %f \n", j, w[j], buffer[j], yn);
		}

		e[i]=original_data[i]-yn;	
		filtered_data[i]=e[i];
//		filtered_data[i]=1000-(e[i]-10000)*1000/60000;
	printf("damon ===> %d - %d %d %d - %f \n", i, original_data[i], noise_data[i], filtered_data[i], yn);
	usleep(1000*500);
		for(j=0; j<M; j++)
		{
			w[j]=w[j]+2*u*e[i]*buffer[j];
		}
	}
#endif
}


void testLmsFilter()
{
#define PI                            3.141592654
#define Fs                            10000
#define Ts                            1/Fs
#define F1                           2
#define F2                           10
#define F3                           20
#define F4                           500

	float pTempSignal[200], pTempNoise[200], pTempSignalNoise[200];

	int i=0;
	for(i=0; i<200; i++)
	{
		pTempSignal[i]=sin(2*PI*F1*i*Ts) + 0.5*sin(2*PI*F2*i*Ts) + 0.25*sin(2*PI*F3*i*Ts);
		pTempNoise[i] = 5*sin(2*PI*F4*i*Ts+PI/2);
	        pTempSignalNoise[i] = pTempSignal[i] + 0.2*pTempNoise[i];
	        if(i>0)
	            pTempSignalNoise[i] += 0.15*pTempNoise[i-1];
	        if(i>1)
	            pTempSignalNoise[i] += 0.1*pTempNoise[i-2];
	}



	float w[M]={0};
	float buffer[M]={0};
	float u=0.00001;

	int j=0;

	float e[200];
	float yn;

	for(i=0; i<200; i++)
	{
		yn=0;
		for(j=0; j<M; j++)
		{
			buffer[j]=pTempNoise[i+M-j-1];
		}

		for(j=0; j<M; j++)
		{
			yn = yn+w[j]*buffer[j];
	//	printf("damon ===> %d : %f %f %f \n", j, w[j], buffer[j], yn);
		}

		e[i]=pTempSignalNoise[i]-yn;	
//		filtered_data[i]=1000-(e[i]-10000)*1000/60000;
	printf("damon ===> %d - %f %f %f - %f %f \n", i, pTempSignalNoise[i], pTempNoise[i], yn, e[i], pTempSignal[i]);
	usleep(1000*500);
		for(j=0; j<M; j++)
		{
			w[j]=w[j]+2*u*e[i]*buffer[j];
		}
	}

}

void draw_other_img(char *pInputWinName, int *pInputData, int tInputLength)
{
	int i=0, j=0;
	IplImage *pTempRgbImg=cvCreateImage(cvSize(tInputLength, 1000), IPL_DEPTH_8U, 3);
		memset(pTempRgbImg->imageData, 0, pTempRgbImg->widthStep*pTempRgbImg->height);
	unsigned char *pTempImgData=pTempRgbImg->imageData;
			int m,n, k=0;
			int tTempColor=0;
	unsigned char r, g, b;
		CvPoint tTempPrevPoint, tTempCurPoint;
	tTempPrevPoint.x=0;
		tTempPrevPoint.y=1000-(pInputData[0]-10000)*1000/60000;//pInputData[0];
printf("damon ===> cnt = %d \n", tXPointCnt);
	for(m=1; m<pTempRgbImg->width; m ++)
	{
		//	int tTempY=pTempOutData[m];
			tTempCurPoint.x=m;
		tTempCurPoint.y=1000-(pInputData[m]-10000)*1000/60000;//pInputData[m];
	//		cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8);

		cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);
//		printf("damon ====> point : %d %d \n", tTempCurPoint.x, tTempCurPoint.y);
			tTempPrevPoint.x=tTempCurPoint.x;
			tTempPrevPoint.y=tTempCurPoint.y;
	}
	
	cvShowImage(pInputWinName, pTempRgbImg);
}

void draw_hrp_img(int *pInputData, int tInputLength)
{
	int i=0, j=0;
	IplImage *pTempRgbImg=cvCreateImage(cvSize(tInputLength, 1000), IPL_DEPTH_8U, 3);
   		memset(pTempRgbImg->imageData, 0, pTempRgbImg->widthStep*pTempRgbImg->height);
	unsigned char *pTempImgData=pTempRgbImg->imageData;
	        int m,n, k=0;
			int tTempColor=0;
	unsigned char r, g, b;
    	CvPoint tTempPrevPoint, tTempCurPoint;
   	tTempPrevPoint.x=0;
    	tTempPrevPoint.y=pInputData[0];
printf("damon ===> cnt = %d \n", tXPointCnt);
    	for(m=1; m<pTempRgbImg->width; m ++)
    	{
	#if 0
            //	int tTempY=pTempOutData[m];
            	tTempCurPoint.x=m;
           	tTempCurPoint.y=pInputData[m];
    	//      cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8);

    		if(m>=635 && m<752 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 0), 1, 8, 0);
    		else if(m>=272 && m<307 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 0, 255), 1, 8, 0);
    		else if(m>=307 && m<343 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 255, 0), 1, 8, 0);
    		else if(m>=343 && m<379 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 255), 1, 8, 0);	
    		else if(m>=379 && m<412 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 255, 0), 1, 8, 0);	
    		else if(m>=412 && m<448 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 255), 1, 8, 0);	
		else
                	cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);
	//      printf("damon ====> point : %d %d \n", tTempCurPoint.x, tTempCurPoint.y);
    	        tTempPrevPoint.x=tTempCurPoint.x;
    	        tTempPrevPoint.y=tTempCurPoint.y;
	#else
		tTempCurPoint.x=m;
		tTempCurPoint.y=pInputData[m];
//		cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8);

		if(k<tXPointCnt-1 && m>=pXImgPoint[k])
		{

	//	printf("damon ===> k=%d , point=%d , color=%d \n", k, pXImgPoint[k], tTempColor);
		
	//	printf("damon ===> save cnt : %d %d %d %d %d \n", c, tTempSaveMinCnt, m, pTempSaveMinIndex[c], pTempSaveMinIndex[c+1]);
			r=(tTempColor%3==2)?255:0;//abs(rand()%255);
			g=(tTempColor%3==1)?255:0;
			b=(tTempColor%3==0)?255:0;
			tTempColor++;
			if(tTempColor>=3)
				tTempColor=0;
		
			for(i=pXImgPoint[k]; i<pXImgPoint[k+1]; i++)
			{
				tTempCurPoint.x=m;
				tTempCurPoint.y=pInputData[m];
				cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(r, g, b), 1, 8, 0);

				m++;
				
				tTempPrevPoint.x=tTempCurPoint.x;
				tTempPrevPoint.y=tTempCurPoint.y;
			}
			
			k++;
			m=pXImgPoint[k]-1;

		//	printf("damon ===> index : %d \n", m);
		}else
		{
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);

			
			tTempPrevPoint.x=tTempCurPoint.x;
			tTempPrevPoint.y=tTempCurPoint.y;
		}		
	#endif
        }
	
	cvShowImage("心率补偿图", pTempRgbImg);
	cvWaitKey(0);
}


int main(int argc, char *argv[])
{
	if(argc<3)
	{
		printf("damon ===> usage : ./a.out heart.dat gsensor.dat \n");
		return 0;
	}

	srand(time(NULL));

	int tTempHeartFd=open(argv[1], O_RDONLY);
	if(tTempHeartFd<0)
	{
		printf("damon ===> open file [%s] failed !\n", argv[1]);
		return 0;
	}

	int tTempGSensorFd=open(argv[2], O_RDONLY);
	if(tTempGSensorFd<0)
	{
		printf("damon ===> open file [%s] failed !\n", argv[2]);
		return 0;
	}

	int tTempHeartFileLen=lseek(tTempHeartFd, 0, SEEK_END);
	int tTempGSensorFileLen=lseek(tTempGSensorFd, 0, SEEK_END);
	char *pTempHeartBuf=malloc(tTempHeartFileLen+1);
	memset(pTempHeartBuf, 0, tTempHeartFileLen+1);
	char *pTempGSensorBuf=malloc(tTempGSensorFileLen+1);
	memset(pTempGSensorBuf, 0, tTempGSensorFileLen+1);

	lseek(tTempHeartFd, 0, SEEK_SET);
	lseek(tTempGSensorFd, 0, SEEK_SET);

	if(tTempHeartFileLen!=(read(tTempHeartFd, pTempHeartBuf, tTempHeartFileLen)))
	{
		printf("damon ===> read file [%s] failed !\n", argv[1]);
		close(tTempHeartFd);
		close(tTempGSensorFd);
		return 0;
	}
	
printf("damon ==> heart file len= %d \n", tTempHeartFileLen);

	if(tTempGSensorFileLen!=(read(tTempGSensorFd, pTempGSensorBuf, tTempGSensorFileLen)))
	{
		printf("damon ===> read file [%s] failed !\n", argv[1]);
		close(tTempHeartFd);
		close(tTempGSensorFd);
		return 0;
	}


	// 获取心率数据
	char *pTempStr=strrchr(pTempHeartBuf, ',');
	if(pTempStr==NULL)
	{
		close(tTempHeartFd);
		close(tTempGSensorFd);
		
		return 0;
	}
	int tTempSubIndex=pTempStr-pTempHeartBuf;
	pTempStr++;
	
	int i=0;
	char pTempNum[10]={0};
	while(pTempStr)
	{
		if(*pTempStr>='0' && *pTempStr<='9')		
		{
			pTempNum[i++]=*pTempStr;
		}else
		{
			break;
		}
		pTempStr++;
	}
	int tTempHeartDataNum=atoi(pTempNum);
	int *pTempHeartData=malloc(tTempHeartDataNum*sizeof(int));
	pTempHeartBuf[tTempSubIndex]=0;
	memset(pTempNum, 0, sizeof(pTempNum));
	int j=0, k=0;
	for(i=0; i<tTempSubIndex; i++)
	{
		if(pTempHeartBuf[i]>='0' && pTempHeartBuf[i]<='9')
		{
			pTempNum[j++]=pTempHeartBuf[i];
		}else
		{
			if(j>0)
			{
				if(k<tTempHeartDataNum)
					pTempHeartData[k++]=atoi(pTempNum);
				else
					printf("damon ===> data err: %d %d \n", k, tTempHeartDataNum);
			}
			j=0;
			memset(pTempNum, 0, sizeof(pTempNum));			
		}
	}


#ifndef dNoGSensorData
	// gsensor 数据
	pTempStr=strrchr(pTempGSensorBuf, ',');
	if(pTempStr==NULL)
	{
		close(tTempHeartFd);
		close(tTempGSensorFd);
		
		return 0;
	}
	tTempSubIndex=pTempStr-pTempGSensorBuf;
	pTempStr++;
	
	i=0;
	memset(pTempNum, 0, sizeof(pTempNum));
	while(pTempStr)
	{
		if(*pTempStr>='0' && *pTempStr<='9')		
		{
			pTempNum[i++]=*pTempStr;
		}else
		{
			break;
		}
		pTempStr++;
	}
	int tTempGSensorDataNum=atoi(pTempNum);
	if(tTempGSensorDataNum!=tTempHeartDataNum)
	{
		printf("damon ===> data num not equal !\n");
		return 0;
	}
	int *pTempGSensorData=malloc(tTempGSensorDataNum*sizeof(int));
	pTempGSensorBuf[tTempSubIndex]=0;
	memset(pTempNum, 0, sizeof(pTempNum));
	j=0; k=0;
	for(i=0; i<tTempSubIndex; i++)
	{
		if(pTempGSensorBuf[i]>='0' && pTempGSensorBuf[i]<='9')
		{
			pTempNum[j++]=pTempGSensorBuf[i];
		}else
		{
			if(j>0)
			{
				if(k<tTempGSensorDataNum)
					pTempGSensorData[k++]=atoi(pTempNum);
				else
					printf("damon ===> data err: %d %d \n", k, tTempGSensorDataNum);
			}
			j=0;
			memset(pTempNum, 0, sizeof(pTempNum));
		}
	}

	printf("damon ===> read data : %d %d \n", k, tTempGSensorDataNum);

	int *pTempFilterGSensorData=draw_gsensor_data(pTempGSensorData, tTempGSensorDataNum);
#endif

	int *pTempOutData=malloc(tTempHeartDataNum*sizeof(int));
#if 1

/*	int tTempNum=0;
	for(i=0; i<tTempGSensorDataNum; i+=tTempNum)
	{
		if(i+200<=tTempGSensorDataNum)
			tTempNum=200;
		else
			tTempNum=tTempGSensorDataNum-i;

		
		adaptLmsFilter(pTempHeartData+i, pTempFilterGSensorData+i, pTempOutData+i, tTempNum);
	}*/
	testLmsFilter();

	draw_other_img("after_lms_img", pTempOutData, tTempGSensorDataNum);
	
//	adaptLmsFilter(pTempHeartData, pTempFilterGSensorData, pTempOutData, tTempGSensorDataNum);
	
#ifndef dNoGSensorData
	MyTestFilter(pTempHeartData, pTempFilterGSensorData, pTempOutData, tTempHeartDataNum);
#else
	MyTestFilter(pTempHeartData, NULL, pTempOutData, tTempHeartDataNum);
#endif

	draw_hrp_img(pTempOutData, tTempHeartDataNum);
#elif 0
#ifndef dNoGSensorData
	test(pTempHeartData, pTempFilterGSensorData, tTempGSensorDataNum);
#else
	test(pTempHeartData, NULL, tTempHeartDataNum);
#endif
	for(i=0; i<tTempHeartDataNum; i++)
		pTempOutData[i]=1000-(pTempHeartData[i]-10000)*1000/60000;
	draw_hrp_img(pTempOutData, tTempHeartDataNum);
#else
	// 动态补偿后的心率图
	{
	int tTempSampleCnt=(1000/20)*60/60;
		IplImage *pTempRgbImg=cvCreateImage(cvSize(tTempGSensorDataNum, 1000), IPL_DEPTH_8U, 3);
       		memset(pTempRgbImg->imageData, 0, pTempRgbImg->widthStep*pTempRgbImg->height);
		unsigned char *pTempImgData=pTempRgbImg->imageData;
 	        int m,n, k=0;
		unsigned char r, g, b;
        	CvPoint tTempPrevPoint, tTempCurPoint;
       		tTempPrevPoint.x=0;
        	tTempPrevPoint.y=pTempOutData[0];
	printf("damon ===> cnt = %d \n", tXPointCnt);
        	for(m=1; m<pTempRgbImg->width; m ++)
        	{
        	#if 0
                //	int tTempY=pTempOutData[m];
                	tTempCurPoint.x=m;
               		tTempCurPoint.y=pTempOutData[m];
        	//      cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8);
        		if(m>=1317 && m<1317+200 /*tTempSampleCnt+260*/)
				cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 0), 1, 8, 0);
        		else if(m>=440 && m<478 /*tTempSampleCnt+260*/)
				cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 0, 255), 1, 8, 0);
        		else if(m>=478 && m<520 /*tTempSampleCnt+260*/)
				cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 255, 0), 1, 8, 0);
        		else if(m>=520 && m<562 /*tTempSampleCnt+260*/)
				cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 255), 1, 8, 0);	
			else
	                	cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);
		//      printf("damon ====> point : %d %d \n", tTempCurPoint.x, tTempCurPoint.y);
        	        tTempPrevPoint.x=tTempCurPoint.x;
        	        tTempPrevPoint.y=tTempCurPoint.y;
		#else
			tTempCurPoint.x=m;
			tTempCurPoint.y=pTempOutData[m];
	//		cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8);

		/*	if(k<tXPointCnt-1 && m>=pXImgPoint[k])
			{
			
		//	printf("damon ===> save cnt : %d %d %d %d %d \n", c, tTempSaveMinCnt, m, pTempSaveMinIndex[c], pTempSaveMinIndex[c+1]);
				r=abs(rand()%255);
				g=abs(rand()%255);
				b=abs(rand()%255);
			
				for(i=pXImgPoint[k]; i<pXImgPoint[k+1]; i++)
				{
					tTempCurPoint.x=m;
					tTempCurPoint.y=pTempOutData[m];
					cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(r, g, b), 1, 8, 0);

					m++;
					
					tTempPrevPoint.x=tTempCurPoint.x;
					tTempPrevPoint.y=tTempCurPoint.y;
				}
				
				k++;
				m=pXImgPoint[k]-1;

				printf("damon ===> index : %d \n", m);
			}else*/
			{
				cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);

				
				tTempPrevPoint.x=tTempCurPoint.x;
				tTempPrevPoint.y=tTempCurPoint.y;
			}		
		#endif
	        }
		
		cvShowImage("心率补偿图", pTempRgbImg);
	}
#endif

	cvWaitKey(0);
	while(1)
	{
		sleep(2);
	}

	return 0;
}







