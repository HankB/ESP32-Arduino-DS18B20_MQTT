

typedef struct
{
    byte addr[8];
    char *tag;
} DS18B20;

int init_DS18B20(DS18B20 *sensors, int max_sensors);
float read_DS18B20(DS18B20 *s);
