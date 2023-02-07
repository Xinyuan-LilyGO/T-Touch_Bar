
#define I2C_SDA       10
#define I2C_SCL       11
#define INT_N_PIN     12
#define RST_N_PIN     9
#define TFT1_CS       14
#define TFT2_CS       13
#define LED_PIN       21
#define TOUCH_PIN     1
#define TOUCH_CE_PIN  39
#define TFT1_EN   {digitalWrite(TFT2_CS, HIGH);digitalWrite(TFT1_CS, LOW);}
#define TFT2_EN   {digitalWrite(TFT1_CS, HIGH);digitalWrite(TFT2_CS, LOW);}
#define DISP_BUF_SIZE (screenWidth * screenHeight)
#define RESET_WIFI_PIN   0
#define ROOT_HTML  "<!DOCTYPE html><html><head><title>WIFI Config by lwang</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.input{display: block; margin-top: 10px;}.input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}.input input{height: 30px;width: 200px;}.btn{width: 120px; height: 35px; background-color: #000000; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}</style><body><form method=\"GET\" action=\"connect\"><label class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\"></label><label class=\"input\"><span>WiFi PASS</span><input type=\"text\"  name=\"pass\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"Submie\"> <p><span> Nearby wifi:</P></form>"

#include "Arduino.h"
#include <TFT_GC9D01N.h>
#include <SPI.h>
#include "lvgl.h"
#include "FT6336U.h"
#include <stdio.h>
#include "ui.hpp"
#include <WiFi.h>
#include "time.h"
#include "sntp.h"
#include "USB.h"
#include "USBHIDConsumerControl.h"
#include <WebServer.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>

const char *AP_SSID  = "T-Touch_Bar";
String wifi_ssid = "";
String wifi_pass = "";
String scanNetworksID = "";//储存扫描到的WiFi
const char *ntpServer = "pool.ntp.org";
const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
const char *time_zone = "HKT-8";// "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)
char time_String[10] = "";
char date_String[50] = "";
WebServer server(80);
USBHIDConsumerControl ConsumerControl;

FT6336U       ft6336u(I2C_SDA, I2C_SCL, RST_N_PIN, INT_N_PIN);
FT6336U_TouchPointType tp;
TFT_GC9D01N_Class tft;
static const uint32_t     screenWidth  = 40;
static const uint32_t     screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;
static lv_color_t        *buf;
lv_disp_t *disp;

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t UI_Task_Handle = NULL;
static TaskHandle_t Task_Handle = NULL;

void UI_Task(void *pvParameters);
void Task(void *pvParameters);

static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
void lv_port_disp_init(void);

void setup_time();
void timeavailable(struct timeval *t);
void printLocalTime();

void wifi_Config();
bool AutoConfig();
bool Read_pin_state(uint8_t pin);

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT );
    digitalWrite(LED_PIN, HIGH);

    pinMode(TOUCH_CE_PIN, OUTPUT);
    digitalWrite(TOUCH_CE_PIN, HIGH);

    pinMode(RESET_WIFI_PIN, INPUT_PULLUP);
    ConsumerControl.begin();
    USB.begin();

    tft.begin();
    tft.backlight(255);
    pinMode(TFT2_CS, OUTPUT);
    pinMode(TFT1_CS, OUTPUT);

    digitalWrite(TFT2_CS, LOW);
    digitalWrite(TFT1_CS, LOW);
    delay(200);

    TFT2_EN
    tft.writecommand(0x36);
    tft.writedata(0x00);

    ft6336u.begin();
    delay(200);

    TFT1_EN
    tft.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, BLACK);
    TFT2_EN
    tft.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, BLACK);
    delay(200);

    setup_time();

    lv_init();
    lv_port_disp_init();
    ui_screen_init();
    lv_disp_load_scr(meun_obj_bg);


    xTaskCreatePinnedToCore(
        Task_Create
        , "Task_Create"
        , 4096
        ,  NULL
        ,  1  // Priority
        ,  &AppTaskCreate_Handle
        ,  ARDUINO_RUNNING_CORE);

    if (!AutoConfig()) {
        wifi_Config();
    }

}

