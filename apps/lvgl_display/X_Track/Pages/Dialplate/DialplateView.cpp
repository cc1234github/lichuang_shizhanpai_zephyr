#include "DialplateView.h"
#include <stdarg.h>
#include <stdio.h>
#include <zephyr/logging/log.h>
#define ARRAY_SIZE(arr) (int(sizeof(arr) / sizeof(arr[0])))
LOG_MODULE_REGISTER(dialplate_view,CONFIG_LOG_DEFAULT_LEVEL);
using namespace Page;

/**************************************************************************
    \brief   创建整个界面
**************************************************************************/
void DialplateView::Create(lv_obj_t *root)
{
    // === 第一步：初始化根对象 ===

    lv_obj_remove_style_all(root);                          // 移除所有样式
    lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);          // 设置为全屏
    lv_obj_set_style_bg_color(root, lv_color_black(), 0);   // 设置背景颜色为黑色
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);         // 设置背景透明度为完全不透明

    // === 第二步：创建三个主要区域 ===

    BottomInfo_Create(root);// 底部信息区（4个数据项）
    TopInfo_Create(root);   // 顶部速度显示区
    BtnCont_Create(root);   // 底部按钮区（3个按钮）

    // === 第三步：创建入场动画时间线 ===
    ui.anim_timeline = lv_anim_timeline_create();

    // 动画定义宏
#define ANIM_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, (lv_anim_exec_xcb_t)lv_obj_set_##attr, start, end, 500, lv_anim_path_ease_out, true}

    // 透明度动画宏
#define ANIM_OPA_DEF(start_time, obj) \
    {start_time, obj, (lv_anim_exec_xcb_t)lv_obj_set_style_opa, LV_OPA_TRANSP, LV_OPA_COVER}

    // 记录目标位置（动画结束位置）
    lv_coord_t y_tar_top = lv_obj_get_y(ui.topInfo.cont);
    lv_coord_t y_tar_bottom = lv_obj_get_y(ui.bottomInfo.cont);
    lv_coord_t h_tar_btn = lv_obj_get_height(ui.btnCont.btnRec);

    // 动画序列定义
    lv_anim_timeline_wrapper_t wrapper[] =
        {
            // 0ms: 顶部信息从上方滑入
            ANIM_DEF(0, ui.topInfo.cont, y, -lv_obj_get_height(ui.topInfo.cont), y_tar_top),
            // // 200ms: 底部信息从上方滑入 + 淡入
            ANIM_DEF(200, ui.bottomInfo.cont, y, -lv_obj_get_height(ui.bottomInfo.cont), y_tar_bottom),
            ANIM_OPA_DEF(200, ui.bottomInfo.cont),

            // 500ms, 600ms, 700ms: 三个按钮依次从 0 高度展开
            ANIM_DEF(500, ui.btnCont.btnMap, height, 0, h_tar_btn),
            ANIM_DEF(600, ui.btnCont.btnRec, height, 0, h_tar_btn),
            ANIM_DEF(700, ui.btnCont.btnMenu, height, 0, h_tar_btn),
            LV_ANIM_TIMELINE_WRAPPER_END
        };
    // 将动画序列添加到时间线
    lv_anim_timeline_add_wrapper(ui.anim_timeline, wrapper);

    #undef ANIM_DEF
    #undef ANIM_OPA_DEF
}

void DialplateView::Delete()
{
    if (ui.anim_timeline)
    {
        lv_anim_timeline_delete(ui.anim_timeline);
        ui.anim_timeline = nullptr;
    }
}

