#ifndef TIMER_SETUP_H
#define TIMER_SETUP_H
#include "freertos/idf_additions.h"
void configure_gptimer();
void setTimerQueueHandle(QueueHandle_t ref);

#endif