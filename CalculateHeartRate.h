

#ifndef __CALCULATE_HEART_RATE_H__
#define __CALCULATE_HEART_RATE_H__

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


typedef struct tagHeartValue{
	int index;
	int value;
}HeartValue;

void CalHeartRate(int tInputHeartData, int tInputGSensorata);

void test(int *pInputHeartData, int *pInputGSensorData, int tInputLength);

#endif


