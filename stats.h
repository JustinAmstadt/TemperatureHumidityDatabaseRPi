#ifndef STATS_H
#define STATS_H

float getAverage(float* tableData, int arrSize){
    double average = 0;
    for(size_t i = 0; i < arrSize; ++i){
        average += tableData[i];
    }

    return average / arrSize;
}

float getMax(float* tableData, int arrSize){
    float max = 0;

    for(size_t i = 0; i < arrSize; ++i){
	if(tableData[i] > max){
            max = tableData[i];
	}
    }

    return max;
}

float getMin(float* tableData, int arrSize){
    float min = tableData[0];

    for(size_t i = 1; i < arrSize; ++i){
	if(tableData[i] < min){
            min = tableData[i];
	}
    }

    return min;
}

#endif
