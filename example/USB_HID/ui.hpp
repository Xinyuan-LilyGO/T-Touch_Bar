
#include "lvgl.h"
#include "USBHIDConsumerControl.h"
extern USBHIDConsumerControl ConsumerControl;
LV_FONT_DECLARE(font_alibaba_26);
extern  char time_String[10] ;
extern char date_String[50];
extern FT6336U  ft6336u;

//meun group  主菜单组
lv_obj_t *meun_obj_bg;
lv_obj_t *ui_meun_group;
lv_obj_t *meun_brightness_obj;
lv_obj_t *meun_brightness_img;
lv_obj_t *meun_Speaker_obj;
lv_obj_t *meun_Speaker_img;
lv_obj_t *meun_Browser_obj;
lv_obj_t *meun_Browser_img;
lv_obj_t *meun_clock_obj;
lv_obj_t *meun_clock_img;
lv_obj_t *meun_calculator_obj;
lv_obj_t *meun_calculator_img;
lv_obj_t *meun_sleep_obj;
lv_obj_t *meun_sleep_img;



//brightness group 亮度组UI
lv_obj_t *ui_brightness_group;
lv_obj_t *img_brightness;
lv_obj_t *brightness_up_obj;
lv_obj_t *brightness_up_img;
lv_obj_t *brightness_down_obj;
lv_obj_t *brightness_down_img;

//Speaker group 扬声器组
lv_obj_t *ui_Speaker_group ; //扬声器组
lv_obj_t *img_Speaker;
lv_obj_t *Speaker_up_obj;
lv_obj_t *Speaker_up_img;
lv_obj_t *Speaker_down_img;
lv_obj_t *Speaker_down_obj;

//Browser group  浏览器组
lv_obj_t *ui_Browser_group;
lv_obj_t *Browser_search_obj;
lv_obj_t *Browser_search_img;
lv_obj_t *Browser_refresh_obj;
lv_obj_t *Browser_refresh_img;
lv_obj_t *Browser_forward_obj;
lv_obj_t *Browser_forward_img;
lv_obj_t *Browser_fallback_obj;
lv_obj_t *Browser_fallback_img;

//clock group 时钟组
lv_obj_t *ui_clock_group;
lv_obj_t *clock_date_obj;
lv_obj_t *clock_date_label;
lv_obj_t *clock_obj;
lv_obj_t *clock_label;

//back obj 回退对象
lv_obj_t *imgbtn_icons_back;
lv_obj_t *img_icons_back;

LV_IMG_DECLARE(icons_brightness);
LV_IMG_DECLARE(icons_Speaker);
LV_IMG_DECLARE(icons_Browser);
LV_IMG_DECLARE(icons_clock);
LV_IMG_DECLARE(icons_calculator);
LV_IMG_DECLARE(icons_sleep);
LV_IMG_DECLARE(icons_up);
LV_IMG_DECLARE(icons_down);
LV_IMG_DECLARE(icons_search);
LV_IMG_DECLARE(icons_refresh);
LV_IMG_DECLARE(icons_forward);
LV_IMG_DECLARE(icons_fallback);
LV_IMG_DECLARE(icons_circle);


#define _UI_MODIFY_FLAG_ADD 0
#define _UI_MODIFY_FLAG_REMOVE 1
#define _UI_MODIFY_FLAG_TOGGLE 2

//修改UI标志
void _ui_flag_modify(lv_obj_t *target, int32_t flag, int value)
{
    if (value == _UI_MODIFY_FLAG_TOGGLE) {
        if (lv_obj_has_flag(target, flag)) {
            lv_obj_clear_flag(target, flag);
        } else {
            lv_obj_add_flag(target, flag);
        }
    } else if (value == _UI_MODIFY_FLAG_ADD) {
        lv_obj_add_flag(target, flag);
    } else {
        lv_obj_clear_flag(target, flag);
    }
}

void _ui_anim_callback_set_opacity(lv_anim_t *a, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)a->user_data, v, 0);
}

void Fadeoff_Animation(lv_obj_t *TargetObject, int delay)
{
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 100);//动画的持续时间
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);//设置动画的自定义用户数据字段。
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);//设置动画
    lv_anim_set_values(&PropertyAnimation_0, 255, 0);//设置动画开始和接受的值
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);//设置动画路径
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, true);
    lv_anim_start(&PropertyAnimation_0);

}

