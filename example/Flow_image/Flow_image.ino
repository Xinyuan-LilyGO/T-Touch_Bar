
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

#include "Arduino.h"
#include <TFT_GC9D01N.h>
#include <SPI.h>
#include "lvgl.h"
#include "FT6336U.h"
#include <stdio.h>

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

static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
void lv_port_disp_init(void);
static void ofs_y_anim(void *img, int32_t v);
void ui_screen_init(void);
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
    ui_screen_init();
    lv_disp_load_scr(meun_obj_bg);

}

void loop()
{
    tp = ft6336u.scan();
    lv_timer_handler();
    vTaskDelay(5);

}



static void ofs_y_anim(void *img, int32_t v)
{
    lv_img_set_offset_y((lv_obj_t *)img, v);
}

void ui_screen_init(void)
{
    meun_obj_bg  = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(meun_obj_bg, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(meun_obj_bg, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_meun_group = lv_obj_create(meun_obj_bg);
    lv_obj_set_style_bg_color(ui_meun_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT); //Background black
    lv_obj_set_style_bg_opa(ui_meun_group, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_meun_group, 0, LV_PART_MAIN | LV_STATE_DEFAULT); //Set the radius of the box to 0
    lv_obj_set_style_border_color(ui_meun_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(ui_meun_group, 40, 320 );
    lv_obj_set_pos(ui_meun_group, 0, 0);
    lv_obj_set_style_bg_opa(ui_meun_group, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);//The color of the scroll bar defaults to transparent
    lv_obj_set_style_bg_opa(ui_meun_group, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);//The color of the scroll bar is also set to transparent
    lv_obj_align(ui_meun_group, LV_ALIGN_TOP_LEFT, 0, 0);

    LV_IMG_DECLARE(icons_World_Cup);
    /*Now create the actual image*/
    lv_obj_t *img1 = lv_img_create(ui_meun_group);
    lv_img_set_src(img1, &icons_World_Cup);
    lv_obj_align(img1, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(img1, 30, 320);
    lv_img_set_pivot(img1, 0, 0);    /*Rotate around the top left corner*/

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, img1);
    lv_anim_set_exec_cb(&a, ofs_y_anim);
    lv_anim_set_values(&a, 0, 300);
    lv_anim_set_time(&a, 5000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

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
    disp_drv.rotated = LV_DISP_ROT_180;
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
