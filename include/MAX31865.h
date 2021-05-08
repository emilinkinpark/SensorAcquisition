#include <Adafruit_MAX31865.h>
#include <Wire.h>
#define CS  4
#define MOSI 19
#define MISO 18
#define CLK 5


Adafruit_MAX31865 thermo = Adafruit_MAX31865(CS, MOSI, MISO, CLK); // CS, SI (MOSI), SO (MISO), CLK
// To add new PT100 to the ESP32 just declare new class and only change the CS pin

#define RREF 430.0     // The value of the Rref resistor. Use 430.0 for temp_pt100 and 4300.0 for temp_pt1000
#define RNOMINAL 100.0 // The 'nominal' 0-degrees-C resistance of the sensor // 100.0 for temp_pt100, 1000.0 for temp_pt1000

void pt100Init()
{
    thermo.begin(MAX31865_2WIRE); // set to 2WIRE or 4WIRE as necessary
    uint8_t fault = thermo.readFault();
    Serial.print("PT100 Fault: ");
    Serial.println(fault);
}

float temperature()
{
    //uint16_t rtd = thermo.readRTD();
    float temp = thermo.temperature(RNOMINAL, RREF);
    //Serial.print("Temperature : "); Serial.println(midTemp);
    //Serial.print("Fault : "); Serial.println(pt100Fault);
    return temp;
}


uint8_t fault()
{
    uint8_t fault = thermo.readFault();
    if (fault & MAX31865_FAULT_HIGHTHRESH)
    {
        return 1;
    }
    if (fault & MAX31865_FAULT_LOWTHRESH)
    {
        return 1;
    }
    if (fault & MAX31865_FAULT_REFINLOW)
    {
        return 1;
    }
    if (fault & MAX31865_FAULT_REFINHIGH)
    {
        return 1;
    }
    if (fault & MAX31865_FAULT_RTDINLOW)
    {
        return 1;
    }
    if (fault & MAX31865_FAULT_OVUV)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    thermo.clearFault();
}
