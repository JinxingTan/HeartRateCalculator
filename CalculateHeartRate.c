

#include "CalculateHeartRate.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



typedef struct{
	int heartData;
	int gsensorData;
	int index;
}MyHeartData;

#define dXArrayMax	200

MyHeartData *pXHeartData=NULL;

int tXHeartHz=4;

int tXTotalDataCnt=0;
int tXDataHead=0, tXDataTail=0;
int tXStartIndex=0;

typedef struct{
	int up;
	int down;
	int falseValue;
	HeartValue pMinValue[7];
	HeartValue pMaxValue[7];
	int minIdx;
	int maxIdx;
	int prevValue;
}WaveParam;
WaveParam *pXWaveParam=NULL;


int *pXTestData=NULL;
int tXTestCnt=0;

int FindStartPoint()
{
	// find first spie
	int i=0, j=0;
	char tTempUp=0, tTempDown=0, tTempFalseValue=0, tTempFindFirstLow=0;
	HeartValue pTempMinHeartVal[7];
	HeartValue pTempMaxHeartVal[7];
	int tTempMinIdx=0, tTempMaxIdx=0;
	int tTempPrevVal;
	int tTempCurVal=0;
	int tTempHeadIdx=0, tTempTailIdx=0;
	tTempHeadIdx=(tXDataHead)%dXArrayMax;
	tTempTailIdx=tXDataTail%dXArrayMax;
	tTempPrevVal= pXHeartData[tTempHeadIdx].heartData;//original_data[0];
	unsigned int tTempTotalHeartValue=0;
	int tTempTotalHeartCnt=0;
//printf("damon ===> find point = %d %d \n", tTempHeadIdx, tTempTailIdx);
	do{
		i++;
		tTempHeadIdx=(tXDataHead+i)%dXArrayMax;
		tTempCurVal= pXHeartData[tTempHeadIdx].heartData;//original_data[i];
//	printf("damon ===> index = %d %d \n", tTempHeadIdx, pXHeartData[tTempHeadIdx].index);
//	sleep(1);

	//	tTempTotalHeartValue += tTempCurVal;
	//	tTempTotalHeartCnt++;
		
		if(tTempCurVal>tTempPrevVal)
		{
			if(tTempUp==0 && tTempDown==0)
			{
				pTempMinHeartVal[tTempMinIdx].index=pXHeartData[tTempHeadIdx].index;
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
						if(pXHeartData[tTempHeadIdx].index-pTempMaxHeartVal[tTempMaxIdx-1].index>8)
							printf("damon ===> up adjuest value max val =  %d %d -%d %d \n", pXHeartData[tTempHeadIdx].index, tTempCurVal, pTempMaxHeartVal[tTempMaxIdx-1].index, pTempMaxHeartVal[tTempMaxIdx-1].value);
					
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
							if(pXHeartData[tTempHeadIdx].index-pTempMinHeartVal[tTempMinIdx-1].index>100)
								tTempMinIdx=0;
						
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
					
						pTempMinHeartVal[tTempMinIdx].index=pXHeartData[tTempHeadIdx].index;
						pTempMinHeartVal[tTempMinIdx].value=tTempPrevVal;
						tTempMinIdx++;
					}
				}else if((pXHeartData[tTempHeadIdx].index-pTempMinHeartVal[tTempMinIdx-1].index>8) && (tTempCurVal>pTempMinHeartVal[tTempMinIdx-1].value))
				{
					tTempDown=0;
					tTempUp=1;
					tTempFalseValue=0;

					if(tTempMinIdx>=5)
					{
						tXStartIndex=pTempMinHeartVal[0].index;
						return tTempHeadIdx;
					}

				}

				tTempPrevVal=tTempCurVal;
			}

		}else if(tTempCurVal < tTempPrevVal)
		{
			if(tTempUp==0 && tTempDown==0)
			{//printf("damon ===> start max idx : %d %d %d \n", tTempMaxIdx, i, tTempPrevVal);
				pTempMaxHeartVal[tTempMaxIdx].index=pXHeartData[tTempHeadIdx].index;
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
						if(pXHeartData[tTempHeadIdx].index-pTempMinHeartVal[tTempMinIdx-1].index>8)
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
						// filter value
						if(tTempMaxIdx>=2)
						{
							if(pXHeartData[tTempHeadIdx].index-pTempMaxHeartVal[tTempMaxIdx-1].index>100)
								tTempMaxIdx=0;

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
					
						pTempMaxHeartVal[tTempMaxIdx].index=pXHeartData[tTempHeadIdx].index;
						pTempMaxHeartVal[tTempMaxIdx].value=tTempPrevVal;
						tTempMaxIdx++;		
				//	printf("damon ===> max idx = %d \n", tTempMaxIdx);
					}
				}else if((pXHeartData[tTempHeadIdx].index-pTempMaxHeartVal[tTempMaxIdx-1].index>8) && (tTempCurVal<pTempMaxHeartVal[tTempMaxIdx-1].value))
				{
					tTempUp=0;
					tTempDown=1;
					tTempFalseValue=0;


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

	}while(tTempHeadIdx!=tTempTailIdx);

	return 0;
}


