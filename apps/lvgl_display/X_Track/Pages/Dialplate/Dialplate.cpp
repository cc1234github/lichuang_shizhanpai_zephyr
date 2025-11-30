#include "Dialplate.h"
#include "misc/lv_timer.h" 
#include <zephyr/logging/log.h>

using namespace Page;
LOG_MODULE_REGISTER(dialplate,CONFIG_LOG_DEFAULT_LEVEL);
// 构造函数
Dialplate::Dialplate()
    : timer(nullptr)
    , recState(RECORD_STATE_READY)
    // , lastFocus(nullptr)
    , is_view_active(false)
{
}
// 析构函数
Dialplate::~Dialplate()
{
}

/**************************************************************************
    \brief   自定义属性配置
**************************************************************************/
void Dialplate::onCustomAttrConfig()
{
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE);// 禁用加载动画
}

/**************************************************************************
    \brief   视图加载
**************************************************************************/
void Dialplate::onViewLoad()
{
    Model.Init();// 初始化模型
    View.Create(_root);// 创建视图

    LOG_INF("=== Attaching events ===");
    LOG_INF("btnMap: %p", View.ui.btnCont.btnMap);
    LOG_INF("btnRec: %p", View.ui.btnCont.btnRec);
    LOG_INF("btnMenu: %p", View.ui.btnCont.btnMenu);

    AttachEvent(View.ui.btnCont.btnMap);// 绑定按钮事件
    AttachEvent(View.ui.btnCont.btnRec);// 绑定按钮事件
    AttachEvent(View.ui.btnCont.btnMenu);// 绑定按钮事件

    LOG_INF("Events attached successfully");
}

/**************************************************************************
    \brief   视图加载完成
**************************************************************************/
void Dialplate::onViewDidLoad()
{

}

/**************************************************************************
    \brief   视图即将出现
**************************************************************************/
void Dialplate::onViewWillAppear()
{
    // // 等待当前输入设备释放
    // lv_indev_wait_release(lv_indev_get_act());

    // // 获取默认组
    // lv_group_t* group = lv_group_get_default();
    // LV_ASSERT_NULL(group);

    // // 禁用组的环绕
    // lv_group_set_wrap(group, false);

    // // 将按钮添加到组中
    // lv_group_add_obj(group, View.ui.btnCont.btnMap);
    // lv_group_add_obj(group, View.ui.btnCont.btnRec);
    // lv_group_add_obj(group, View.ui.btnCont.btnMenu);

    // // 恢复上次的焦点，或默认聚焦到开始按钮
    // if (lastFocus){
    //     lv_group_focus_obj(lastFocus);
    // }
    // else{
    //     lv_group_focus_obj(View.ui.btnCont.btnRec);
    // }

    // 设置状态栏样式为透明
    Model.SetStatusBarStyle(DataProc::STATUS_BAR_STYLE_TRANSP);

    is_view_active = true;

    // 更新显示数据
    Update();

    // 启动视图出现动画
    View.AppearAnimStart();
}

/**************************************************************************
    \brief   视图出现完成
**************************************************************************/
void Dialplate::onViewDidAppear()
{
    // 启动定时器，每秒更新一次运动数据
    timer = lv_timer_create(onTimerUpdate, 1000, this);
}

/**************************************************************************
    \brief   视图即将隐藏
**************************************************************************/
void Dialplate::onViewWillDisappear()
{
    // lv_group_t* group = lv_group_get_default();
    // LV_ASSERT_NULL(group);
    // lastFocus = lv_group_get_focused(group);// 保存当前焦点
    // lv_group_remove_all_objs(group);        // 从输入组移除所有对象
    if (timer != nullptr)                   // 删除定时器
    { 
        lv_timer_del(timer);
        timer = nullptr;
    }                  
    // View.AppearAnimStart(true);             // 开始消失动画（参数 true 表示反向）
}

/**************************************************************************
    \brief   视图隐藏完成
**************************************************************************/
void Dialplate::onViewDidDisappear()
{
    is_view_active = false;
}

/**************************************************************************
    \brief   视图卸载
**************************************************************************/
void Dialplate::onViewUnload()
{
    Model.Deinit();
    View.Delete();
}
/**************************************************************************
    \brief   视图卸载完成
**************************************************************************/
void Dialplate::onViewDidUnload()
{
}

