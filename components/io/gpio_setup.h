#ifndef GPIO_SETUP_H
#define GPIO_SETUP_H
#include "freertos/idf_additions.h"
void configure_gpio();
void led_task(void *arg);
void setIOQueueHandle(QueueHandle_t ref);

#endif