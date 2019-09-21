
//外部文件引用
#include "speed_estimator.h"
#include "height_control.h"
#include "SPL06.h"
#include "control.h"
#include "gcs.h"
#include "myMath.h"

//宏定义区
#define VELOCITY_LIMIT  (130.f)    /*速度限幅 单位cm/s*/

//Extern引用

//私有函数区
float applyDeadbandf(float value, float deadband);

//私有变量区

//===================ANOTC===================
#include "Ano_OF.h"

#define FIX_A1 0.8f
#define FIX_B1 0.5f
#define FIX_C1 0.2f

#define FIX_A2 1.6f
#define FIX_B2 0.5f
#define FIX_C2	0.2f

//wz_fus struct
float fix_rat[3];
_wz_fus_sta_enum wz_fus_sta;

int32_t obs_wz_velocity[2];
int32_t obs_wz_height[2];

int32_t obs_wz_hasl;
int32_t obs_wz_hasl_gnd;

float obs_wz_velocity_ref;
float obs_wz_height_ref;
	
float est_wz_velocity;
float est_wz_height;

float fix_wz_velocity;
float fix_wz_height;
float fix_wz_acceleration;

static uint8_t height_init_f = 10;

static int32_t height_old[2];
bool Select_Baro = false;
void WZ_Obs_Calcu(float dT_s)//跟随OBS数据更新周期
{
	//height[1]
	
	obs_wz_hasl = (int32_t)(g_SPL06Manager.fALT *100);//cm
	
	//
	if(height_init_f!=0)
	{
		//
		height_init_f --;
		//
		obs_wz_hasl_gnd = obs_wz_hasl;
	}
	else
	{
		obs_wz_height[0] = obs_wz_hasl - obs_wz_hasl_gnd;
	}
	//height[2]
    {
        uint16_t tmp = ANO_OF.ALT;
        static uint16_t Alt_Old = 0;
        static float time = 0;
        
        if(tmp < 200)
        {
            obs_wz_height[1] = ANO_OF.ALT;
            Alt_Old = ANO_OF.ALT;
            
            time = 0;
        }else
        {
            obs_wz_height[1] = Alt_Old;
            time += dT_s;
            
            if(time > 3)
            {
                Select_Baro = true;
                ANO_OF.STATE.alt = 0;
            }
        }
    }
	
	//velocity
	obs_wz_velocity[0] = (int32_t)((obs_wz_height[0] - height_old[0])/dT_s);
	obs_wz_velocity[1] = (int32_t)((obs_wz_height[1] - height_old[1])/dT_s);
	
	height_old[0] = obs_wz_height[0];
	height_old[1] = obs_wz_height[1];
	
	//selector
	if(ANO_OF.STATE.alt != 0 && !Select_Baro)
	{
		obs_wz_velocity_ref = obs_wz_velocity[1];
		//
		fix_rat[0] = FIX_A2;
		fix_rat[1] = FIX_B2;
		fix_rat[2] = FIX_C2;
	}
	else
	{
		obs_wz_velocity_ref = obs_wz_velocity[0];
		//
		fix_rat[0] = FIX_A1;
		fix_rat[1] = FIX_B1;
		fix_rat[2] = FIX_C1;
	}
}


void WZ_Est_Calcu(float dT_s)//跟随惯性数据更新周期
{
    //加速度计积分
	est_wz_velocity += HeightInfo.Z_Acc *dT_s;
	
    //速度值积分
	est_wz_height += est_wz_velocity *dT_s;
}


void WZ_Fix_Calcu(float dT_s)//跟随控制周期
{
	//==calcu
	//速度值限幅
	obs_wz_velocity_ref = (obs_wz_velocity_ref > 500) ? 500 : ((obs_wz_velocity_ref < -500 ) ? -500 : obs_wz_velocity_ref);
	
    //计算观测高度积分值
	obs_wz_height_ref += obs_wz_velocity_ref * dT_s;
	
    //修正的高度值 = 修正参数 * （观测高度积分值 - 加速度的积分高度）
	fix_wz_height = fix_rat[0] * ((float)obs_wz_height_ref - est_wz_height);
    
	//修正的速度值 = 修正参数 * （观测速度值 - 加速度的积分速度）
	fix_wz_velocity = fix_rat[1] * ((float)obs_wz_velocity_ref - est_wz_velocity);
    
	//修正的加速度值 = 修正参数 * （修正的速度值）* dt;
	fix_wz_acceleration += fix_rat[2] * (fix_wz_velocity) *dT_s;
    
    //修正的加速度限幅
	fix_wz_acceleration = (fix_wz_acceleration > 50) ? 50 : ((fix_wz_acceleration < -50 ) ? -50 : fix_wz_acceleration);
	
    //
	//==fix
	//
	est_wz_height += fix_wz_height *dT_s;	
	//
	est_wz_velocity += (fix_wz_acceleration + fix_wz_velocity ) *dT_s;
	//==
	HeightInfo.Z_Speed = est_wz_velocity;
	HeightInfo.Z_Postion = est_wz_height;
	
	//
	if(wz_fus_sta == FIX_RESET || g_FMUflg.unlock == 0)
	{
		WZ_Fus_Reset();
		//
		wz_fus_sta = FIX_WORKING;
	}
}

void WZ_Fus_Reset()
{
	obs_wz_height_ref = 0;
	obs_wz_velocity_ref = 0;
	est_wz_height = 0;
}













