#include <math.h>

float floatTOdecimal(long int byte0, long int byte1, long int byte2, long int byte3) //Floating point Number in Little-endian to decimal conversion
{
    long int realbyte0, realbyte1, realbyte2, realbyte3;
    char S;
    long int E, M;
    float D;

    realbyte0 = byte3;
    realbyte1 = byte2;
    realbyte2 = byte1;
    realbyte3 = byte0;

    if ((realbyte0 & 0x80) == 0)
    {
        S = 0; //Positive
    }
    else
    {
        S = 1; //Negative
    }

    E = ((realbyte0 << 1) | (realbyte1 & 0x80) >> 7) - 127;
    M = ((realbyte1 & 0x7f) << 16) | (realbyte2 << 8) | realbyte3;
    D = pow(-1, S) * (1.0 + M / pow(2, 23)) * pow(2, E);
    return D;
}

float domglcalc(float t, float DOperc) //Temp in deg C, DOperc in %
{

    float T = 273.15 + t; //Temperature in Kelvin

    /*Constants*/
    float const S = 11.00;                       //11.0;         //Salinity Taken 10 as constant
    float const atm_pressure = 101.325;          //Atmospheric Pressure in kPa
    float const pressure = 16.06 + atm_pressure; //pressure in kPa; const pressure in salt water at 1.60m depth; more at https://bluerobotics.com/learn/pressure-depth-calculator/
    float const A1 = -173.4292;
    float const A2 = 249.6339;
    float const A3 = 143.3483;
    float const A4 = -21.8492;
    float const tmp_B1 = -0.033096;
    float const B2 = 0.014259;
    float const B3 = -0.001700;

    //     X1' =  Al + A2*(100/T)    + A3 ln T/100   +     A4 T/100       + S*[B1 + B2 T/100 + B3 (T/100)^2]; Weiss Equation more at https://water.usgs.gov/admin/memo/QW/qw81.11.html
    double X1prime = A1 + (A2 * (100 / T)) + (A3 * log(T / 100)) + (A4 * (T / 100)) + S * (tmp_B1 + B2 * (T / 100) + B3 * pow((T / 100), 2));
    double X1 = exp(X1prime);

    double uprime = 8.10765 - (1750.286 / T);

    double u = pow(10, uprime);

    double Phmg = pressure * (760 / 101.325); // Phmg in mmHg

    double X2 = ((Phmg - u) / (760 - u));

    double DOmgL = DOperc * X1 * X2 * 1.4276;

    return DOmgL;
}

uint16_t dec16_hex8(uint16_t byte1)                             // Converts 16 bit decimal to 8bit hex
{
    uint8_t temp_arr[2];
    temp_arr[0] = (byte1 >> 8);   //Higher Bits
    temp_arr[1] = (byte1 & 0xFF); //Lower Bits

    return (temp_arr[0] << 8 |temp_arr[1]);
}

int16_t hex16_signedint(uint8_t byte1, uint8_t byte2)          // Converts two 8 bits HEX into a single signed integer
{
    uint8_t sign_check = byte1 >> 7;

    if (sign_check == 1)
    {
        uint8_t compliment_1 = ~(byte1);

        uint8_t  compliment_2 = ~(byte2)+1;  //2's compliment

        int16_t compliment = -1*((compliment_1<<8)|compliment_2);

        return (compliment);
        
    }
    else
    {
        return(byte1<<8|byte2);
    }
}