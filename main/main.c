/*
* SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: CC0-1.0
*/
// #define ESP_LOG_COLOR_DISABLED     (1)  /* For Log v2 only */
// #define ESP_LOG_TIMESTAMP_DISABLED (1)  /* For Log v2 only */
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"

// Criação da TAG para facilitação de análise dos logs.
static const char* TAG = "P1";

// Função que funciona como main do projeto.
void app_main(void){
    
    // Iniciando o processo de log
    ESP_LOGI(TAG,"Iniciando execução do código");

    esp_chip_info_t infos;

    // Função que preenche a struct esp_chip_info_t com as informações pertinentes.
    esp_chip_info(&infos);

    // Log para dizer o nome do chip e a quantidade de cores.
    ESP_LOGI(TAG,"Modelo do chip: %s\nN° de cores: %u.", (char *)infos.model, infos.cores);
    ESP_LOGI(TAG, "TARGET do framework: %s\nFeatures: %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           (infos.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (infos.features & CHIP_FEATURE_BT) ? "BT" : "",
           (infos.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (infos.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    ESP_LOGI(TAG, "Finalizando execução...");

    fflush(stdout);
}