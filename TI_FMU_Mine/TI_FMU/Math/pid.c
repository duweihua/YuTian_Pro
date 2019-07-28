
//PID驱动使用方法如下：
//1.构建一个PIDInfo_t结构体，将所需要控制的数据存放进去；
//2.调UpdatePID函数，计算PID输出结果
//3.可以直接调用ClacCascadePID直接计算串级PID


//外部文件引用
#include "pid.h"
#include "myMath.h"    

//宏定义区

//Extern引用

//私有函数区

//私有变量区
/*PID工程变量*/
PIDInfo_t PIDGroup[emNum_Of_PID_List];


void ResetPID(void)
{
    for(int i = 0; i < emNum_Of_PID_List; i++)
    {
        PIDGroup[i].integ = 0;
        PIDGroup[i].prevError = 0;
        PIDGroup[i].out = 0;
        PIDGroup[i].offset = 0;
    }
    
    PIDGroup[emPID_Height_Pos].desired = 80;
}


void UpdatePID(PIDInfo_t* pid, const float dt)
{
    float deriv;
    
    pid->Err = pid->desired - pid->measured + pid->offset; //当前角度与实际角度的误差

    
    if(pid->Err < pid->DeathArea && pid->Err > -pid->DeathArea)
    {
        pid->Err = 0;
    }
    
    if(pid->Err_LimitHigh != 0 && pid->Err_LimitLow != 0)
    {
        pid->Err = LIMIT(pid->Err, pid->Err_LimitLow, pid->Err_LimitHigh);
    }
    
    pid->integ += pid->Err * dt;    
    
    if(pid->IntegLimitHigh != 0 && pid->IntegLimitLow != 0)
    {
        pid->integ = LIMIT(pid->integ, pid->IntegLimitLow, pid->IntegLimitHigh);
    }
    
    //deriv = (pid->Err - pid->prevError)/dt;  
    deriv = -(pid->measured - pid->prevError)/dt;
		
    pid->out = pid->kp * pid->Err + pid->ki * pid->integ + pid->kd * deriv;//PID输出
    
    if(pid->OutLimitHigh != 0 && pid->OutLimitLow != 0)
    {
        pid->out = LIMIT(pid->out, pid->OutLimitLow, pid->OutLimitHigh);
    }
    
    pid->prevError = pid->measured;//pid->Err;  微分先行（变式）用法
}


//串级PID
void ClacCascadePID(PIDInfo_t* pidRate, PIDInfo_t* pidAngE, const float dt)  //串级PID
{     
    UpdatePID(pidAngE, dt);    //先计算外环
    pidRate->desired = pidAngE->out;
    UpdatePID(pidRate, dt);   
}







