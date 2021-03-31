#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "ctype.h"
#include <fstream>

using namespace std;

int main()
{
	string filename;

	cout << "Please enter a file name to read and write: ";
	getline(cin, filename);

	char delim;
	float delimI, accelI, gyroI, magI, tempI, timeI, airspeedI;
	float _accelScale = 4.0 / 32768.0f;
	float _gyroScale = 500.0f / 32768.0f;
	//float _magScaleX = ((((float)rxData[0]) - 128.0f) / (256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla

	int16_t accel, gyro, mag, temp, airspeed;
	uint32_t time;

	ifstream infile;
	infile.open(filename + ".raw", ios::in | ios::binary);
	if (!infile.is_open()) {
		cout << "failed to open file " << filename << ".raw" << endl;
		return 0;
	}
	cout << "Converting ..." << endl;
	
	ofstream outfile;
	outfile.open(filename + ".csv");

	outfile << "Time (s)" << "," << "Pitot Status Code" << "," << "Differential Pressure (psi)" << "," << "Temperature (c)" << "," << "Body Accel x (g)" << "," << "Body Accel y (g)" << "," << "Body Accel z (g)" << "," << "Body Gyro x (°/s)" << "," << "Body Gyro y (°/s)" << "," << "Body Gyro z (°/s)" << "," << "Left Accel x (g)" << "," << "Left Accel y (g)" << "," << "Left Accel z (g)" << "," << "Left Gyro x (°/s)" << "," << "Left Gyro y (°/s)" << "," << "Left Gyro z (°/s)" << "," << "Right Accel x (g)" << "," << "Right Accel y (g)" << "," << "Right Accel z (g)" << "," << "Right Gyro x (°/s)" << "," << "Right Gyro y (°/s)" << "," << "Right Gyro z (°/s)";

	while (!infile.eof())
	{
		delim = infile.get();
		delimI = delim;
		if (delimI != -1) { cout << "incorrect delimiter byte \n"; }
		outfile << "\n";

		// timing
		time = infile.get() << 24 | infile.get() << 16 | infile.get() << 8 | infile.get();
		timeI = time;
		outfile << timeI * .000001 << ",";

 		uint8_t ms4525doData[4] = {0, 0, 0, 0};
		for (int i = 0; i < 4; i++){
			ms4525doData[i] = infile.get();
		}
		uint8_t ms4525doStatus = (ms4525doData[0] & 0xC0) >> 6;
  		ms4525doData[0] = ms4525doData[0] & 0x3F;

  		// format data
  		uint16_t ms4525doDifferentialPressureRaw = ms4525doData[0] << 8 | ms4525doData[1];
  		#define P_MIN -1.0f
		#define P_MAX 1.0f
		float ms4525doDifferentialPressure = -((ms4525doDifferentialPressureRaw - 0.1f * 16383) * (P_MAX - P_MIN) / (0.8f * 16383) + P_MIN);

  		uint16_t ms4525doTemperatureRaw = 0xffe0 & (ms4525doData[2] << 8 | ms4525doData[3]) >> 5;
  		float ms4525doTemperature = ((200.0f * ms4525doTemperatureRaw) / 2047) - 50;

		switch (ms4525doStatus) {
   			case 0:
      				outfile << "Ok ,";
      				break;
			case 1:
      				outfile << "Reserved ,";
      				break;
			case 2:
      				outfile << "Stale ,";
      				break;
    			default:
      				outfile << "Error ,";
      				break;
		}

		// airspeed sensor

		outfile << ms4525doDifferentialPressure << ",";

		// temperature

		outfile << ms4525doTemperature << ",";

		// mpu 1 
		for (int i = 0; i < 3; i++)
		{
			// accel
			accel = infile.get() << 8 | infile.get();
			accelI = accel;
			outfile << accelI * _accelScale << ",";
		}
		for (int i = 0; i < 3; i++)
		{
			// gyro
			gyro = infile.get() << 8 | infile.get();
			gyroI = gyro;
			outfile << gyroI * _gyroScale << ",";

		}

		// mpu 2
		for (int i = 0; i < 3; i++)
		{
			// accel
			accel = infile.get() << 8 | infile.get();
			accelI = accel;
			outfile << accelI * _accelScale << ",";
		}
		for (int i = 0; i < 3; i++)
		{
			// gyro
			gyro = infile.get() << 8 | infile.get();
			gyroI = gyro;
			outfile << gyroI * _gyroScale << ",";
		}

		// mpu 3
		for (int i = 0; i < 3; i++)
		{
			// accel
			accel = infile.get() << 8 | infile.get();
			accelI = accel;
			outfile << accelI * _accelScale << ",";
		}
		for (int i = 0; i < 3; i++)
		{
			// gyro
			gyro = infile.get() << 8 | infile.get();
			gyroI = gyro;
			outfile << gyroI * _gyroScale << ",";
		}
	}
	cout << "Done!" << endl;
}