void loop()
{
    if (Read_pin_state(RESET_WIFI_PIN)) {
        esp_wifi_restore();
        vTaskDelay(10);
        ESP.restart();
    }
    vTaskDelay(5);

}


//Task_Create
static void Task_Create(void *pvParameters)
{
    (void) pvParameters;

    BaseType_t xReturn = pdPASS;


    xReturn = xTaskCreatePinnedToCore(
                  UI_Task
                  ,  "Touch_Task"
                  ,  4096  // Stack size
                  ,  NULL
                  ,  1  // Priority
                  ,  &UI_Task_Handle
                  ,  ARDUINO_RUNNING_CORE);
    if (xReturn == pdPASS) {
        Serial.println("Create Touch_Task  succeeded!");
    }



    xReturn = xTaskCreatePinnedToCore(
                  Time_Task
                  ,  "Time_Task"
                  ,  4096  // Stack size
                  ,  NULL
                  ,  1  // Priority
                  ,  &Task_Handle
                  ,  ARDUINO_RUNNING_CORE);
    if (xReturn == pdPASS) {
        Serial.println("Create Time_Task  succeeded!");
    }

    vTaskDelete(AppTaskCreate_Handle); //Delete AppTaskCreate

    for (;;) {
    }

}


void UI_Task(void *pvParameters)
{
    (void) pvParameters;

    for (;;) {
        tp = ft6336u.scan();
        lv_timer_handler();
        vTaskDelay(5);

    }
}


bool Read_pin_state(uint8_t pin)
{
    if (digitalRead(pin)) {
        vTaskDelay(50);
        while ((digitalRead(pin)));
        return true;
    }
    return false;

}

void Time_Task(void *pvParameters)
{
    (void) pvParameters;


    for (;;) {
        server.handleClient();
        vTaskDelay(5);

        if (WiFi.status() == WL_CONNECTED) {//WIFI connected
            printLocalTime();
            vTaskDelay(500);
        } else {
            lv_obj_set_style_text_font(clock_date_label, &lv_font_montserrat_10, LV_STATE_DEFAULT);
            lv_label_set_text_fmt(clock_date_label, "Connect to T-Touch_Bar to configure wifi");

            lv_label_set_text_fmt(clock_label, "#FF7F24 %d# ", WiFi.RSSI(0));
            vTaskDelay(5);
        }

    }
}

void wifi_Config()
{
    Serial.println("scan start");
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
        scanNetworksID = "no networks found";
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            scanNetworksID += "<P>" + WiFi.SSID(i) + "</P>";
            delay(10);
        }
    }
    Serial.println("");

    WiFi.mode(WIFI_AP);
    boolean result = WiFi.softAP(AP_SSID, "");
    if (result) {
        IPAddress myIP = WiFi.softAPIP();
        Serial.println("");
        Serial.print("Soft-AP IP address = ");
        Serial.println(myIP);
        Serial.println(String("MAC address = ")  + WiFi.softAPmacAddress().c_str());
        Serial.println("waiting ...");
    } else {
        Serial.println("WiFiAP Failed");
        delay(3000);
        ESP.restart();
    }

    if (MDNS.begin("lilygo")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", []() {
        server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
    });

    server.on("/connect", []() {
        server.send(200, "text/html", "<html><body><font size=\"10\">successd,wifi connecting...<br />Please close this page manually.</font></body></html>");

        WiFi.softAPdisconnect(true);
        wifi_ssid = server.arg("ssid");
        wifi_pass = server.arg("pass");
        server.close();
        WiFi.softAPdisconnect();
        Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass);

        WiFi.mode(WIFI_STA);
        WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
        uint8_t Connect_time = 0;
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
            Connect_time ++;
            if (Connect_time > 80) {
                Serial.println("Connection timeout, check input is correct or try again later!");
                delay(3000);
                ESP.restart();
            }
        }
        Serial.println("");
        Serial.println("WIFI Config Success");
        Serial.printf("SSID:%s", WiFi.SSID().c_str());
        Serial.print("  LocalIP:");
        Serial.print(WiFi.localIP());
        Serial.println("");

    });
    server.begin();
}

