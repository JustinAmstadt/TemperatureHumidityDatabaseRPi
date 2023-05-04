#include <mysql/mysql.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "sensor.h"
#include "sql.h"
#include "stats.h"

bool insertHumidity(Sql* sql, float humidity);
char** getTimeRange(Sql* sql);
bool insertTemp(Sql* sql, float temp, char* tempType);
bool getHumidityData(Sql* sql, float* outputArr, char* startTime, char* endTime);
bool getTempData(Sql* sql, float* outputArr, char* startTime, char* endTime);
int getTableSize(Sql* sql, char* tableName, char* startTime, char* endTime);
void printStats(Sql* sql, char* startTime, char* endTime);
void readSensorData(Sql* sql);

int main(void)
{
    UserData userdata;
    Sql sqlData;

    userdata.server = "localhost";
    userdata.user = "justin";
    userdata.password = "xylion123";
    userdata.database ="sensordata";

    myConnect(&sqlData, &userdata);

    char* startTime = (char*)malloc(sizeof(char) * 200);
    char* endTime = (char*)malloc(sizeof(char) * 200);
    char** timeRange = getTimeRange(&sqlData);
    strcpy(startTime, timeRange[0]);
    strcpy(endTime, timeRange[1]);

    int user_input = 1;

    while(user_input){
	puts("Note: will read all data without a proper end and start time inputted");
	puts("0: end program");
	puts("1: enter new start time");
	puts("2: enter new end time");
	puts("3: get statistics on data");
	puts("4: read more data");

        scanf("%d", &user_input);

	char overflow[500];
	// scanf("%s", overflow);

	switch(user_input){
	    case 0:
		break;
	    case 1:
		puts("");
    		printf("Time Range: %s to %s\n", timeRange[0], timeRange[1]);
		puts("Enter start time (yyyy-mm-dd-hh:mm:ss) only year, month, and day required");
		memset(startTime, 0, strlen(startTime));
		scanf("%s", startTime);
		startTime[10] = ' ';
		puts("");
		break;
	    case 2:
		puts("");
    		printf("Time Range: %s to %s\n", timeRange[0], timeRange[1]);
		puts("Enter end time (yyyy-mm-dd-hh:mm:ss) only year, month, and day required");
		memset(endTime, 0, strlen(endTime));
		scanf("%s", endTime);
		endTime[10] = ' ';
		puts("");
		break;
	    case 3:
		puts("");
		printStats(&sqlData, startTime, endTime);
		puts("");
		break;
	    case 4:
                readSensorData(&sqlData);
		break;
	    default:
		puts("Incorrect number inputted");
	}
	
    }

    return 0;
}

void printStats(Sql* sql, char* startTime, char* endTime){
	if(strcmp(startTime, endTime) == 0){
	    puts("start time and end time cannot be the same");
	    return;
	}
	int humSize = getTableSize(sql, "humidity", startTime, endTime);
	int tempSize = getTableSize(sql, "temperature", startTime, endTime);
	float* tempTable = (float*)malloc(sizeof(float) * (tempSize + 1));    
	float* humidityTable = (float*)malloc(sizeof(float) * (humSize + 1));    
    
	getTempData(sql, tempTable, startTime, endTime);
	getHumidityData(sql, humidityTable, startTime, endTime);

	printf("Temperature average: %.2f C\n", getAverage(tempTable, tempSize));
	printf("Temperature max: %.2f C\n", getMax(tempTable, tempSize));
	printf("Temperature min: %.2f C\n", getMin(tempTable, tempSize));
	printf("Humidity average: %.2f%\n", getAverage(humidityTable, humSize));
	printf("Humidity max: %.2f%\n", getMax(humidityTable, humSize));
	printf("Humidity min: %.2f%\n", getMin(humidityTable, humSize));

	free(tempTable);
	free(humidityTable);
}

// Read sensor data in. Inputs into database if successful and does nothing otherwise.
void readSensorData(Sql* sql){
    if ( wiringPiSetup() == -1 )
        exit( 1 );
    while ( 1 )
    {
	int sensor_data[5] = { 0, 0, 0, 0, 0 };
        if(read_dht11_dat(sensor_data)){
	    insertHumidity(sql, sensor_data[0] + sensor_data[1]);
	    insertTemp(sql, sensor_data[2] + sensor_data[3], "C");
	}
	delay( 1000 );
    }
}

// Inserts data into humidity table. Returns success bool
bool insertHumidity(Sql* sql, float humidity){
    char command[80];
    sprintf(command, "insert into humidity values (%.2f, current_timestamp())", humidity);
    free(command);
    return myQueryCh(sql, command, NULL);

}

// Inserts data into temperature table. Returns success bool
bool insertTemp(Sql* sql, float temp, char* tempType){
    char command[80];
    sprintf(command, "insert into temperature values (%.2f, \'%s\', current_timestamp())", temp, tempType);
    return myQueryCh(sql, command, NULL);
}

bool getHumidityData(Sql* sql, float* outputArr, char* startTime, char* endTime){
    char command[200];
    if(startTime == NULL || endTime == NULL){
        sprintf(command, "select * from humidity");
    }
    else{
        sprintf(command, "select * from humidity where ts between \'%s\' and \'%s\'", startTime, endTime);
    }
    return myQueryFl(sql, command, outputArr);
}

bool getTempData(Sql* sql, float* outputArr, char* startTime, char* endTime){
    char command[200];
    if(startTime == NULL || endTime == NULL){
        sprintf(command, "select * from temperature");
    }
    else{
        sprintf(command, "select * from temperature where ts between \'%s\' and \'%s\'", startTime, endTime);
    }
    return myQueryFl(sql, command, outputArr);
}

char** getTimeRange(Sql* sql){
    int tempSize = getTableSize(sql, "temperature", NULL, NULL);
    char** timestamps = (char**)malloc(sizeof(char*) * tempSize);

    char command[80];
    sprintf(command, "select ts from temperature");
    if(!myQueryChArr(sql, command, timestamps)){
        return NULL;
    }

    char** timeRange = (char**)malloc(sizeof(char*) * 2);

    strcpy(timeRange[0] = malloc(strlen(timestamps[0]) + 1), timestamps[0]);
    strcpy(timeRange[1] = malloc(strlen(timestamps[tempSize - 1]) + 1), timestamps[tempSize - 1]);
    
    for(size_t i = 0; i < tempSize; ++i){
        free(timestamps[i]);
    }

    return timeRange;
}

int getTableSize(Sql* sql, char* tableName, char* startTime, char* endTime){
    char command[200];
    char size [50] = ""; 
    if(startTime == NULL || endTime == NULL){
        sprintf(command, "select count(*) row_count from %s", tableName);
    }
    else{
        sprintf(command, "select count(*) row_count from %s where ts between \'%s\' and \'%s\'", tableName, startTime, endTime);
    }
    myQueryCh(sql, command, size);
    
    char *result = malloc(strlen(size) + 1);
    strcpy(result, size);

    return atoi(result);
}
