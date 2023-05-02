#ifndef SENSOR_H
#define SENSOR_H

#include <wiringPi.h>
#include <stdint.h>
#define MAXTIMINGS 85
#define DHTPIN 7

// Returns 1 on success and 0 on failure
int read_dht11_dat(int dht11_dat[])
{
	uint8_t laststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i;
	float f;
	pinMode(DHTPIN, OUTPUT);
	digitalWrite(DHTPIN, LOW);
	delay(18);
	digitalWrite(DHTPIN, HIGH);
	delayMicroseconds(40);
	pinMode(DHTPIN, INPUT);
	for (i = 0; i < MAXTIMINGS; i++)
	{
		counter = 0;
		while(digitalRead(DHTPIN) == laststate)
		{
			counter++;
			delayMicroseconds(1);
			if (counter ==255)
				break;
		}
		laststate = digitalRead(DHTPIN);
		if (counter == 255)
			break;
		if ((i >= 4) && (i % 2 == 0))
		{
			dht11_dat[j/8] <<= 1;
		if (counter > 25) // this check is how long the pulse width is low.
			dht11_dat[j/8] |= 1;
			j++;
		}
	}
	if ((j >= 40) && (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2]
	+ dht11_dat[3]) & 0xFF )))
	{
		f = dht11_dat[2] * 9. / 5. + 32;
		printf("Humidity = %d.%d %% Temperature = %d.%d C (%.1f F)\n",
		dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f);
		return 1;
	}
	else
	{
		printf("Data not good, skip \n");
		return 0;
	}
}

#endif
