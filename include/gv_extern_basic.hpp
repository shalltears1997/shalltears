#include <iostream>
#include <thread>
#include <vector>

#include "class.hpp"

extern double s;


extern std::vector<std::thread> threads;

extern bool GoOnFlag;

// extern Dog pioneer;
extern Room nlab;
extern Human human_0;
extern Human human_1;
extern Human human_2;
// extern Obs obs[10000];
// extern PotentialGenerator ptf;
// extern Ball ball;

extern MovingObjectClass C_object[100];
extern CalWeightClass CalWeight;

extern handleOpenGLClass handleOpenGL;
extern connectWithMCClass connectWithMC;
extern connectWithZPSClass connectWithZPS;
extern connectWithUnityClass connectWithUnity;
extern RobotSimulatorClass RobotSimulator;

extern DataClass data, data_pre;
extern DebugDataClass debug_data;
extern ConfigDataClass config_data;

extern DebugClass Debug;

#include "tk_data_GL.hpp"
extern GLDataClass GLData;


////////////////////////////////////////////////////////////


// シミュレータか実機か
extern int poR, poO, poS, poF, HARK_connect, ROBOT, change_attachment, random_model, simRefreshRatio_clock;
extern bool exp_condition;

//環境地図につなぐ？
extern int map;

// setting
extern unsigned short PORT_Ser;
extern unsigned short PORT_Cli_Pio;
extern unsigned short PORT_Cli_Pan;
extern unsigned short PORT_Cli_PF;
extern unsigned short PORT_Cli_Face;
extern unsigned short PORT_Cli_Camera;
extern unsigned short PORT_Onboard;
extern unsigned short PORT_Cli_tale;
extern unsigned short PORT_Cli_oyatsu;
extern unsigned short PORT_Cli_LRF;
extern const char* IP_face;
extern const char* IP_ZPS;
extern const char* IP_Unity;
extern const char* IP_onboad;
extern const int NUM_OF_SEND_NUM;		//オンボードPCへの送信データ長
extern const char* IP_tale;		//arduino
extern const char* IP_LRF;		//LRF

// 確認用
extern int debug_SO;
extern bool log_save;


extern int gaze_time_th;
extern bool home;
extern float dist_owner, dist_stranger;
extern float sound_angle_owner;



////////////////////////////////////////////////////////////


#ifndef ZPSDATA
#define ZPSDATA
struct ZPSData
{
	ZPSData()
	{
		tag;
		type;
		for(int i; i < 3; i++)
		{
			pos[i] = 0.0;
		}
	}
	int tag; //タグ番号
	char type; //種類
	//char pm[3]; //正負
	float pos[3]; //座標
};
#endif


extern bool start_flag;
extern int motionFlag;

extern bool explore_cut;

extern double NextX, NextY;				// setMotion、potentialPath
extern Position targetpose;
extern Position passivearea;

//#define SB 50					// 使用なし
//double NextX_tk[SB], NextY_tk[SB];	// 使用なし

extern Position PassiveTarget;
//これは、要はInitPoseの位置をpassivetargetに指定しているということ
//（単なる初期化ではなく、意味のある値を代入しているということ）

// 大域変数（金井さん、最新版では未使用の模様）
extern bool interactionFlag;
extern bool interactionFlag2;

// 大域変数（おそらく洋平さん）
extern int owner_flag;		// MonitoringThread、selectMotionFlag
extern int owner_flags;		// MonitoringThread、selectMotionFlag


extern int ms_ball;			// CountThread、MonitoringThread
extern int ms_r;			// 使用なし



////// new //////

// "_new" のコードで必要

// 新MCに合わせて仮に必要

extern double temp_robot_deg; // tk1701
extern bool stop_flag2;		// 03n_TKpote3.cpp で使用：確実に止まるようにしている
//#define NEW_MC
extern double sim_speed;

// OpenGL
extern int samplingTime;



// ポテンシャルフィールド改良
extern bool barrier_set_flag;
extern bool barrier_removeAll_flag;
extern double barrier_x, barrier_y;

//	音源定位（2016/05/25、塚田）
#include "22_hark.hpp"
extern struct hark_struct hark;
extern bool sound_gaze;
extern double sound_gaze_angle;

extern clock_t time_hark_gaze_start;
extern double gaze_time;

extern clock_t soundMoveFinishTime;
extern clock_t soundMoveBeginTime;
extern bool soundMoveFinishFlag;


//顔の向きのための変数
extern int human_face_direction;

//人とロボットの距離を保持するためのベクトル
extern std::vector<double> move_distance;	

//社会的参照のためのownerのstrangerへの動きによる距離を保持するためのベクトル
extern std::vector<double> move_distance_people;

//ロボットの速度を計算するための速度係数（これをMCに送りつけてあっちで計算させる）
extern double robot_velocity_parm;

//確認用（後で消す）
extern double print_delta;
extern double print_delta2;
extern double time_duration;

//速度確認用
extern double velocity;

//ボーカリゼーションのためのストレスポテンシャルの最大値、最小値
extern double max_stress_potential;
extern double min_stress_potential;
extern DATA_xy max_pote, min_pote;
//ボーカリゼーションのための音フラグ
extern double sound_flag;