int AdjuestWithGSensorHz(int tInputHeartHz)
{
	int i=0;
	int tTempUp=0;
	int tTempDown=0;
	int tTempFalseValue=0;
	int tTempMinIdx=0;
	int tTempMaxIdx=0;
	HeartValue pTempMaxSportData[5]={0};
	HeartValue pTempMinSportData[5]={0};

	HeartValue pTempSaveMinVal[20]={0};
	int tTempSaveCnt=0;

	unsigned int tTempTotalSportVal=0;
	int tTempTotalSportCnt=0;

	int tTempPrevVal=0, tTempCurVal=0, tTempAvgVal=0;
	int tTempHeadIdx=0, tTempTailIdx=0;
	tTempHeadIdx=(tXDataHead)%dXArrayMax;
	tTempTailIdx=tXDataTail%dXArrayMax;
	tTempPrevVal= pXHeartData[tTempHeadIdx].gsensorData;//original_data[0];
	do
	{
		i++;
		tTempHeadIdx=(tXDataHead+i)%dXArrayMax;
		tTempCurVal= pXHeartData[tTempHeadIdx].gsensorData;//original_data[i];


		tTempTotalSportVal += tTempCurVal;
		tTempTotalSportCnt++;
		tTempAvgVal=tTempTotalSportVal/tTempTotalSportCnt;

		if(tTempCurVal>tTempPrevVal)
		{
			if(tTempUp==0 && tTempDown==0)
			{
				tTempUp=1;
				pTempMinSportData[tTempMinIdx].index=pXHeartData[tTempHeadIdx].index;
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
					}else if(pXHeartData[tTempHeadIdx].index-pTempMaxSportData[tTempMaxIdx-1].index>10 && tTempCurVal-tTempAvgVal>10)
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

					pTempMinSportData[tTempMinIdx].index=pXHeartData[tTempHeadIdx].index;
					pTempMinSportData[tTempMinIdx].value=tTempPrevVal;
					tTempMinIdx++;

					
					tTempTotalSportVal=0;
					tTempTotalSportCnt=0;

				}else if(pXHeartData[tTempHeadIdx].index-pTempMinSportData[tTempMinIdx-1].index>5)
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

				#if 0
					if(tTempMinIdx>=3)
					{
						// cal Hz
						
						printf("damon ===> cal : (%d , %d), (%d, %d) \n", pTempMinSportData[0].index, pTempMinSportData[0].value,
												pTempMinSportData[1].index, pTempMinSportData[1].value);

						pTempMinSportData[0].index=pTempMinSportData[1].index;
						pTempMinSportData[0].value=pTempMinSportData[1].value;							
						pTempMinSportData[1].index=pTempMinSportData[2].index;
						pTempMinSportData[1].value=pTempMinSportData[2].value;
						tTempMinIdx -= 1;
					}
				#else
					if(tTempMinIdx>=3)
					{
						pTempSaveMinVal[tTempSaveCnt].index=pTempMinSportData[0].index;
						pTempSaveMinVal[tTempSaveCnt++].value=pTempMinSportData[0].value;
					printf("damon ===> (%d, %d) \n", pTempMinSportData[0].index, pTempMinSportData[0].value);
						
						pTempMinSportData[0].index=pTempMinSportData[1].index;
						pTempMinSportData[0].value=pTempMinSportData[1].value;							
						pTempMinSportData[1].index=pTempMinSportData[2].index;
						pTempMinSportData[1].value=pTempMinSportData[2].value;
						tTempMinIdx -= 1;
					}
				#endif
				}
			}
		}else
		{
			if(tTempUp==0 && tTempDown==0)
			{
				tTempDown=1;
				pTempMaxSportData[tTempMaxIdx].index=pXHeartData[tTempHeadIdx].index;
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
					}else if(pXHeartData[tTempHeadIdx].index-pTempMinSportData[tTempMinIdx-1].index>10 && tTempAvgVal-tTempCurVal>10)
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

					pTempMaxSportData[tTempMaxIdx].index=pXHeartData[tTempHeadIdx].index;
					pTempMaxSportData[tTempMaxIdx].value=tTempPrevVal;
					tTempMaxIdx++;

					
					tTempTotalSportVal=0;
					tTempTotalSportCnt=0;
				}else if(pXHeartData[tTempHeadIdx].index-pTempMaxSportData[tTempMaxIdx-1].index>5)
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
	}while(tTempHeadIdx!=tTempTailIdx);

	printf("damon ===> adjuest value : %d %d \n", tInputHeartHz, tTempSaveCnt);
	if(tTempSaveCnt<=0)
		return 0;


	i=0;
	tTempHeadIdx=(tXDataHead)%dXArrayMax;
	int tTempFirstVal=0, tTempToTalData=0;
	int tTempWindowWidth=200/tTempSaveCnt;

	tTempFirstVal=pXHeartData[tTempHeadIdx].heartData;
	for(i=0; i<tTempWindowWidth; i++)
	{
		tTempToTalData += pXHeartData[tTempHeadIdx].heartData;
	
		tTempHeadIdx=(tXDataHead+i)%dXArrayMax;		
	}

	float A=0.0, x=0.0;
	x=(float)tInputHeartHz/tTempSaveCnt;
	A=pow(x,3)-pow(x, 2)+1.01;
