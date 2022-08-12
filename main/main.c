#include <stdio.h>

#include "nvs_flash.h"
#include "ov2640.h"
#include "HTTP_Server.h"

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    camera_init();

    wifi_init_staAp();

    start_webserver();
}
