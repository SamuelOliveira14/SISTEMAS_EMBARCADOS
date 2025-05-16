// Timer definitions
#include "timer_setup.h"
#include "driver/gptimer.h"
#include "driver/gptimer_types.h"
#include "freertos/idf_additions.h"
#include "esp_log.h"

 // 1s in 1 MHz clock resolution
#define ONE_SECOND_1MHZ (1 * 1000 * 1000)

// Fila de eventos do timer de propósito geral
static QueueHandle_t timer_evt_queue = NULL;

// Criação da TAG para facilitação de análise dos logs.
static const char* TAG = "TMR";
// Gerenciador de interrupções de IO
static _Bool IRAM_ATTR timer_isr_handler(gptimer_handle_t timer, 
                                        const gptimer_alarm_event_data_t *edata,
                                        void *user_data){
    uint64_t current_timer_count = edata->count_value;
    xQueueSendFromISR(timer_evt_queue, &current_timer_count, NULL);
    // Reconfigurando o alarme
    gptimer_alarm_config_t new_alarm_config = {
        .alarm_count = edata->alarm_value + (ONE_SECOND_1MHZ),
    };

    gptimer_set_alarm_action(timer, &new_alarm_config);
    return true;
}

// Getter para variável da fila.
static QueueHandle_t getQueueHandle(){
    return timer_evt_queue;
}
// Setter para variável da fila.
void setTimerQueueHandle(QueueHandle_t ref){
    timer_evt_queue = ref;
}

void gptimer_clock(void *arg){
    uint64_t current_timer_count;

    for ( ; ; ){
        if(xQueueReceive(getQueueHandle(), &current_timer_count, portMAX_DELAY)){
            //ESP_LOGI(TAG, " count: %" PRIu64 , current_timer_count);

            uint64_t seconds = current_timer_count / ONE_SECOND_1MHZ;
            uint64_t minutes = seconds / 60;
            uint64_t hours = minutes / 60;
            //ESP_LOGI(TAG, "Contador atual: %" PRIu64 " segundos", seconds % 60);
            //ESP_LOGI(TAG, "Contador atual: %" PRIu64 " minutos", minutes % 60);
            //ESP_LOGI(TAG, "Contador atual: %" PRIu64 " horas", hours % 24);

            ESP_LOGI(TAG, "%02" PRIu64 ":" "%02" PRIu64 ":" "%02" PRIu64, hours % 24, minutes % 60, seconds % 60);
        }
    }
}

// Método de configuração do GPIO.
void configure_gptimer(){

    // Prática 3 - GPtimer
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t config = {
        .direction = GPTIMER_COUNT_UP,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000 * 1000
    };

    ESP_ERROR_CHECK(gptimer_new_timer(&config, &gptimer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = ONE_SECOND_1MHZ,
        .flags.auto_reload_on_alarm = false,
    };

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_isr_handler,
    };

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}