void Fadeon_Animation(lv_obj_t *TargetObject, int delay)
{
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 100);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, true);
    lv_anim_start(&PropertyAnimation_0);

}

static void icons_back_event_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED ) {
        Fadeon_Animation(ui_meun_group, 200);
        Fadeoff_Animation(lv_obj_get_parent(target), 0);
        _ui_flag_modify(lv_obj_get_parent(target), LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    }

}

//显示回退图标
static void display_icons_back(lv_obj_t *parent)
{
    /*Create an image button*/
    imgbtn_icons_back = lv_imgbtn_create(parent);
    lv_obj_set_style_width(imgbtn_icons_back, 20, 0);
    lv_obj_set_style_height(imgbtn_icons_back, 35, 0);
    lv_obj_set_style_bg_opa(imgbtn_icons_back, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(imgbtn_icons_back, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_add_flag(imgbtn_icons_back, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(imgbtn_icons_back, icons_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(imgbtn_icons_back,  LV_ALIGN_RIGHT_MID, 0, 0);

    img_icons_back = lv_img_create(imgbtn_icons_back); //回退图标img
    lv_obj_set_style_img_recolor_opa(img_icons_back, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(img_icons_back, lv_color_hex(0xFF4500), LV_PART_MAIN);
    lv_img_set_src(img_icons_back, &icons_circle);
    lv_obj_align(img_icons_back, LV_ALIGN_CENTER, 0, 0);

}

static void meun_brightness_obj_event_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED ) {
        Fadeoff_Animation(ui_meun_group, 100);
        Fadeon_Animation(ui_brightness_group, 200);
        _ui_flag_modify(ui_brightness_group, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }

}

static void meun_Speaker_obj_event_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED ) {
        Fadeoff_Animation(ui_meun_group, 100);
        Fadeon_Animation(ui_Speaker_group, 200);
        _ui_flag_modify(ui_Speaker_group, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
}

static void meun_Browser_obj_event_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED ) {
        ConsumerControl.press(CONSUMER_CONTROL_HOME);
        ConsumerControl.release();
        Fadeoff_Animation(ui_meun_group, 100);
        Fadeon_Animation(ui_Browser_group, 200);
        _ui_flag_modify(ui_Browser_group, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
}

static void meun_clock_obj_event_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED ) {
        Fadeoff_Animation(ui_meun_group, 100);
        Fadeon_Animation(ui_clock_group, 200);
        _ui_flag_modify(ui_clock_group, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
}




static void meun_calculator_obj_event_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);     lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED ) {
        ConsumerControl.press(CONSUMER_CONTROL_CALCULATOR);
        ConsumerControl.release();
    }
}

static void meun_sleep_obj_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        // LV_LOG_USER("sleep");
        pinMode(48, INPUT_PULLDOWN);
        pinMode(18, INPUT_PULLDOWN);
        pinMode(17, INPUT_PULLDOWN);
        pinMode(16, INPUT_PULLDOWN);
        pinMode(15, INPUT_PULLDOWN);
        pinMode(TFT2_CS, INPUT_PULLUP);
        pinMode(TFT1_CS, INPUT_PULLUP);
        pinMode(INT_N_PIN, INPUT_PULLUP);

        //触摸屏唤醒  Touch screen wake up
        pinMode(45, OUTPUT);
        digitalWrite(45, HIGH);
        gpio_deep_sleep_hold_en();
        gpio_hold_en(GPIO_NUM_45);
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_12, ESP_EXT1_WAKEUP_ALL_LOW); //1 = High, 0 = Low
        ft6336u.monitor_mdoe();

        //IO0唤醒  IO0 wake up
        // gpio_deep_sleep_hold_dis();
        // gpio_hold_en(GPIO_NUM_0);
        // pinMode(0, INPUT_PULLUP);
        // esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0); //1 = High, 0 = Low

        delay(1000);
        //Go to sleep now
        Serial.println("Going to sleep now");
        esp_deep_sleep_start();
        Serial.println("This will never be printed");
    }
}

static void brightness_up_event_cb(lv_event_t *e)
{

    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_PRESSING ) {
        ConsumerControl.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
        ConsumerControl.release();
    }
}

