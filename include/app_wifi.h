#ifndef WIFI_H
#define WIFI_H

int init_wifi(void);
bool is_connected_wifi(void);
bool reconnect_wifi(void);
void report_wifi(void);

void init_ntp(void);
int update_ntp(void);
time_t get_time_t(void);

void init_mqtt(const char* broker);
void mqtt_reconnect(void);
bool mqtt_is_connected(void);
bool mqtt_publish(const char *topic, const char *payload);

// How's that for consistent naming!
// Deal with it.

#endif // WIFI_H