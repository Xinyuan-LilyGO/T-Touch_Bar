/*
 * This sketch is to get the current battery voltage
 */

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
#define BAT_ADC    2

#include "Arduino.h"
#include <TFT_GC9D01N.h>
#include <SPI.h>
#include "lvgl.h"
#include "FT6336U.h"
#include <stdio.h>
#include "esp_adc_cal.h"

FT6336U       ft6336u(I2C_SDA, I2C_SCL, RST_N_PIN, INT_N_PIN);
FT6336U_TouchPointType tp;
TFT_GC9D01N_Class tft;
static const uint32_t     screenWidth = 40;
static const uint32_t     screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;
static lv_color_t        *buf;
lv_disp_t *disp;
lv_obj_t *meun_obj_bg ;
lv_obj_t *ui_meun_group;
float Voltage = 0.0;
char Voltage_String[10] = "";
u_int32_t last = 0;

static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
void lv_port_disp_init(void);
static void ofs_y_anim(void *img, int32_t v);
void ui_screen_init(void);
uint32_t readADC_Cal(int ADC_Raw);
lv_obj_t *label;
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT );
    pinMode(TOUCH_CE_PIN, OUTPUT);
    digitalWrite(TOUCH_CE_PIN, HIGH);

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

    lv_init();
    lv_port_disp_init();
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_label_set_text_fmt(label, "%s ", Voltage_String);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

}

void loop()
{
    tp = ft6336u.scan();
    lv_timer_handler();
    vTaskDelay(5);

    if (millis() - last > 500) {
        Voltage = (readADC_Cal(analogRead(BAT_ADC))) * 2;
        sprintf(Voltage_String, "BAT Voltage: %.2fV", Voltage / 1000.0);
        lv_label_set_text_fmt(label, "%s ", Voltage_String);
        last = millis();
    }
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

uint32_t readADC_Cal(int ADC_Raw)
{
    esp_adc_cal_characteristics_t adc_chars;

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    return (esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}