static void brightness_down_event_cb(lv_event_t *e)
{

    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);     if (event_code == LV_EVENT_PRESSING ) {
        ConsumerControl.press(CONSUMER_CONTROL_BRIGHTNESS_DECREMENT);
        ConsumerControl.release();
    }
}

static void Speaker_up_event_cb(lv_event_t *e)
{

    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);     if (event_code == LV_EVENT_PRESSING ) {
        ConsumerControl.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
        ConsumerControl.release();
    }
}

static void Speaker_down_event_cb(lv_event_t *e)
{

    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);     if (event_code == LV_EVENT_PRESSING) {
        ConsumerControl.press(CONSUMER_CONTROL_VOLUME_DECREMENT);
        ConsumerControl.release();
    }
}

static void Browser_forward_event_cb(lv_event_t *e)
{

    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_PRESSING) {
        ConsumerControl.press(CONSUMER_CONTROL_FORWARD);
        ConsumerControl.release();
    }
}

static void Browser_back_event_cb(lv_event_t *e)
{

    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);     if (event_code == LV_EVENT_PRESSING) {
        ConsumerControl.press(CONSUMER_CONTROL_BACK);
        ConsumerControl.release();
    }
}

static void Browser_refresh_event_cb(lv_event_t *e)
{

    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);     if (event_code == LV_EVENT_PRESSING) {
        ConsumerControl.press(CONSUMER_CONTROL_REFRESH);
        ConsumerControl.release();
    }
}
static void Browser_search_event_cb(lv_event_t *e)
{

    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);     if (event_code == LV_EVENT_PRESSING) {
        ConsumerControl.press(CONSUMER_CONTROL_SEARCH);
        ConsumerControl.release();
    }
}

