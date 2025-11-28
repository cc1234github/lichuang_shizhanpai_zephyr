#include "DialplateModel.h"


using namespace Page;

/**************************************************************************
    \brief   初始化模型 在 Dialplate::onViewLoad() 时调用
**************************************************************************/
void DialplateModel::Init()
{
    account = new Account("DialplateModel", DataProc::Center(), 0, this);
    // account->Subscribe("SportStatus");
    // account->Subscribe("Recorder");
    // account->Subscribe("StatusBar");
    // account->Subscribe("GPS");
    // account->Subscribe("MusicPlayer");
    account->SetEventCallback(onEvent);
}

void DialplateModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}

/**************************************************************************
    \brief   获取GPS是否就绪 在 Dialplate::onRecord() 中被调用
**************************************************************************/
bool DialplateModel::GetGPSReady()
{
    HAL::GPS_Info_t gps = {0};
    if (account == nullptr) { 
        return false;
    }

    if(account->Pull("GPS", &gps, sizeof(gps)) != Account::RES_OK)
    {
        return false;// GPS 数据获取失败
    }
    return (gps.satellites > 0);// 至少有一个卫星
}

/**************************************************************************
    \brief   处理事件
**************************************************************************/
int DialplateModel::onEvent(Account* account, Account::EventParam_t* param)
{
    if (account == nullptr || param == nullptr) {
        return Account::RES_PARAM_ERROR;
    }

    //1. 检查事件类型
    if (param->event != Account::EVENT_PUB_PUBLISH)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }
    // 2. 检查数据来源和大小
    if (strcmp(param->tran->ID, "SportStatus") != 0
            || param->size != sizeof(HAL::SportStatus_Info_t))
    {
        return Account::RES_PARAM_ERROR;
    }

    // 3. 从 Account 的用户数据中恢复实例指针
    DialplateModel* instance = (DialplateModel*)account->UserData;

    if (instance == nullptr) {
        return Account::RES_PARAM_ERROR;
    }
    if (param->data_p == nullptr) {
        return Account::RES_PARAM_ERROR;
    }
    
    memcpy(&(instance->sportStatusInfo), param->data_p, param->size);

    return Account::RES_OK;
}

/**************************************************************************
    \brief   这个函数控制录制器状态，并更新状态栏显示。
**************************************************************************/
void DialplateModel::RecorderCommand(RecCmd_t cmd)
{
    if (cmd != REC_READY_STOP)
    {
        DataProc::Recorder_Info_t recInfo;
        DATA_PROC_INIT_STRUCT(recInfo);
        recInfo.cmd = (DataProc::Recorder_Cmd_t)cmd;
        recInfo.time = 1000;
        account->Notify("Recorder", &recInfo, sizeof(recInfo));
    }

    DataProc::StatusBar_Info_t statInfo;
    DATA_PROC_INIT_STRUCT(statInfo);
    statInfo.cmd = DataProc::STATUS_BAR_CMD_SET_LABEL_REC;

    switch (cmd)
    {
    case REC_START:
    case REC_CONTINUE:
        statInfo.param.labelRec.show = true;
        statInfo.param.labelRec.str = "REC";
        break;
    case REC_PAUSE:
        statInfo.param.labelRec.show = true;
        statInfo.param.labelRec.str = "PAUSE";
        break;  
    case REC_READY_STOP:
        statInfo.param.labelRec.show = true;
        statInfo.param.labelRec.str = "STOP";
        break;
    case REC_STOP:
        statInfo.param.labelRec.show = false;
        break;
    default:
        break;
    }
    // 通知状态栏更新显示
    account->Notify("StatusBar", &statInfo, sizeof(statInfo));
}

void DialplateModel::PlayMusic(const char* music)
{
    // DataProc::MusicPlayer_Info_t info;
    // DATA_PROC_INIT_STRUCT(info);

    // info.music = music;
    // account->Notify("MusicPlayer", &info, sizeof(info));
}

/**************************************************************************
    \brief   设置状态栏样式
**************************************************************************/
void DialplateModel::SetStatusBarStyle(DataProc::StatusBar_Style_t style)
{
    // DataProc::StatusBar_Info_t info;
    // DATA_PROC_INIT_STRUCT(info);

    // info.cmd = DataProc::STATUS_BAR_CMD_SET_STYLE;
    // info.param.style = style;

    // // 通知状态栏更新样式
    // account->Notify("StatusBar", &info, sizeof(info));
}