printf("damon ===> x=%f, A=%f \n", x, A);
	for(i=0; i<200-tTempWindowWidth; i++)
	{
		tTempHeadIdx=(tXDataHead+i)%dXArrayMax;
		tTempFirstVal=pXHeartData[tTempHeadIdx].heartData;
		if(pXHeartData[tTempHeadIdx].index>=pTempSaveMinVal[0].index && pXHeartData[tTempHeadIdx].index<pTempSaveMinVal[tTempSaveCnt-1].index)
			pXHeartData[tTempHeadIdx].heartData=(tTempToTalData/tTempWindowWidth)/A;


		tTempToTalData=tTempToTalData-tTempFirstVal+pXHeartData[(tXDataHead+i+tTempWindowWidth)%dXArrayMax].heartData;
	}

	return tTempWindowWidth;

//	return 0;
}

void GetHeartRate(int tInputLength, int *pOutputRate)
{
	static int tTempFirst=0;
	int tTempCurVal=0;
	int i=0;

printf("damon ===> get heart rate : %d %d %d \n", tInputLength, pXWaveParam->up, pXWaveParam->down);
	if(pOutputRate)
		*pOutputRate=0;

	for(i=0; i<tInputLength; i++)
	{
	
		int tTempHeadIdx=(tXDataHead+i)%dXArrayMax;

		// test data 
		pXTestData[tXTestCnt++]=pXHeartData[tTempHeadIdx].heartData;

		if(tTempFirst==0)
		{
			pXWaveParam->prevValue=pXHeartData[tTempHeadIdx].heartData;
			tTempFirst=1;
			continue;
		}

		tTempCurVal= pXHeartData[tTempHeadIdx].heartData;
//	printf("damon ===> %d %d - %d %d \n", tTempHeadIdx, pXHeartData[tTempHeadIdx].index, pXWaveParam->prevValue, tTempCurVal);
//	sleep(1);

	//	tTempTotalHeartValue += tTempCurVal;
	//	tTempTotalHeartCnt++;
		
		if(tTempCurVal>pXWaveParam->prevValue)
		{
			if(pXWaveParam->up==0 && pXWaveParam->down==0)
			{
				pXWaveParam->pMinValue[pXWaveParam->minIdx].index=pXHeartData[tTempHeadIdx].index;
				pXWaveParam->pMinValue[pXWaveParam->minIdx].value=pXWaveParam->prevValue;
				pXWaveParam->minIdx++;
				pXWaveParam->prevValue=tTempCurVal;
				pXWaveParam->up=1;
			}else if(pXWaveParam->up==1)
			{
				if(pXWaveParam->falseValue>0)
				{
					if(pXWaveParam->maxIdx<=0)
						printf("damon ===> max idx error : %d \n", pXWaveParam->maxIdx);
				
					if( tTempCurVal>pXWaveParam->pMaxValue[pXWaveParam->maxIdx-1].value /*|| (i-pTempMaxHeartVal[tTempMaxIdx-1].index>8)*/)
					{
						if(pXHeartData[tTempHeadIdx].index-pXWaveParam->pMaxValue[pXWaveParam->maxIdx-1].index>8)
							printf("damon ===> up adjuest value max val =  %d %d -%d %d \n", pXHeartData[tTempHeadIdx].index, tTempCurVal, pXWaveParam->pMaxValue[pXWaveParam->maxIdx-1].index, pXWaveParam->pMaxValue[pXWaveParam->maxIdx-1].value );

						pXWaveParam->up=1;
						pXWaveParam->down=0;
						pXWaveParam->maxIdx--;
						pXWaveParam->falseValue=0;
					}
				}
			
				pXWaveParam->prevValue=tTempCurVal;
			}else if(pXWaveParam->down==1)
			{
				if(pXWaveParam->falseValue==0)
				{
					pXWaveParam->falseValue=1;

		//		printf("damon ===> min idx 00 = %d \n", pXWaveParam->minIdx);
					if(pXWaveParam->minIdx<7)
					{

				#if 1
						// filter  max value
						if(pXWaveParam->minIdx>=2)
						{
							if((pXHeartData[tTempHeadIdx].index-pXWaveParam->pMinValue[pXWaveParam->minIdx-1].value>100)
								|| pXWaveParam->pMinValue[pXWaveParam->minIdx-1].value<12000)
							{
								pXWaveParam->minIdx=0;
								pXWaveParam->maxIdx=0;

								pXWaveParam->falseValue=0;
								pXWaveParam->up=0;
								pXWaveParam->down=0;
							}
						
							int m=0, n=0;
							for(m=0; m<pXWaveParam->minIdx-1; m++)
							{
								if(pXWaveParam->pMinValue[m].value>45000)
								{
									for(n=m; n<pXWaveParam->minIdx-1; n++)
									{
										pXWaveParam->pMinValue[n].index=pXWaveParam->pMinValue[n+1].index;
										pXWaveParam->pMinValue[n].value=pXWaveParam->pMinValue[n+1].value;
									}

									m--;
									pXWaveParam->minIdx--;
								}else
								{
									if(/*m>0 &&*/ m<pXWaveParam->minIdx-2)
									{
										if(pXWaveParam->pMinValue[m+2].index-pXWaveParam->pMinValue[m+1].index>100)
										{//printf("damon ===> error delete max value %d : %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index);
											for(n=0; n<m+2; n++)
											{
												if(n+m+1<pXWaveParam->minIdx)
												{
													pXWaveParam->pMinValue[n].index=pXWaveParam->pMinValue[n+m+2].index;
													pXWaveParam->pMinValue[n].value=pXWaveParam->pMinValue[n+m+2].value;
												}
											}
											pXWaveParam->minIdx=pXWaveParam->minIdx-(m+1);
											m=0;
										//	printf("damon ===> value : %d \n", tTempMaxIdx);
										}else
										if(((pXWaveParam->pMinValue[m+1].index-pXWaveParam->pMinValue[m].index<20) 
											|| (pXWaveParam->pMinValue[m+1].index-pXWaveParam->pMinValue[m].index>80)) 
											&& (pXWaveParam->pMinValue[m+2].index-pXWaveParam->pMinValue[m+1].index>20))
										{

											for(n=m; n<pXWaveParam->minIdx-1; n++)
											{
												pXWaveParam->pMinValue[n].index=pXWaveParam->pMinValue[n+1].index;
												pXWaveParam->pMinValue[n].value=pXWaveParam->pMinValue[n+1].value;
											}
											
											m--;
											pXWaveParam->minIdx--;
										}else if((pXWaveParam->pMinValue[m+1].index-pXWaveParam->pMinValue[m].index>20) 
											&& ((pXWaveParam->pMinValue[m+2].index-pXWaveParam->pMinValue[m+1].index<20) 
											|| (pXWaveParam->pMinValue[m+2].index-pXWaveParam->pMinValue[m+1].index>80)))
										{
								//		printf("damon ==> error delete value %d : %d %d %d \n", tTempMinIdx, pTempMinHeartVal[m].index, pTempMinHeartVal[m+1].index, pTempMinHeartVal[m+2].index);
											for(n=m+2; n<pXWaveParam->minIdx-1; n++)
											{
												pXWaveParam->pMinValue[n].index=pXWaveParam->pMinValue[n+1].index;
												pXWaveParam->pMinValue[n].value=pXWaveParam->pMinValue[n+1].value;
											}
											
											m+=2;
											pXWaveParam->minIdx--;
										}else if(((pXWaveParam->pMinValue[m+1].index-pXWaveParam->pMinValue[m].index<20) 
											|| (pXWaveParam->pMinValue[m+1].index-pXWaveParam->pMinValue[m].index>80)) 
											&& ((pXWaveParam->pMinValue[m+2].index-pXWaveParam->pMinValue[m+1].index<20) 
											|| (pXWaveParam->pMinValue[m+2].index-pXWaveParam->pMinValue[m+1].index>80)))
										{
											for(n=m; n<pXWaveParam->minIdx-2; n++)
											{
												pXWaveParam->pMinValue[n].index=pXWaveParam->pMinValue[n+2].index;
												pXWaveParam->pMinValue[n].value=pXWaveParam->pMinValue[n+2].value;
											}
											
											m--;
											pXWaveParam->minIdx-=2; 										
										}
									}
								}
							}							
						}
			#endif


						printf("damon ===> min idx 11 = %d \n", pXWaveParam->minIdx);
					
						pXWaveParam->pMinValue[pXWaveParam->minIdx].index=pXHeartData[tTempHeadIdx].index;
						pXWaveParam->pMinValue[pXWaveParam->minIdx].value=pXWaveParam->prevValue;
						pXWaveParam->minIdx++;
					}
				}else if((pXHeartData[tTempHeadIdx].index-pXWaveParam->pMinValue[pXWaveParam->minIdx-1].index>8) 
					&& (tTempCurVal>pXWaveParam->pMinValue[pXWaveParam->minIdx-1].value))
				{
					pXWaveParam->down=0;
					pXWaveParam->up=1;
					pXWaveParam->falseValue=0;

			
					if(pXWaveParam->minIdx>=7)
					{
						printf("damon ===> min : (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d) (%d, %d) (%d, %d)\n", 
							pXWaveParam->pMinValue[0].index, pXWaveParam->pMinValue[0].value,
							pXWaveParam->pMinValue[1].index, pXWaveParam->pMinValue[1].value,
							pXWaveParam->pMinValue[2].index, pXWaveParam->pMinValue[2].value,
							pXWaveParam->pMinValue[3].index, pXWaveParam->pMinValue[3].value,
							pXWaveParam->pMinValue[4].index, pXWaveParam->pMinValue[4].value,
							pXWaveParam->pMinValue[5].index, pXWaveParam->pMinValue[5].value,
							pXWaveParam->pMinValue[6].index, pXWaveParam->pMinValue[6].value);


						int m=0;
						int tTempPointCount=pXWaveParam->pMinValue[5].index-pXWaveParam->pMinValue[0].index;
						int tTempMaxPoint=0, tTempMinPoint=1000;
						for(m=0; m<5; m++)
						{
							int tTempSubIdx=pXWaveParam->pMinValue[m+1].index-pXWaveParam->pMinValue[m].index;
							if(tTempSubIdx>tTempMaxPoint)
								tTempMaxPoint=tTempSubIdx;
							if(tTempSubIdx<tTempMinPoint)
								tTempMinPoint=tTempSubIdx;
						}
						// delete max and min value 
						tTempPointCount -= (tTempMaxPoint+tTempMinPoint);
						int tTempRate=(60*1000)/((tTempPointCount*20)/3);
						tXHeartHz=(200*3)/tTempPointCount;
						for(m=0; m<5; m++)
						{
//						pXImgPoint[tXPointCnt++]=pXWaveParam->pMinValue[m].index;

							if(m+5<7)
							{
								pXWaveParam->pMinValue[m].index=pXWaveParam->pMinValue[m+5].index;
								pXWaveParam->pMinValue[m].value=pXWaveParam->pMinValue[m+5].value;

								pXWaveParam->pMinValue[m+5].index=0;
								pXWaveParam->pMinValue[m+5].value=0;
							}else
							{
								pXWaveParam->pMinValue[m].index=0;
								pXWaveParam->pMinValue[m].value=0;
							}
						}
						pXWaveParam->minIdx-=5;

						printf("damon ===> heart rate : %d	%d \n", tTempRate, pXWaveParam->minIdx);
						if(pOutputRate)
							*pOutputRate=tTempRate;
					//	goto start_ok;

					}
				}

				pXWaveParam->prevValue=tTempCurVal;
			}

		}else if(tTempCurVal < pXWaveParam->prevValue)
		{
			if(pXWaveParam->up==0 && pXWaveParam->down==0)
			{//printf("damon ===> start max idx : %d %d %d \n", tTempMaxIdx, i, tTempPrevVal);
				pXWaveParam->pMaxValue[pXWaveParam->maxIdx].index=i;
				pXWaveParam->pMaxValue[pXWaveParam->maxIdx].value=pXWaveParam->prevValue;
				pXWaveParam->maxIdx++;
				pXWaveParam->prevValue=tTempCurVal;
				pXWaveParam->down=1;
			}else if(pXWaveParam->down==1)
			{
				if(pXWaveParam->falseValue>0)
				{
					if(pXWaveParam->minIdx<=0)
						printf("damon ===> min idx error : %d \n", pXWaveParam->minIdx);

					if(tTempCurVal<pXWaveParam->pMinValue[pXWaveParam->minIdx-1].value /*|| (i-pTempMinHeartVal[tTempMinIdx-1].index>8)*/)
					{
						if(i-pXWaveParam->pMinValue[pXWaveParam->minIdx-1].index>8)
							printf("damon ===> down adjuest value min value = %d %d \n", pXWaveParam->pMinValue[pXWaveParam->minIdx-1].index, pXWaveParam->pMinValue[pXWaveParam->minIdx-1].value);

					printf("damon ===> min idx 111 = %d \n", pXWaveParam->minIdx);
						pXWaveParam->up=0;
						pXWaveParam->down=1;
						pXWaveParam->minIdx--;
					
						pXWaveParam->falseValue=0;
					}					
				}
				
				pXWaveParam->prevValue=tTempCurVal;
			}else if(pXWaveParam->up==1)
			{
				if(pXWaveParam->falseValue==0)
				{
					pXWaveParam->falseValue=1;
			//	printf("damon ===> max idx : %d \n", tTempMaxIdx);
					if(pXWaveParam->maxIdx<7)
					{
						// filter value
						if(pXWaveParam->maxIdx>=2)
						{
							if(i-pXWaveParam->pMaxValue[pXWaveParam->maxIdx-1].index>100 
								|| pXWaveParam->pMaxValue[pXWaveParam->maxIdx-1].value>50000)
							{
								pXWaveParam->maxIdx=0;
								pXWaveParam->minIdx=0;

								pXWaveParam->falseValue=0;
								pXWaveParam->up=0;
								pXWaveParam->down=0;
							}
						
							int m=0, n=0;
							for(m=0; m<pXWaveParam->maxIdx-1; m++)
							{
								if(pXWaveParam->pMaxValue[m].value<15000)
								{
									for(n=m; n<pXWaveParam->maxIdx-1; n++)
									{
										pXWaveParam->pMaxValue[n].index=pXWaveParam->pMaxValue[n+1].index;
										pXWaveParam->pMaxValue[n].value=pXWaveParam->pMaxValue[n+1].value;
									}

									m--;
									pXWaveParam->maxIdx--;
								}else
								{

									if(/*m>0 &&*/ m<pXWaveParam->maxIdx-2)
									{
									
										if(pXWaveParam->pMaxValue[m+2].index-pXWaveParam->pMaxValue[m+1].index>100)
										{//printf("damon ===> error delete max value %d : %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index);
											for(n=0; n<m+2; n++)
											{
												if(n+m+1<pXWaveParam->maxIdx)
												{
													pXWaveParam->pMaxValue[n].index=pXWaveParam->pMaxValue[n+m+2].index;
													pXWaveParam->pMaxValue[n].value=pXWaveParam->pMaxValue[n+m+2].value;
												}
											}
											
											pXWaveParam->maxIdx=pXWaveParam->maxIdx-(m+1);
											m=0;
										//	printf("damon ===> value : %d \n", tTempMaxIdx);
										}else
										if(((pXWaveParam->pMaxValue[m+1].index-pXWaveParam->pMaxValue[m].index<20) 
											|| (pXWaveParam->pMaxValue[m+1].index-pXWaveParam->pMaxValue[m].index>80))
											&& (pXWaveParam->pMaxValue[m+2].index-pXWaveParam->pMaxValue[m+1].index>20))
										{										
								//		printf("damon ==> delete max value %d : %d %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index, pTempMaxHeartVal[m+2].index);
											for(n=m; n<pXWaveParam->maxIdx-1; n++)
											{
												pXWaveParam->pMaxValue[n].index=pXWaveParam->pMaxValue[n+1].index;
												pXWaveParam->pMaxValue[n].value=pXWaveParam->pMaxValue[n+1].value;
											}
											
											m--;
											pXWaveParam->maxIdx--;
										}else if((pXWaveParam->pMaxValue[m+1].index-pXWaveParam->pMaxValue[m].index>20) 
											&& ((pXWaveParam->pMaxValue[m+2].index-pXWaveParam->pMaxValue[m+1].index<20) 
											|| (pXWaveParam->pMaxValue[m+2].index-pXWaveParam->pMaxValue[m+1].index>80)))
										{
											for(n=m+2; n<pXWaveParam->maxIdx-1; n++)
											{
												pXWaveParam->pMaxValue[n].index=pXWaveParam->pMaxValue[n+1].index;
												pXWaveParam->pMaxValue[n].value=pXWaveParam->pMaxValue[n+1].value;
											}
											
											m+=2;
											pXWaveParam->maxIdx--;
										}else if(((pXWaveParam->pMaxValue[m+1].index-pXWaveParam->pMaxValue[m].index<20) 
											|| (pXWaveParam->pMaxValue[m+1].index-pXWaveParam->pMaxValue[m].index>80) ) 
											&& ((pXWaveParam->pMaxValue[m+2].index-pXWaveParam->pMaxValue[m+1].index<20) 
											|| (pXWaveParam->pMaxValue[m+2].index-pXWaveParam->pMaxValue[m+1].index>80)))
										{
								//			printf("damon ==> 2 delete max value %d : %d %d %d \n", tTempMaxIdx, pTempMaxHeartVal[m].index, pTempMaxHeartVal[m+1].index, pTempMaxHeartVal[m+2].index);										
											for(n=m; n<pXWaveParam->maxIdx-2; n++)
											{
												pXWaveParam->pMaxValue[n].index=pXWaveParam->pMaxValue[n+2].index;
												pXWaveParam->pMaxValue[n].value=pXWaveParam->pMaxValue[n+2].value;
											}
											
											m--;
											pXWaveParam->maxIdx-=2;
										}
									}
								}
							}							
						}
					
						pXWaveParam->pMaxValue[pXWaveParam->maxIdx].index=i;
						pXWaveParam->pMaxValue[pXWaveParam->maxIdx].value=pXWaveParam->prevValue;
						pXWaveParam->maxIdx++;
				//	printf("damon ===> max idx = %d \n", tTempMaxIdx);
					}
				}else if((pXHeartData[tTempHeadIdx].index-pXWaveParam->pMaxValue[pXWaveParam->maxIdx-1].index>8) 
					&& (tTempCurVal<pXWaveParam->pMaxValue[pXWaveParam->maxIdx-1].value))
				{
					pXWaveParam->up=0;
					pXWaveParam->down=1;
					pXWaveParam->falseValue=0;


				//	printf("damon ===> cal max val : %d \n", tTempMaxIdx);
					if(pXWaveParam->maxIdx>=7)
					{
						printf("damon ===> max : (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d) (%d, %d) (%d, %d)\n", 
							pXWaveParam->pMaxValue[0].index, pXWaveParam->pMaxValue[0].value,
							pXWaveParam->pMaxValue[1].index, pXWaveParam->pMaxValue[1].value,
							pXWaveParam->pMaxValue[2].index, pXWaveParam->pMaxValue[2].value,
							pXWaveParam->pMaxValue[3].index, pXWaveParam->pMaxValue[3].value,
							pXWaveParam->pMaxValue[4].index, pXWaveParam->pMaxValue[4].value,
							pXWaveParam->pMaxValue[5].index, pXWaveParam->pMaxValue[5].value,
							pXWaveParam->pMaxValue[6].index, pXWaveParam->pMaxValue[6].value);

						int m=0;
						for(m=0; m<5; m++)
						{
							if(m+5<7)
							{
								pXWaveParam->pMaxValue[m].index=pXWaveParam->pMaxValue[m+5].index;
								pXWaveParam->pMaxValue[m].value=pXWaveParam->pMaxValue[m+5].value;

								pXWaveParam->pMaxValue[m+5].index=0;
								pXWaveParam->pMaxValue[m+5].value=0;
							}else
							{
								pXWaveParam->pMaxValue[m].index=0;
								pXWaveParam->pMaxValue[m].value=0;
							}
						}
						pXWaveParam->maxIdx-=5;
					}
				}

				pXWaveParam->prevValue=tTempCurVal;
			}
		}
	}
}