void ui_screen_init(void)
{
    meun_obj_bg  = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(meun_obj_bg, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);//背景颜色黑色
    lv_obj_set_style_bg_opa(meun_obj_bg, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_meun_group = lv_obj_create(meun_obj_bg); //创建菜单对象组
    lv_obj_set_style_bg_color(ui_meun_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);//背景黑色
    lv_obj_set_style_bg_opa(ui_meun_group, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_meun_group, 0, LV_PART_MAIN | LV_STATE_DEFAULT); //设置方框的半径为0
    lv_obj_set_style_border_color(ui_meun_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(ui_meun_group, 320, 40);                   /*Set the size of the parent 设置对象尺寸*/
    lv_obj_set_pos(ui_meun_group, 0, 0);                       /*Set the position of the new object 设置对象位置*/
    lv_obj_set_style_bg_opa(ui_meun_group, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);//滚动条的颜色默认为透明
    lv_obj_set_style_bg_opa(ui_meun_group, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);//滚动条滚动时的颜色也设置为透明
    lv_obj_align(ui_meun_group, LV_ALIGN_TOP_LEFT, 0, 0);

    //菜单小组件的风格
    static lv_style_t icon_bg_style_def;
    lv_style_init(&icon_bg_style_def);
    lv_style_set_width(&icon_bg_style_def, 60);
    lv_style_set_height(&icon_bg_style_def, 30);
    lv_style_set_bg_opa(&icon_bg_style_def, LV_OPA_COVER);
    lv_style_set_bg_color(&icon_bg_style_def, lv_color_hex(0x0000));
    lv_style_set_radius(&icon_bg_style_def, 5);//设置方框的半径为5
    lv_style_set_border_width(&icon_bg_style_def, 0);
    lv_style_set_border_color(&icon_bg_style_def, lv_color_hex(0xFF8000)); //边框的颜色

    //控制小组件的风格
    static lv_style_t control_style_def;
    lv_style_init(&control_style_def);
    lv_style_set_width(&control_style_def, 90);
    lv_style_set_height(&control_style_def, 30);
    lv_style_set_bg_opa(&control_style_def, LV_OPA_COVER);
    lv_style_set_bg_color(&control_style_def, lv_color_hex(0x0000));
    lv_style_set_radius(&control_style_def, 5);//设置方框的半径为5
    lv_style_set_border_width(&control_style_def, 2);
    lv_style_set_border_color(&control_style_def, lv_color_hex(0x00FFFF));//边框的颜色


    /*Create the brightness component in the menu 在菜单创建亮度组件*/
    meun_brightness_obj = lv_imgbtn_create(ui_meun_group);
    lv_obj_add_style(meun_brightness_obj, &icon_bg_style_def, 0);
    lv_obj_add_event_cb(meun_brightness_obj, meun_brightness_obj_event_cb, LV_EVENT_CLICKED, NULL);//按下回调
    lv_obj_align(meun_brightness_obj, LV_ALIGN_LEFT_MID, 5, 0);

    meun_brightness_img = lv_img_create(meun_brightness_obj);
    lv_obj_set_style_img_recolor_opa(meun_brightness_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(meun_brightness_img, lv_color_hex(0xFF7F24), LV_PART_MAIN);//修改img图像的颜色
    lv_img_set_src(meun_brightness_img, &icons_brightness);
    lv_obj_align(meun_brightness_img, LV_ALIGN_CENTER, 0, 0);

    /*Create the speaker Assembly in the menu 在菜单创建扬声器组件*/
    meun_Speaker_obj = lv_imgbtn_create(ui_meun_group);
    lv_obj_add_style(meun_Speaker_obj, &icon_bg_style_def, 0);
    lv_obj_add_event_cb(meun_Speaker_obj, meun_Speaker_obj_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(meun_Speaker_obj, LV_ALIGN_LEFT_MID, 5 + 80, 0);

    meun_Speaker_img = lv_img_create(meun_Speaker_obj);
    lv_obj_set_style_img_recolor_opa(meun_Speaker_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(meun_Speaker_img, lv_color_hex(0x00BFFF), LV_PART_MAIN);
    lv_img_set_src(meun_Speaker_img, &icons_Speaker);
    lv_obj_align(meun_Speaker_img, LV_ALIGN_CENTER, 0, 0);

    /*Create the browser component in the menu 在菜单创建浏览器组件*/
    meun_Browser_obj = lv_imgbtn_create(ui_meun_group);
    lv_obj_add_style(meun_Browser_obj, &icon_bg_style_def, 0);
    lv_obj_add_event_cb(meun_Browser_obj, meun_Browser_obj_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(meun_Browser_obj, LV_ALIGN_LEFT_MID, 5 + 80 * 2, 0);

    meun_Browser_img = lv_img_create(meun_Browser_obj);
    lv_obj_set_style_img_recolor_opa(meun_Browser_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(meun_Browser_img, lv_color_hex(0xBF3EFF), LV_PART_MAIN);
    lv_img_set_src(meun_Browser_img, &icons_Browser);
    lv_obj_align(meun_Browser_img, LV_ALIGN_CENTER, 0, 0);

    /*Create a clock component in the menu 在菜单创建时钟组件*/
    meun_clock_obj = lv_imgbtn_create(ui_meun_group);
    lv_obj_add_style(meun_clock_obj, &icon_bg_style_def, 0);
    lv_obj_add_event_cb(meun_clock_obj, meun_clock_obj_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(meun_clock_obj, LV_ALIGN_LEFT_MID, 5 + 80 * 3, 0);

    meun_clock_img = lv_img_create(meun_clock_obj);
    lv_obj_set_style_img_recolor_opa(meun_clock_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(meun_clock_img, lv_color_hex(0x00FF7F), LV_PART_MAIN);
    lv_img_set_src(meun_clock_img, &icons_clock);
    lv_obj_align(meun_clock_img, LV_ALIGN_CENTER, 0, 0);

    /*Create the calculator component in the menu 在菜单创建计算器组件*/
    meun_calculator_obj = lv_imgbtn_create(ui_meun_group);
    lv_obj_add_style(meun_calculator_obj, &icon_bg_style_def, 0);
    lv_obj_add_event_cb(meun_calculator_obj, meun_calculator_obj_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(meun_calculator_obj, LV_ALIGN_LEFT_MID, 5 + 80 * 4, 0);

    meun_calculator_img = lv_img_create(meun_calculator_obj);
    lv_obj_set_style_img_recolor_opa(meun_calculator_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(meun_calculator_img, lv_color_hex(0xAB82FF), LV_PART_MAIN);
    lv_img_set_src(meun_calculator_img, &icons_calculator);
    lv_obj_align(meun_calculator_img, LV_ALIGN_CENTER, 0, 0);


    /*Create the sleep component in the menu 在菜单创建休眠组件*/
    meun_sleep_obj = lv_imgbtn_create(ui_meun_group);
    lv_obj_add_style(meun_sleep_obj, &icon_bg_style_def, 0);
    lv_obj_add_event_cb(meun_sleep_obj, meun_sleep_obj_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(meun_sleep_obj, LV_ALIGN_LEFT_MID, 5 + 80 * 5, 0);

    meun_sleep_img = lv_img_create(meun_sleep_obj);
    lv_obj_set_style_img_recolor_opa(meun_sleep_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(meun_sleep_img, lv_color_hex(0x00BFFF), LV_PART_MAIN);
    lv_img_set_src(meun_sleep_img, &icons_sleep);
    lv_obj_align(meun_sleep_img, LV_ALIGN_CENTER, 0, 0);

    //****************************Create a brightness adjustment group 创建亮度调节组****************************
    ui_brightness_group = lv_obj_create(meun_obj_bg); //创建亮度调节组对象
    lv_obj_add_flag(ui_brightness_group, LV_OBJ_FLAG_HIDDEN);   //隐藏这个组（对象）
    lv_obj_set_style_bg_color(ui_brightness_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_brightness_group, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_brightness_group, 0, LV_PART_MAIN | LV_STATE_DEFAULT); //设置方框的半径为0
    lv_obj_set_style_border_color(ui_brightness_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_brightness_group, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(ui_brightness_group, 320, 40);                   /*Set the size of the parent 设置对象尺寸*/
    lv_obj_clear_flag(ui_brightness_group, LV_OBJ_FLAG_SCROLLABLE);//禁用滚动
    lv_obj_align(ui_brightness_group, LV_ALIGN_TOP_LEFT, 0, 0);

    //亮度图标
    img_brightness = lv_img_create(ui_brightness_group);
    lv_obj_set_style_img_recolor_opa(img_brightness, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(img_brightness, lv_color_hex(0xFF7F24), LV_PART_MAIN);//修改img图像的颜色
    lv_img_set_src(img_brightness, &icons_brightness);
    lv_img_set_zoom(img_brightness, 200); //图片缩放
    lv_obj_align(img_brightness, LV_ALIGN_LEFT_MID, 0, 0);

    /*在亮度'up'组件*/
    brightness_up_obj = lv_imgbtn_create(ui_brightness_group);
    lv_obj_add_style(brightness_up_obj, &control_style_def, 0);
    lv_obj_add_event_cb(brightness_up_obj, brightness_up_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_align(brightness_up_obj, LV_ALIGN_LEFT_MID, 50, 0);

    brightness_up_img = lv_img_create(brightness_up_obj);
    lv_obj_set_style_img_recolor_opa(brightness_up_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(brightness_up_img, lv_color_hex(0xEE6363), LV_PART_MAIN);

    lv_img_set_src(brightness_up_img, &icons_up);
    lv_obj_align(brightness_up_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_zoom(brightness_up_img, 200);

    // /*在亮度'down'组件*/
    brightness_down_obj = lv_imgbtn_create(ui_brightness_group);
    lv_obj_add_style(brightness_down_obj, &control_style_def, 0);
    lv_obj_add_event_cb(brightness_down_obj, brightness_down_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_align(brightness_down_obj, LV_ALIGN_LEFT_MID, 150, 0);

    brightness_down_img = lv_img_create(brightness_down_obj);
    lv_obj_set_style_img_recolor_opa(brightness_down_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(brightness_down_img, lv_color_hex(0x6495ED), LV_PART_MAIN);
    lv_img_set_src(brightness_down_img, &icons_down);
    lv_obj_align(brightness_down_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_zoom(brightness_down_img, 200);

    display_icons_back(ui_brightness_group);


    //****************************Create a speaker adjustment group 创建扬声器调节组****************************
    ui_Speaker_group = lv_obj_create(meun_obj_bg);
    lv_obj_add_flag(ui_Speaker_group, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(ui_Speaker_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Speaker_group, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Speaker_group, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Speaker_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Speaker_group, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(ui_Speaker_group, 320, 40);
    lv_obj_set_pos(ui_Speaker_group, 0, 0);
    lv_obj_clear_flag(ui_Speaker_group, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(ui_Speaker_group, LV_ALIGN_TOP_LEFT, 0, 0);

    //扬声器图标
    img_Speaker = lv_img_create(ui_Speaker_group);
    lv_obj_set_style_img_recolor_opa(img_Speaker, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(img_Speaker, lv_color_hex(0x00BFFF), LV_PART_MAIN);
    lv_img_set_src(img_Speaker, &icons_Speaker);
    lv_img_set_zoom(img_Speaker, 200);
    lv_obj_align(img_Speaker, LV_ALIGN_LEFT_MID, 0, 0);

    /*在扬声器'up'组件*/
    Speaker_up_obj = lv_imgbtn_create(ui_Speaker_group);
    lv_obj_add_style(Speaker_up_obj, &control_style_def, 0);
    lv_obj_add_event_cb(Speaker_up_obj, Speaker_up_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_align(Speaker_up_obj, LV_ALIGN_LEFT_MID, 50, 0);

    Speaker_up_img = lv_img_create(Speaker_up_obj);
    lv_obj_set_style_img_recolor_opa(Speaker_up_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(Speaker_up_img, lv_color_hex(0xEE6363), LV_PART_MAIN);
    lv_img_set_src(Speaker_up_img, &icons_up);
    lv_obj_align(Speaker_up_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_zoom(Speaker_up_img, 200);

    /*在扬声器'down'组件*/
    Speaker_down_obj = lv_imgbtn_create(ui_Speaker_group);
    lv_obj_add_style(Speaker_down_obj, &control_style_def, 0);
    lv_obj_add_event_cb(Speaker_down_obj, Speaker_down_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_align(Speaker_down_obj, LV_ALIGN_LEFT_MID, 150, 0);

    Speaker_down_img = lv_img_create(Speaker_down_obj);
    lv_obj_set_style_img_recolor_opa(Speaker_down_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(Speaker_down_img, lv_color_hex(0x6495ED), LV_PART_MAIN);
    lv_img_set_src(Speaker_down_img, &icons_down);
    lv_obj_align(Speaker_down_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_zoom(Speaker_down_img, 200);

    display_icons_back(ui_Speaker_group);

    //****************************Creating a Browser Group 创建浏览器组****************************
    ui_Browser_group = lv_obj_create(meun_obj_bg);
    lv_obj_add_flag(ui_Browser_group, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(ui_Browser_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Browser_group, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Browser_group, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Browser_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Browser_group, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(ui_Browser_group, 320, 40);
    lv_obj_set_pos(ui_Browser_group, 0, 0);
    lv_obj_clear_flag(ui_Browser_group, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(ui_Browser_group, LV_ALIGN_TOP_LEFT, 0, 0);

    /*在浏览器组创建'search'组件*/
    Browser_search_obj = lv_imgbtn_create(ui_Browser_group);
    lv_obj_add_style(Browser_search_obj, &icon_bg_style_def, 0);
    lv_obj_add_event_cb(Browser_search_obj, Browser_search_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_align(Browser_search_obj, LV_ALIGN_LEFT_MID, 0, 0);

    Browser_search_img = lv_img_create(Browser_search_obj);
    lv_obj_set_style_img_recolor_opa(Browser_search_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(Browser_search_img, lv_color_hex(0xFFA500), LV_PART_MAIN);
    lv_img_set_src(Browser_search_img, &icons_search);
    lv_obj_align(Browser_search_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_zoom(Browser_search_img, 200);

    /*在浏览器组创建'refresh'组件*/
    Browser_refresh_obj = lv_imgbtn_create(ui_Browser_group);
    lv_obj_add_style(Browser_refresh_obj, &icon_bg_style_def, 0);
    lv_obj_add_event_cb(Browser_refresh_obj, Browser_refresh_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_align(Browser_refresh_obj, LV_ALIGN_LEFT_MID, 60, 0);

    Browser_refresh_img = lv_img_create(Browser_refresh_obj);
    lv_obj_set_style_img_recolor_opa(Browser_refresh_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(Browser_refresh_img, lv_color_hex(0x00FFFF), LV_PART_MAIN);
    lv_img_set_src(Browser_refresh_img, &icons_refresh);
    lv_obj_align(Browser_refresh_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_zoom(Browser_refresh_img, 200);

    /*在浏览器组创建'←'组件*/
    Browser_fallback_obj = lv_imgbtn_create(ui_Browser_group);
    lv_obj_add_style(Browser_fallback_obj, &icon_bg_style_def, 0);
    lv_obj_add_event_cb(Browser_fallback_obj, Browser_back_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_align(Browser_fallback_obj, LV_ALIGN_LEFT_MID, 60 * 2 + 5, 0);

    Browser_fallback_img = lv_img_create(Browser_fallback_obj);
    lv_obj_set_style_img_recolor_opa(Browser_fallback_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(Browser_fallback_img, lv_color_hex(0xB23AEE), LV_PART_MAIN);
    lv_img_set_src(Browser_fallback_img, &icons_fallback);
    lv_obj_align(Browser_fallback_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_zoom(Browser_fallback_img, 200);

    /*在浏览器组创建'→'组件*/
    Browser_forward_obj = lv_imgbtn_create(ui_Browser_group);
    lv_obj_add_style(Browser_forward_obj, &icon_bg_style_def, 0);
    lv_obj_add_event_cb(Browser_forward_obj, Browser_forward_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_align(Browser_forward_obj, LV_ALIGN_LEFT_MID, 60 * 3 + 5, 0);

    Browser_forward_img = lv_img_create(Browser_forward_obj);
    lv_obj_set_style_img_recolor_opa(Browser_forward_img, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(Browser_forward_img, lv_color_hex(0x00B2EE), LV_PART_MAIN);
    lv_img_set_src(Browser_forward_img, &icons_forward);
    lv_obj_align(Browser_forward_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_zoom(Browser_forward_img, 200);

    display_icons_back(ui_Browser_group);

    //****************************Creating a clock Group 创建时钟组****************************
    ui_clock_group = lv_obj_create(meun_obj_bg);
    lv_obj_add_flag(ui_clock_group, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(ui_clock_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_clock_group, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_clock_group, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_clock_group, lv_color_hex(0x0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_clock_group, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(ui_clock_group, 320, 40);
    lv_obj_set_pos(ui_clock_group, 0, 0);
    lv_obj_clear_flag(ui_clock_group, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(ui_clock_group, LV_ALIGN_TOP_LEFT, 0, 0);

    clock_date_obj = lv_obj_create(ui_clock_group);
    lv_obj_set_style_bg_opa(clock_date_obj, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(clock_date_obj, lv_color_hex(0x0000), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(clock_date_obj, 5, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(clock_date_obj, 2, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(clock_date_obj, lv_color_hex(0x00F5FF), LV_STATE_DEFAULT);
    lv_obj_set_size(clock_date_obj, 130, 35);
    lv_obj_clear_flag(clock_date_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(clock_date_obj, LV_ALIGN_LEFT_MID, 0, 0);

    //时间字符
    clock_date_label = lv_label_create(ui_clock_group);
    lv_obj_set_style_text_font(clock_date_label, &font_alibaba_26, LV_STATE_DEFAULT);
    lv_label_set_recolor(clock_date_label, true);
    lv_obj_set_width(clock_date_label, 130);
    lv_label_set_text_fmt(clock_date_label, "#00F5FF %s# ", time_String);
    lv_obj_align(clock_date_label, LV_ALIGN_LEFT_MID, 3, 0);

    clock_obj = lv_obj_create(ui_clock_group);
    lv_obj_set_style_bg_opa(clock_obj, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(clock_obj, lv_color_hex(0x0000), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(clock_obj, 5, LV_STATE_DEFAULT); //设置方框的半径为5
    lv_obj_set_style_border_width(clock_obj, 2, LV_STATE_DEFAULT);
    lv_obj_set_size(clock_obj, 110, 35);                   /*Set the size of the parent 设置对象尺寸*/
    lv_obj_set_style_border_color(clock_obj, lv_color_hex(0xFF7F24), LV_STATE_DEFAULT);
    lv_obj_clear_flag(clock_obj, LV_OBJ_FLAG_SCROLLABLE);//禁用滚动
    lv_obj_align(clock_obj, LV_ALIGN_LEFT_MID, 155, 0);

    clock_label = lv_label_create(clock_obj);
    lv_obj_set_style_text_font(clock_label, &font_alibaba_26, LV_STATE_DEFAULT);
    lv_label_set_recolor(clock_label, true);
    lv_obj_set_width(clock_label, 110);
    lv_label_set_text_fmt(clock_label, "#FF7F24 %s# ", date_String); //
    lv_obj_align(clock_label, LV_ALIGN_LEFT_MID, 0, 0);

    display_icons_back(ui_clock_group);

}