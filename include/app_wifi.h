#ifndef WIFI_H
#define WIFI_H

int init_wifi(void);
bool is_connected_wifi(void);
bool reconnect_wifi(void);
void report_wifi(void);

void init_ntp(void);
int update_ntp(void);
time_t get_time_t(void);


#endif // WIFI_H