/*
* SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: CC0-1.0
*/
// #define ESP_LOG_COLOR_DISABLED     (1)  /* For Log v2 only */
// #define ESP_LOG_TIMESTAMP_DISABLED (1)  /* For Log v2 only */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_chip_info.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "gpio_setup.h"
#include "timer_setup.h"

// Criação da TAG para facilitação de análise dos logs.
static const char* TAG = "P3-Main";

// Função para pegar modelo baseado no valor carregado na struct esp_chip_info_t
const char* get_chip_model_name(int chip_model) {
    switch (chip_model) {
        case 1: return "ESP32";
        case 2: return "ESP32-S2";
        case 4: return "ESP32-S3";
        case 5: return "ESP32-C3";
        case 6: return "ESP32-H2";
        case 7: return "ESP32-C2";
        case 8: return "ESP32-C6";
        case 9: return "ESP32-P4";
        case 10: return "ESP32-H4";
        case 0:
        default: return "UNKNOWN";
    }
}

static void log_info(){
    esp_chip_info_t infos;

    // Preenche a struct esp_chip_info_t com as informações pertinentes.
    esp_chip_info(&infos);

    ESP_LOGI(TAG, "Versão do framework: %s", esp_get_idf_version());
    ESP_LOGI(TAG, "Modelo do chip: %s", get_chip_model_name((int) infos.model));
    ESP_LOGI(TAG, "Nº de cores: %d", infos.cores);
    ESP_LOGI(TAG, "Target do framework: %s", CONFIG_IDF_TARGET);
    ESP_LOGI(TAG, "Features: %s%s%s%s, ",
           (infos.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (infos.features & CHIP_FEATURE_BT) ? "BT" : "",
           (infos.features & CHIP_FEATURE_BLE) ? ", BLE" : "",
           (infos.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");
    
    // Variavel para receber endereço MAC
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    char mac_str[18]; // 6 bytes * 2 dígitos cada + 5 ponto-e-virgula + \0
    
    // Formata e salva string em um buffer mac_str
    snprintf(mac_str, sizeof(mac_str),
             "%02X:%02X:%02X:%02X:%02X:%02X",
             baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    ESP_LOGI(TAG, "Endereço MAC: %s", mac_str);

    // Envia o buffer acima para receber endereço MAC do Bluetooth.
    esp_read_mac(baseMac, ESP_MAC_BT);

    // Formata e salva string em um buffer mac_str
    snprintf(mac_str, sizeof(mac_str),
            "%02X:%02X:%02X:%02X:%02X:%02X",
            baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    ESP_LOGI(TAG, "Endereço Bluetooth: %s", mac_str);
}

void app_main(void){

    ESP_LOGI(TAG,"Iniciando execução do código");
    log_info();
    
    // GPIO
    setIOQueueHandle(xQueueCreate(12, sizeof(uint32_t)));
    xTaskCreate(led_task, "led_task", 2048, NULL, 12, NULL);
    configure_gpio();

    // GPTimer
    setTimerQueueHandle(xQueueCreate(12, sizeof(uint64_t)));
    xTaskCreate(gptimer_clock, "gptimer_clock", 2048, NULL, 12, NULL);
    configure_gptimer();

    fflush(stdout);
}