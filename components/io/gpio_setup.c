#include "gpio_setup.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include <stdint.h>

// Definição de pinos para GPIOs
// Macro para IO: Botão 0 - Pino 21
#define GPIO_INPUT_IO_0 21
// Macro para IO: Botão 1 - Pino 22
#define GPIO_INPUT_IO_1 22
// Macro para IO: Botão 2 - Pino 23
#define GPIO_INPUT_IO_2 23
// Máscara de bits para pinos de entrada
#define GPIO_INPUT_PIN_SEL ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1) | (1ULL<<GPIO_INPUT_IO_2))

// Macro para IO: Led 0 - Pino 2
#define GPIO_OUTPUT_IO_0 2
// Máscara de bits para pinos de entrada
#define GPIO_OUTPUT_PIN_SEL (1ULL<<GPIO_OUTPUT_IO_0)

// Criação da TAG para facilitação de análise dos logs.
static const char* TAG = "IO";

// Fila de eventos do IO de propósito geral
static QueueHandle_t gpio_evt_queue = NULL;

// Gerenciador de interrupções de IO
static void IRAM_ATTR gpio_isr_handler(void * arg){
    uint32_t* gpio_num = arg;
    xQueueSendFromISR(gpio_evt_queue, gpio_num, NULL);
}

// Getter para variável da fila.
static QueueHandle_t getQueueHandle(){
    return gpio_evt_queue;
}
// Setter para variável da fila.
void setIOQueueHandle(QueueHandle_t ref){
    gpio_evt_queue = ref;
}

void led_task(void *arg){
    static uint32_t led_state = 0;
    uint32_t io_num;
    for ( ;; ){
        if(xQueueReceive(getQueueHandle(), &io_num, portMAX_DELAY)){
            ESP_LOGI(TAG, "BOTAO %" PRIu32 " APERTADO. ESTADO = %d", io_num, gpio_get_level(io_num));
            if(io_num == GPIO_INPUT_IO_0){
                led_state = 1;
                ESP_LOGI(TAG, "LED aceso!");
            }else if(io_num == GPIO_INPUT_IO_1){
                led_state = 0;
                ESP_LOGI(TAG, "LED apagado!");
            }else if (io_num == GPIO_INPUT_IO_2){
                led_state = 1 - led_state;
                ESP_LOGI(TAG, "LED invertido!");
            }
            gpio_set_level(GPIO_OUTPUT_IO_0, led_state);
        }
    }
}

// Método de configuração do GPIO.
void configure_gpio(){
    
    // Prática 2 - GPIO
    gpio_config_t io_configs = {};

    io_configs.intr_type = GPIO_INTR_NEGEDGE;
    io_configs.mode = GPIO_MODE_INPUT;
    io_configs.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_configs.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_configs.pull_up_en = GPIO_PULLUP_ENABLE;

    gpio_config(&io_configs);

    io_configs.intr_type = GPIO_INTR_DISABLE;
    io_configs.mode = GPIO_MODE_OUTPUT;
    io_configs.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_configs.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_configs.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_configs);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void *) GPIO_INPUT_IO_0);
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void *) GPIO_INPUT_IO_1);
    gpio_isr_handler_add(GPIO_INPUT_IO_2, gpio_isr_handler, (void *) GPIO_INPUT_IO_2);
}