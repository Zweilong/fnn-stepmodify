// #include "i2c.h"
// #include "api.h"
// #include <stdio.h>
// #include <unistd.h>
// #include <wiringPi.h>
// #include "dynamixel.h"
// #include <vector>
// #include <time.h>
// #include <stdlib.h>
#include "robotcontrol.h"
#include "ADS1x15.h"
#define loopRate 100 //hz

#define LF_PIN      1
#define RF_PIN      24
#define LH_PIN      28
#define RH_PIN      29
uint8_t svStatus=0b01010101;
// uint8_t svStatus=0b10101010;


int main()
{
      
    /*********adc test********/
    struct timeval startTime,endTime;
    double timeUse;
   // API api;
    vector<int> ID;
    vector<float> start_pos;//分别用于存储电机的 ID 和电机的初始位置。
    vector<float> target_tor;//用于储存目标扭矩值

    // for(int i=0; i<4; i++)
    // {
    // ID.push_back(3*i);
    // ID.push_back(3*i+1);
    // start_pos.push_back(0.00);
    // start_pos.push_back(0.00);
    // }
     for(int i=0; i<16; i++)
    {
    ID.push_back(i);
    start_pos.push_back(0.00);
    }//这个循环运行了 16 次，将 0 到 15 的整数依次加入 ID 向量中，表示系统中共有 16 个电机，每个电机的 ID 分别为 0 到 15。同时，start_pos 向量中添加了 16 个值 0.00，表示每个电机的初始位置为 0。
     DxlAPI gecko("/dev/ttyAMA0", 1000000, ID, 2); //ttyUSB0
//     // gecko.setBaudRate(5);
     gecko.setOperatingMode(3);  //3 position control; 0 current control
     gecko.torqueEnable();
     gecko.setPosition(start_pos);//电机设置为位置控制模式3，使能电机的扭矩，将所有电机的位置设置为start_pos向量中定义的值
   // gecko.getPosition();
//     int times=1;
//     bool reverse=false;
//     while(1)
//     {
//             gecko.getTorque();
//             gecko.getPosition();
//             gecko.getVelocity();
//             cout<<" present postion: ";
//             for(int i=0;i<16;i++)
//             cout<<gecko.present_position[i]<<" ";
//             cout<<endl;
//     }
//     //~ usleep(1e6);
    //  api.setPump(1, LOW);
    //  api.setPump(24, LOW);
    //  api.setPump(28, LOW);
    //  api.setPump(29, LOW);
    // float torque[12];
    // usleep(1e6);
     //api.setSV(svStatus);


    CRobotControl rbt(110.0,60.0,20.0,800.0,ADMITTANCE);//	这行代码通过 CRobotControl 类创建了一个名为 rbt 的机器人控制对象。参数 110.0, 60.0, 20.0, 800.0 分别可能代表机器人的物理参数，如质量、腿长、步长、速度等。
    Matrix<float,4,2> TimeForSwingPhase;
    Matrix<float, 4, 3> InitPos;//4x3矩阵，用于储存每条腿的初始位置
    Matrix<float, 6,1> TCV;//6x1矩阵，用于储存机器人中心质点（CoM）的速度向量，包括在 X、Y 轴上的速度以及绕各轴的角速度。

    TCV << 3.0/1000.0, 0, 0,0,0,0 ;// X, Y , alpha 
   
float  float_initPos[12]={   70.0,65.5,-21.0,
                             70.0,-65.5,-21.0,
                            -84.0, 65.5,-21.0,
                            -84.0, -65.5,-21.0};
    // float  float_initPos[12];
    // string2float("../include/initPos.csv", float_initPos);//Foot end position
    for(int i=0; i<4; i++)
        for(int j=0;j<3;j++)
        {
           
            InitPos(i, j) = float_initPos[i*3+j]/1000;
            // cout<<InitPos(i, j)<<endl;
        }//将初始位置单位转化为米
    rbt.SetInitPos(InitPos);//通过调用 SetInitPos 方法，将四条腿的初始位置设置为 InitPos 矩阵中的值。
    rbt.InverseKinematics(rbt.mfLegCmdPos);//逆向运动学计算
    cout<<"cmdPos: "<<rbt.mfJointCmdPos<<endl;
   // rbt.SetPos(rbt.mfJointCmdPos);
 //  rbt.PumpAllPositve();
    usleep(1e6);
   rbt.SetCoMVel(TCV);//调用 SetCoMVel 方法设置机器人的中心质点（CoM）速度，这里使用的是之前定义的 TCV 向量。
    TimeForSwingPhase<< 8*TimeForGaitPeriod/16, 	11*TimeForGaitPeriod/16,		
                        0,		 		 					3*TimeForGaitPeriod/16,		
                        12*TimeForGaitPeriod/16, 	15*TimeForGaitPeriod/16,		
                        4*TimeForGaitPeriod/16, 	7*TimeForGaitPeriod/16;//TimeForSwingPhase 矩阵定义了每条腿的摆动相在步态周期中的时间分布。每行代表一条腿，两个值分别表示摆动开始和结束的时间点。
    rbt.SetPhase(TimePeriod, TimeForGaitPeriod, TimeForSwingPhase);//SetPhase 方法则是将这个摆动相的时间设置为机器人的运动参数，使其按照这个时间点来协调四条腿的运动。

    std::ofstream outFile("compensationRecord.txt");//打开补偿记录文件

    Matrix<float,4,2> TimeForSwingPhase;
    Matrix<float, 4, 3> InitPos;
    Matrix<float, 6,1> TCV;
    TCV << 3.0/1000.0, 0, 0,0,0,0 ;// X, Y , alpha 
   
float  float_initPos[12]={   70.0,65.5,-21.0,
                             70.0,-65.5,-21.0,
                            -84.0, 65.5,-21.0,
                            -84.0, -65.5,-21.0};
    // float  float_initPos[12];
    // string2float("../include/initPos.csv", float_initPos);//Foot end position
    for(int i=0; i<4; i++)
        for(int j=0;j<3;j++)
        {
           
            InitPos(i, j) = float_initPos[i*3+j]/1000;
            // cout<<InitPos(i, j)<<endl;
        }
    rbt.SetInitPos(InitPos);
    rbt.InverseKinematics(rbt.mfLegCmdPos);
    cout<<"cmdPos: "<<rbt.mfJointCmdPos<<endl;//输出计算得到的关节命令位置 mfJointCmdPos，用于调试或验证计算的正确性。
   // rbt.SetPos(rbt.mfJointCmdPos);
 //  rbt.PumpAllPositve();
    usleep(1e6);
   rbt.SetCoMVel(TCV);//通过 SetCoMVel 方法，将之前定义的中心质点速度 TCV 设置为机器人的当前运动速度。
    TimeForSwingPhase<< 8*TimeForGaitPeriod/16, 	11*TimeForGaitPeriod/16,		
                        0,		 		 					3*TimeForGaitPeriod/16,		
                        12*TimeForGaitPeriod/16, 	15*TimeForGaitPeriod/16,		
                        4*TimeForGaitPeriod/16, 	7*TimeForGaitPeriod/16;
    rbt.SetPhase(TimePeriod, TimeForGaitPeriod, TimeForSwingPhase);

    std::ofstream outFile("compensationRecord.txt");//打开（或创建）一个名为 compensationRecord.txt 的文件，用于记录补偿数据。这些数据可能包括机器人的关节位置、速度、扭矩等信息，便于后续的分析和调试。

    // 检查文件是否成功打开
    if (!outFile) {
        std::cerr << "无法打开文件进行写入" << std::endl;
        return 1;
    }

//     //ADS1015 ads;

   
//     // while(1){
//     //     struct timeval startTime,endTime;
//     //     double timeUse;
//     //     gettimeofday(&startTime,NULL);
//     //     for(int i=0;i<4;i++){
//     //         value[i]=ads.read_adc(i,gain);
//     //           usleep(8000);
//     //     }
//     //     for(auto a:value){
//     //         std::cout<<a<<" ";
//     //     }
//     //     std::cout<<std::endl;
//     //     gettimeofday(&endTime,NULL);
//     //     timeUse = 1e6*(endTime.tv_sec - startTime.tv_sec)+ endTime.tv_usec - startTime.tv_usec;
//     //     std::cout<<"timeUse="<<timeUse<<std::endl;
//     // }
//     /*********adc test********/
//      vector<int> value(4),preValue(4),prepreValue(4);
//     for(auto a:value)
//         a=0;
//     preValue=value;
//     prepreValue=value;
   struct timeval startTimeswing,endTimeswing;
    double timeUseswing;
     gettimeofday(&startTimeswing,NULL);
    for(int times=0; times<2000; times++)//这个 for 循环表示机器人将执行 2000 次步态循环。每个循环表示一个步态周期的一部分。
    {
        struct timeval startTime,endTime;
        double timeUse;
        gettimeofday(&startTime,NULL);
        std::cout<<std::endl;
        std::cout<<"times"<<times<<std::endl;//每次步态循环开始时，记录当前时间 startTime，并打印当前的循环次数 times 以便调试或监视。
        // gecko.getTorque();
        // gecko.getPosition();
        // gecko.getVelocity();
        rbt.NextStep();
        rbt.fnn_stepmodify();
        rbt.InverseKinematics(rbt.mfLegCmdPos);
        for(uint8_t legNum=0; legNum<4; legNum++)  // run all 4 legs
        {   
            enum_LEGSTATUS ls=rbt.m_glLeg[legNum]->GetLegStatus(); //get present status
            outFile << ls << " ";
            cout<<ls<<" ";
        }//这个循环遍历机器人的四条腿，调用 GetLegStatus() 函数获取每条腿的状态（如摆动、支撑状态），并将状态值记录到文件 outFile 中，同时打印出来。
        cout<<endl;
        for (int index=0; index<3; index++)
        {
            outFile << rbt.mfCompensationZ(index) << " ";
        }//	代码记录了机器人对 Z 轴方向上的补偿值（可能是高度或姿态调整），并将其输出到文件和控制台。
        outFile << std::endl;
        cout<<"mfCompensationZ: \n"<<rbt.mfCompensationZ<<"\n";//<<"mfJointCmdPos: \n"<<rbt.mfJointCmdPos<<endl;

        rbt.UpdatejointPresPosAndVel(motorMapping(rbt.mfJointCmdPos));
        rbt.ForwardKinematics(1);
       // cout<<"forward: " <<rbt.mfLegPresPos<<endl;
        // cout<<" present postion: ";
        // for(int i=0;i<16;i++)
        //         cout<<gecko.present_position[i]<<" ";
        // cout<<endl;
        SetPos(rbt.mfJointCmdPos,gecko,rbt.vLastSetPos);
        gettimeofday(&endTime,NULL);
        timeUse = 1e6*(endTime.tv_sec - startTime.tv_sec) + endTime.tv_usec - startTime.tv_usec;
        if(timeUse < 1.0/loopRateStateUpdateSend*1e6)
          usleep(1.0/loopRateStateUpdateSend*1e6 - (double)(timeUse) - 2000); 

        // if(swingtimeFlag==true){
        //     gettimeofday(&endTimeswing,NULL);
        //     timeUseswing = 1e6*(endTimeswing.tv_sec - startTimeswing.tv_sec) + endTime.tv_usec - startTime.tv_usec;
        //     cout<<"timeuse="<<timeUseswing<<endl;
        //     exit(0);
        //}
    }
// //     pos2[11]=((float)times)/2000.0 * 3.14;
// //     pos2[15]=((float)times)/2000.0 * 3.14;
// //     pos2[8]=-((float)times)/2000.0 * 3.14;
// //     pos2[14]=((float)times)/2000.0 * 3.14;
// //     pos2[5]=-((float)times)/2000.0 * 3.14;
// //     pos2[13]=((float)times)/2000.0 * 3.14;
// //     pos2[2]=((float)times)/2000.0 * 3.14;
// //     pos2[12]=((float)times)/2000.0 * 3.14;
// //    rbt.dxlMotors.setPosition(pos2);
// //         // int gain=1;
// //         // for(int i=0;i<4;i++)
// //         // {
// //         //     value[i]=(int)ads.read_adc(i,gain);
// //         //     usleep(10000);
// //         // }
// //         // rbt.UpdateTouchStatus(value,preValue,prepreValue);
// //         // prepreValue=preValue;
// //         // preValue=value;

// //        // rbt.SetPos(rbt.mfJointCmdPos);
// //         usleep(1e3);
// //     }
//     int times=0;
//    bool reverse=false;
//     while(1){
        
//        // std::cout<<times<<std::endl;
//        // rbt.NextStep();
//        //rbt.InverseKinematics(rbt.mfLegCmdPos);
//       rbt.dxlMotors.getTorque();
//      rbt.dxlMotors.getPosition();
//      rbt.dxlMotors.getVelocity();
//     vector<float> pos2(16);
//     for(auto a : pos2){
//         a=0;
//     }
//     if(!reverse){
//     times++;
//     pos2[11]=((float)times)/2000.0 * 3.14;
//     pos2[15]=((float)times)/2000.0 * 3.14;
//     pos2[8]=-((float)times)/2000.0 * 3.14;
//     pos2[14]=((float)times)/2000.0 * 3.14;
//     pos2[5]=-((float)times)/2000.0 * 3.14;
//     pos2[13]=((float)times)/2000.0 * 3.14;
//     pos2[2]=((float)times)/2000.0 * 3.14;
//     pos2[12]=((float)times)/2000.0 * 3.14;
//     if(times>1000) reverse=true;
//     }
//     else{
//         times--;
//     pos2[11]=((float)times)/2000.0 * 3.14;
//     pos2[15]=((float)times)/2000.0 * 3.14;
//     pos2[8]=-((float)times)/2000.0 * 3.14;
//     pos2[14]=((float)times)/2000.0 * 3.14;
//     pos2[5]=-((float)times)/2000.0 * 3.14;
//     pos2[13]=((float)times)/2000.0 * 3.14;
//     pos2[2]=((float)times)/2000.0 * 3.14;
//     pos2[12]=((float)times)/2000.0 * 3.14;
//     if(times<0) reverse=false;
//     }
   
//    rbt.dxlMotors.setPosition(pos2);   
//    usleep(1e3); 
// }
}