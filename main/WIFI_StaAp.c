#include "WIFI_StaAp.h"


#define EXAMPLE_ESP_MAXIMUM_RETRY  5

/* 
    you can change the ESP_WIFI_SSID ESP_WIFI_PASS here.
*/
#define ESP_WIFI_AP_SSID      "mywifissid"
#define ESP_WIFI_AP_PASS      "123456789"
#define ESP_WIFI_CHANNEL   1
#define MAX_STA_CONN       4

static const char *TAG = "wifi StaAp";

static int s_retry_num = 0;

/* If can't connect to Ap, then retry */
void WIFI_EVENT_STA_DISCONNECTED_FUN(void)
{
    if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
        esp_wifi_connect();
        s_retry_num++;
        ESP_LOGI(TAG, "retry to connect to the AP...");
    } else {
    	s_retry_num = 0;
        ESP_LOGI(TAG,"connect to the AP fail, maybe you can check the ssid and password");
        /*  Sending Bits to s_wifi_event_group, which will be received 
            in WIFI_config_handler() which in HTTP_Server.c */
        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
}

/* If connect to Ap */
void WIFI_EVENT_STA_CONNECTED_FUN(void)
{
    s_retry_num = 0;
    /*  Sending Bits to s_wifi_event_group, which will be received 
        in WIFI_config_handler() which in HTTP_Server.c */
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
}

/* If get a ip from Ap */
void IP_EVENT_STA_GOT_IP_FUN( void* event_data )
{
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    /* printf the IP alloc for eps32 */
    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
}

/* When any station connect to esp32 */
void WIFI_EVENT_AP_STACONNECTED_FUN( void* event_data  )
{
    wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    /* printf the MAC of station */
    ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
}

/* When any station disconnect */
void WIFI_EVENT_AP_STADISCONNECTED_FUN( void* event_data  )
{
    wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
    /* printf the MAC of station */
    ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",MAC2STR(event->mac), event->aid);
}

void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    /* use if/else to split the event_base */
    if( event_base == WIFI_EVENT )
    {
        switch ( event_id )
        {
            case WIFI_EVENT_STA_START:          esp_wifi_connect();                  break;
            case WIFI_EVENT_STA_STOP:           ESP_LOGI(TAG,"WIFI_EVENT_STA_STOP"); break; 
            case WIFI_EVENT_STA_CONNECTED:      WIFI_EVENT_STA_CONNECTED_FUN();      break;
            case WIFI_EVENT_STA_DISCONNECTED:   WIFI_EVENT_STA_DISCONNECTED_FUN();   break;
            case WIFI_EVENT_AP_START:           ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d", 
                                                    ESP_WIFI_AP_SSID, ESP_WIFI_AP_PASS, ESP_WIFI_CHANNEL); break;
            case WIFI_EVENT_AP_STOP:            ESP_LOGI(TAG,"WIFI_EVENT_AP_STOP");  break;
            case WIFI_EVENT_AP_STACONNECTED:    WIFI_EVENT_AP_STACONNECTED_FUN( event_data );    break;
            case WIFI_EVENT_AP_STADISCONNECTED: WIFI_EVENT_AP_STADISCONNECTED_FUN( event_data );  break;                 
            default:  break;
        }
    }
    else if( event_base == IP_EVENT )  // 路由事件ID 组
    {
        switch ( event_id )
        {
            case IP_EVENT_STA_GOT_IP:        IP_EVENT_STA_GOT_IP_FUN(event_data);       break;
            case IP_EVENT_STA_LOST_IP:       ESP_LOGI(TAG,"IP_EVENT_STA_LOST_IP");      break;
            case IP_EVENT_AP_STAIPASSIGNED:  ESP_LOGI(TAG,"IP_EVENT_AP_STAIPASSIGNED"); break;
            default:  break;
        }
    }
}

// void wifi_init_staAp(const char *ESP_WIFI_STA_SSID, const char *ESP_WIFI_STA_PASS)
void wifi_init_staAp(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    /* 
        wifi station mode config initialize, this code will be set in HTTP_Server.c  
        wifi_config_t wifi_station_config = {
            .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            },
        };
        strcpy((char *)wifi_station_config.sta.ssid, (char *)ESP_WIFI_STA_SSID);
        strcpy((char *)wifi_station_config.sta.password, (char *)ESP_WIFI_STA_PASS);
    */

    /* wifi SoftAp mode config initialize */
    wifi_config_t wifi_SoftAP_config = {
        .ap = {
            .ssid = ESP_WIFI_AP_SSID,
            .ssid_len = strlen(ESP_WIFI_AP_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_AP_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(ESP_WIFI_AP_PASS) == 0) {
        wifi_SoftAP_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    /* set wifi mode into Station + SoftAp */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    /*  
        set wifi station mode config, this code will be set in HTTP_Server.c
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_station_config) );
    */
    /* set wifi SoftAp mode config */
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP , &wifi_SoftAP_config) );
    /* open wifi */
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "WIFI_StaAp finished.");

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
}