/**************************************************************************
    \brief   TopInfo_Create() - 创建顶部速度显示区
**************************************************************************/
void DialplateView::TopInfo_Create(lv_obj_t *par)
{
    // === 创建容器 ===
    lv_obj_t *cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);// 移除所有样式
    lv_obj_set_size(cont, LV_HOR_RES, 142);// 设置容器大小为全屏宽度，高度为 142px

    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);// 设置背景透明度为完全不透明
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x333333), 0);// 设置背景颜色为深灰色

    lv_obj_set_style_radius(cont, 27, 0);// 设置圆角半径为 27px
    lv_obj_set_y(cont, -36);// 设置容器初始位置，使其从上方滑入
    ui.topInfo.cont = cont;// 记录容器对象

    // === 创建速度数字标签 ===
    lv_obj_t *label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("bahnschrift_65"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text(label, "00");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 63);
    ui.topInfo.labelSpeed = label;

    // === 创建单位标签 ===
    label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("bahnschrift_17"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text(label, "km/h");
    lv_obj_align_to(label, ui.topInfo.labelSpeed, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    ui.topInfo.labelUint = label;
}

/**************************************************************************
    \brief   BottomInfo_Create() - 创建底部信息区（4个数据项）
**************************************************************************/
void DialplateView::BottomInfo_Create(lv_obj_t *par)
{
    // === 创建容器 ===
    lv_obj_t *cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_size(cont, LV_HOR_RES, 90);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 106);

    // === 使用 Flexbox 布局 ===
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    lv_obj_set_flex_align(
        cont,
        LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER);

    ui.bottomInfo.cont = cont;

    // === 创建 4 个信息组 ===
    const char *unitText[4] =
        {
            "AVG",
            "Time",
            "Trip",
            "Calorie"
			};

    for (int i = 0; i < ARRAY_SIZE(ui.bottomInfo.labelInfoGrp); i++)
    {
        SubInfoGrp_Create(
            cont,
            &(ui.bottomInfo.labelInfoGrp[i]),
            unitText[i]
			);
    }
}

/**************************************************************************
    \brief   SubInfoGrp_Create() - 创建底部信息组（包含数值和单位）
**************************************************************************/
void DialplateView::SubInfoGrp_Create(lv_obj_t *par, SubInfo_t *info, const char *unitText)
{
    // === 创建信息组容器 ===
    lv_obj_t *cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 93, 39);

    // === 使用垂直 Flexbox 布局 ===
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        cont,
        LV_FLEX_ALIGN_SPACE_AROUND,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
		);

    // === 创建数值标签（上方） ===
    lv_obj_t *label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("bahnschrift_17"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    info->lableValue = label;

    // === 创建单位标签（下方） ===
    label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("bahnschrift_13"), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xb3b3b3), 0);
    lv_label_set_text(label, unitText);
    info->lableUnit = label;

    info->cont = cont;
}

/**************************************************************************
    \brief   BtnCont_Create() - 创建底部按钮区（3个按钮）
**************************************************************************/
void DialplateView::BtnCont_Create(lv_obj_t *par)
{
    lv_obj_t *cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_HOR_RES, 40);
    lv_obj_align_to(cont, ui.bottomInfo.cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    /*lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_place(
        cont,
        LV_FLEX_PLACE_SPACE_AROUND,
        LV_FLEX_PLACE_CENTER,
        LV_FLEX_PLACE_CENTER
    );*/

    ui.btnCont.cont = cont;

    // === 创建三个按钮 ===
    ui.btnCont.btnMap = Btn_Create(cont, ResourcePool::GetImage("locate"), -80);
    ui.btnCont.btnRec = Btn_Create(cont, ResourcePool::GetImage("start"), 0);
    ui.btnCont.btnMenu = Btn_Create(cont, ResourcePool::GetImage("menu"), 80);
}

// /**************************************************************************
//     \brief   Btn_Create() - 创建底部按钮（通用）
// **************************************************************************/
// lv_obj_t *DialplateView::Btn_Create(lv_obj_t *par, const void *img_src, lv_coord_t x_ofs)
// {
//     lv_obj_t *obj = lv_obj_create(par);
//     lv_obj_remove_style_all(obj);
//     lv_obj_set_size(obj, 40, 31);
//     lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

//     lv_obj_align(obj, LV_ALIGN_CENTER, x_ofs, 0);
//     lv_obj_set_style_bg_img_src(obj, img_src, 0);