bool AutoConfig()
{
    WiFi.begin();
    for (int i = 0; i < 8; i++) {
        int wstatus = WiFi.status();
        if (wstatus == WL_CONNECTED) {
            Serial.println("WIFI SmartConfig Success");
            Serial.printf("SSID:%s", WiFi.SSID().c_str());
            Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
            Serial.print("LocalIP:");
            Serial.print(WiFi.localIP());
            Serial.print(" ,GateIP:");
            Serial.println(WiFi.gatewayIP());
            return true;
        } else {
            Serial.print("WIFI AutoConfig Waiting......");
            Serial.println(wstatus);
            delay(500);
        }
    }
    Serial.println("WIFI AutoConfig Faild!" );
    return false;
}

static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    if (area->y1 <= 160 && area->y2 <= 160) {
        TFT1_EN
        tft.DrawImage(area->x1, area->y1, w, (h > 160 ? 160 : h), (uint16_t *)color_p);
    } else if (area->y1 > 160 && area->y2 > 160) {
        TFT2_EN
        tft.DrawImage(area->x1, area->y1 - 159, w, h, (uint16_t *)color_p);

    } else {
        uint32_t h1 = (160 - area->y1);
        uint32_t h2 = (area->y2 - 160 + 1);
        TFT1_EN
        tft.BlockWrite(area->x1, area->x1 + w - 1, area->y1, 159);
        SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
        DC_D;
        for (int i = 0; i < (w * h1); i++) {
            SPI.write16(*((uint16_t *)(color_p + i)));
        }
        SPI.endTransaction();

        TFT2_EN
        tft.BlockWrite(area->x1, area->x1 + w - 1, 0, area->y1 + h2 - 1);
        SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
        DC_D;
        int j = w * h1;
        for (int i = 0; i < (w * h2); i++) {
            SPI.write16(*((uint16_t *)(color_p + i + j)));
        }
        SPI.endTransaction();
    }

    lv_disp_flush_ready(disp);
}

void my_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{

    if (tp.tp[0].status == 1) {
        if (data->point.y > 165) {
            data->point.x = tp.tp[0].x;
            data->point.y = tp.tp[0].y - 40;
        } else {
            data->point.x = tp.tp[0].x;
            data->point.y = tp.tp[0].y;
        }
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void lv_port_disp_init(void)
{
    buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * DISP_BUF_SIZE,
                                         MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, DISP_BUF_SIZE);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    // disp_drv.full_refresh=1;
    disp_drv.sw_rotate = 1;
    disp_drv.rotated = LV_DISP_ROT_90;
    lv_disp_drv_register(&disp_drv);
    disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/

    //touch
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER  ;
    indev_drv.read_cb = my_input_read;
    /*Register the driver in LVGL and save the created input device object*/
    lv_indev_t *my_indev = lv_indev_drv_register(&indev_drv);

}


void printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    lv_obj_set_style_text_font(clock_date_label, &font_alibaba_26, LV_STATE_DEFAULT);

    // Serial.println(&timeinfo, "%F %T %A");
    sprintf(time_String, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    sprintf(date_String, "%02d.%02d", timeinfo.tm_mon + 1, timeinfo.tm_mday);

    lv_label_set_text_fmt(clock_date_label, "#00F5FF %s# ", time_String);
    lv_label_set_text_fmt(clock_label, "#FF7F24 %s# ", date_String);

}

// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
    Serial.println("Got time adjustment from NTP!");
    printLocalTime();
}

void setup_time()
{
    // set notification call-back function
    sntp_set_time_sync_notification_cb( timeavailable );

    /**
     * NTP server address could be aquired via DHCP,
     *
     * NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
     * otherwise SNTP option 42 would be rejected by default.
     * NOTE: configTime() function call if made AFTER DHCP-client run
     * will OVERRIDE aquired NTP server address
     */
    sntp_servermode_dhcp(1);    // (optional)

    /**
     * This will set configured ntp servers and constant TimeZone/daylightOffset
     * should be OK if your time zone does not need to adjust daylightOffset twice a year,
     * in such a case time adjustment won't be handled automagicaly.
     */
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

    /**
     * A more convenient approach to handle TimeZones with daylightOffset
     * would be to specify a environmnet variable with TimeZone definition including daylight adjustmnet rules.
     * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
     */
    configTzTime(time_zone, ntpServer1, ntpServer2);
}