void CalHeartRate(int tInputHeartData, int tInputGSensorata)
{
	int tTempSkipCnt=0;

	if(pXHeartData==NULL)
		pXHeartData=(MyHeartData *)malloc(dXArrayMax*sizeof(MyHeartData));
	if(pXWaveParam==NULL)
	{
		pXWaveParam=(WaveParam *)malloc(sizeof(WaveParam));
		memset((void *)pXWaveParam, 0, sizeof(WaveParam));
	}

	int tTempIdx=tXDataTail%dXArrayMax;
	pXHeartData[tTempIdx].heartData=tInputHeartData;
	pXHeartData[tTempIdx].gsensorData=tInputGSensorata;
	pXHeartData[tTempIdx].index=tXTotalDataCnt++;
	tXDataTail=(tXDataTail+1)%dXArrayMax;
//printf("damon ===> cal heart rate : %d %d \n", tXDataHead, tXDataTail);
	if(tXDataTail==tXDataHead)		// queue is full
	{
//	printf("damon ===> cal heart rate !\n");
		// cal heart rate 
		if(tXStartIndex==0)
		{
			int tTempHeadIdx=FindStartPoint();
			if(tTempHeadIdx>0)
			{
				tXDataHead=tTempHeadIdx;
				printf("damon ===> find first spie idx = %d %d \n", tXStartIndex, tTempHeadIdx);
			}else
			{
				tXDataHead=(tXDataHead+20)%dXArrayMax;
			}
		}else 
		{
			int tTempRet=AdjuestWithGSensorHz(tXHeartHz);
			if(tTempRet==0)
				tTempSkipCnt = 120;
			else
				tTempSkipCnt = dXArrayMax-tTempRet; 
		printf("damon ===> skip cnt = %d \n", tTempSkipCnt);
			int tTempHeartRate=0;
			GetHeartRate(tTempSkipCnt, &tTempHeartRate);
			tXDataHead=(tXDataHead+tTempSkipCnt)%dXArrayMax;
		}
	}
}


void drawTestImg(int *pInputData, int tInputLength)
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
    	for(m=1; m<pTempRgbImg->width; m ++)
    	{
	#if 1
            //	int tTempY=pTempOutData[m];
            	tTempCurPoint.x=m;
           	tTempCurPoint.y=1000-(pInputData[m]-10000)*1000/60000;// pInputData[m];
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
	
	cvShowImage("filter_img", pTempRgbImg);
//	cvWaitKey(0);
}

void test(int *pInputHeartData, int *pInputGSensorData, int tInputLength)
{
	if(pXTestData==NULL)
		pXTestData=(int *)malloc(sizeof(int)*tInputLength);

	int i=0;
	for(i=0; i<tInputLength; i++)
	{
		CalHeartRate(pInputHeartData[i], pInputGSensorData[i]);
	}

	drawTestImg(pXTestData, tXTestCnt);
}