//     lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
//     lv_obj_set_style_width(obj, 45, LV_STATE_PRESSED);
//     lv_obj_set_style_height(obj, 25, LV_STATE_PRESSED);
//     lv_obj_set_style_bg_color(obj, lv_color_hex(0x666666), 0);
//     lv_obj_set_style_bg_color(obj, lv_color_hex(0xbbbbbb), LV_STATE_PRESSED);
//     lv_obj_set_style_bg_color(obj, lv_color_hex(0xff931e), LV_STATE_FOCUSED);
//     lv_obj_set_style_radius(obj, 9, 0);

//     static lv_style_transition_dsc_t tran;
//     static const lv_style_prop_t prop[] = {LV_STYLE_WIDTH, LV_STYLE_HEIGHT, LV_STYLE_PROP_INV};
//     lv_style_transition_dsc_init(
//         &tran,
//         prop,
//         lv_anim_path_ease_out,
//         200,
//         0,
//         nullptr
// 		);
//     // 应用过渡效果到按下和聚焦状态
//     lv_obj_set_style_transition(obj, &tran, LV_STATE_PRESSED);
//     lv_obj_set_style_transition(obj, &tran, LV_STATE_FOCUSED);

//     lv_obj_update_layout(obj);

//     return obj;
// }

lv_obj_t *DialplateView::Btn_Create(lv_obj_t *par, const void *img_src, lv_coord_t x_ofs)
{
    lv_obj_t *obj = lv_obj_create(par);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 40, 31);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_align(obj, LV_ALIGN_CENTER, x_ofs, 0);
    lv_obj_set_style_bg_img_src(obj, img_src, 0);

    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x666666), 0);
    lv_obj_set_style_radius(obj, 9, 0);
    
    // ? 添加边框，方便看到按钮区域
    lv_obj_set_style_border_width(obj, 2, 0);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xFF0000), 0);  // 红色边框
    lv_obj_set_style_border_opa(obj, LV_OPA_COVER, 0);
    
    // 按下状态
    lv_obj_set_style_width(obj, 45, LV_STATE_PRESSED);
    lv_obj_set_style_height(obj, 25, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xbbbbbb), LV_STATE_PRESSED);
    lv_obj_set_style_border_color(obj, lv_color_hex(0x00FF00), LV_STATE_PRESSED);  // 按下时绿色

    // 过渡动画
    static lv_style_transition_dsc_t tran;
    static const lv_style_prop_t prop[] = {
        LV_STYLE_WIDTH, 
        LV_STYLE_HEIGHT, 
        LV_STYLE_BG_COLOR,
        LV_STYLE_PROP_INV
    };
    lv_style_transition_dsc_init(&tran, prop, lv_anim_path_ease_out, 200, 0, nullptr);
    lv_obj_set_style_transition(obj, &tran, LV_STATE_PRESSED);

    // ? 更新布局并获取实际坐标
    lv_obj_update_layout(obj);
    
    lv_area_t area;
    lv_obj_get_coords(obj, &area);
    
    // ? 打印详细的按钮信息
    LOG_INF("Button created:");
    LOG_INF("  - Address: %p", obj);
    LOG_INF("  - Position: x=%d, y=%d", lv_obj_get_x(obj), lv_obj_get_y(obj));
    LOG_INF("  - Size: %dx%d", lv_obj_get_width(obj), lv_obj_get_height(obj));
    LOG_INF("  - Absolute coords: x1=%d, y1=%d, x2=%d, y2=%d", 
            area.x1, area.y1, area.x2, area.y2);
    LOG_INF("  - Clickable: %s", 
            lv_obj_has_flag(obj, LV_OBJ_FLAG_CLICKABLE) ? "YES" : "NO");

    return obj;
}
/**************************************************************************
    \brief   AppearAnimStart() - 启动出现动画
**************************************************************************/
void DialplateView::AppearAnimStart(bool reverse)
{
    // lv_anim_timeline_set_reverse(ui.anim_timeline, reverse);
    // lv_anim_timeline_start(ui.anim_timeline);
}