/**************************************************************************
    \brief    为 UI 对象绑定事件处理函数
**************************************************************************/
void Dialplate::AttachEvent(lv_obj_t* obj)
{
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

/**************************************************************************
    \brief   更新显示数据
**************************************************************************/
void Dialplate::Update()
{
    char buf[16] = {0};

    // 更新速度显示
    float speed = Model.GetSpeed();
    lv_label_set_text_fmt(View.ui.topInfo.labelSpeed, "%02d",(int)speed);

    // 更新底部信息显示  平均速度
    float avgSpeed = Model.GetAvgSpeed();
    lv_label_set_text_fmt(View.ui.bottomInfo.labelInfoGrp[0].lableValue, "%.1f km/h", (double)avgSpeed);
    // 运动时间
    lv_label_set_text(
        View.ui.bottomInfo.labelInfoGrp[1].lableValue,
        DataProc::MakeTimeString(Model.sportStatusInfo.singleTime, buf, sizeof(buf))
    );
    // 运动距离
    lv_label_set_text_fmt(
        View.ui.bottomInfo.labelInfoGrp[2].lableValue,
        "%0.1f km",
        (double)(Model.sportStatusInfo.singleDistance / 1000.0f)
    );
    // 卡路里
    lv_label_set_text_fmt(
        View.ui.bottomInfo.labelInfoGrp[3].lableValue,
        "%d k",
        int(Model.sportStatusInfo.singleCalorie)
    );
}

/**************************************************************************
    \brief   定时器回调函数，用于更新显示数据
**************************************************************************/
void Dialplate::onTimerUpdate(lv_timer_t* timer)
{
    if (timer == nullptr) 
        return;
    
    Dialplate* instance = (Dialplate*)lv_timer_get_user_data(timer);
    LV_ASSERT_NULL(instance);
    instance->Update();
}

/**************************************************************************
    \brief   按钮点击事件处理函数
**************************************************************************/
void Dialplate::onBtnClicked(lv_obj_t* btn)
{
    if (btn == View.ui.btnCont.btnMap)
    {
        _Manager->Push("Pages/LiveMap");
        
        LOG_INF("LOAD LiveMap ");
    }
    else if (btn == View.ui.btnCont.btnMenu)
    {
        _Manager->Push("Pages/SystemInfos"); // 跳转到系统信息页面
        LOG_INF("LOAD SystemInfos ");
    }
}

/**************************************************************************
    \brief   记录按钮点击事件处理函数
**************************************************************************/
void Dialplate::onRecord(bool longPress)
{
    switch (recState)
    {
    case RECORD_STATE_READY:
        if (longPress)
        {
            if (!Model.GetGPSReady())// 检查 GPS 是否就绪
            { 
                LV_LOG_WARN("GPS has not ready, can't start record");
                Model.PlayMusic("Error");
                return;
            }

            Model.PlayMusic("Connect");
            Model.RecorderCommand(Model.REC_START);
            SetBtnRecImgSrc("pause");
            recState = RECORD_STATE_RUN;// 切换到运行状态
        }
        break;
    case RECORD_STATE_RUN:
        if (!longPress) // 短按暂停记录
        {
            Model.PlayMusic("UnstableConnect");
            Model.RecorderCommand(Model.REC_PAUSE);
            SetBtnRecImgSrc("start");
            recState = RECORD_STATE_PAUSE;
        }
        break;
    case RECORD_STATE_PAUSE:
        if (longPress) // 长按准备停止记录
        {
            Model.PlayMusic("NoOperationWarning");
            SetBtnRecImgSrc("stop");
            Model.RecorderCommand(Model.REC_READY_STOP);
            recState = RECORD_STATE_STOP;
        }
        else// 短按继续记录
        {
            Model.PlayMusic("Connect");
            Model.RecorderCommand(Model.REC_CONTINUE);
            SetBtnRecImgSrc("pause");
            recState = RECORD_STATE_RUN;
        }
        break;
    case RECORD_STATE_STOP:
        if (longPress)// 长按确认停止
        {
            Model.PlayMusic("Disconnect");
            Model.RecorderCommand(Model.REC_STOP);
            SetBtnRecImgSrc("start");
            recState = RECORD_STATE_READY;
        }
        else// 短按取消停止，继续录制
        {
            Model.PlayMusic("Connect");
            Model.RecorderCommand(Model.REC_CONTINUE);
            SetBtnRecImgSrc("pause");
            recState = RECORD_STATE_RUN;
        }
        break;
    default:
        break;
    }
}

/**************************************************************************
    \brief   设置记录按钮的图片源
**************************************************************************/
void Dialplate::SetBtnRecImgSrc(const char* srcName)
{
    lv_obj_set_style_bg_img_src(View.ui.btnCont.btnRec, ResourcePool::GetImage(srcName), 0);
}

/**************************************************************************
    \brief   事件处理函数
**************************************************************************/
void Dialplate::onEvent(lv_event_t* event)
{
    // 获取实例指针（从事件用户数据中）
    Dialplate* instance = (Dialplate*)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);
    if (instance == nullptr || !instance->is_view_active) {  
        return;
    }

    // 获取触发事件的对象
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_current_target(event);
    lv_event_code_t code = lv_event_get_code(event);

    
    // ? 只处理需要的事件，其他事件直接忽略
    if (code != LV_EVENT_SHORT_CLICKED && code != LV_EVENT_LONG_PRESSED) {
        return;  // 不处理也不打印其他事件
    }

    // 现在只有 SHORT_CLICKED 和 LONG_PRESSED 会执行到这里
    LOG_INF("=== Event received: code=%d, obj=%p ===", code, obj);

    const char* btn_name = "UNKNOWN";
    if (obj == instance->View.ui.btnCont.btnMap) {
        btn_name = "MAP";
    } else if (obj == instance->View.ui.btnCont.btnRec) {
        btn_name = "REC";
    } else if (obj == instance->View.ui.btnCont.btnMenu) {
        btn_name = "MENU";
    }
    
    LOG_INF("Button: %s, Event: %d", btn_name, code);

    // 处理短按事件
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        instance->onBtnClicked(obj);
    }

    // 处理记录按钮的长按和短按事件
    if (obj == instance->View.ui.btnCont.btnRec)
    {
        if (code == LV_EVENT_SHORT_CLICKED)
        {
            instance->onRecord(false);
        }
        else if (code == LV_EVENT_LONG_PRESSED)
        {
            instance->onRecord(true);
        }
    }
}
