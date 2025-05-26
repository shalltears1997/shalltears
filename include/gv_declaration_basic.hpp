#include <iostream>
#include <thread>
#include <vector>

#include "class.hpp"

double s = 1.0;

std::vector<std::thread> threads;

bool GoOnFlag = true;

// Dog pioneer;
Room nlab;
Human human_0(70, "Owner");
Human human_1(30, "Stranger");
Human human_2(20, "Person");
// Obs obs[10000];
// PotentialGenerator ptf;
// Ball ball;

handleOpenGLClass handleOpenGL;
connectWithMCClass connectWithMC;
connectWithZPSClass connectWithZPS;
connectWithEMClass connectWithEM;
connectWithUnityClass connectWithUnity;
RobotSimulatorClass RobotSimulator(3);

DataClass data, data_pre;
DebugDataClass debug_data;
ConfigDataClass config_data;

MovingObjectClass C_object[100];
CalWeightClass CalWeight;

DebugClass Debug;

#include "tk_data_GL.hpp"
GLDataClass GLData;

////////////////////////////////////////////////////////////

// 初期値のみ代入するように変更（2017/10/06、塚田）
// 設定する値そのものは config.xml ファイルに記載
int poR = 0, poO = 0, poS = 0, poF = 0, HARK_connect = 0, ROBOT = 0, change_attachment = 0, random_model = 0, simRefreshRatio_clock = 10;
bool exp_condition = 0;

//環境地図
int map = 0;

// setting
unsigned short PORT_Ser = 60000;
unsigned short PORT_Cli_Pio = 45678;
unsigned short PORT_Cli_Pan = 45364;
unsigned short PORT_Cli_PF = 45365;
unsigned short PORT_Cli_Face = 45600;//45377;
unsigned short PORT_Cli_Camera = 45362;
unsigned short PORT_Onboard = 45366;
unsigned short PORT_Cli_tale = 45367;
unsigned short PORT_Cli_oyatsu = 45368;
unsigned short PORT_Cli_LRF = 45369;
const char* IP_face = "127.0.0.1";//"133.91.73.222";//add
const char* IP_ZPS = "133.91.73.183"; //for ZPS
const char* IP_Unity = "133.91.73.216"; //for Unity
const char* IP_onboad = "133.91.73.194"; //from onboard  //
const int NUM_OF_SEND_NUM = 61; //オンボードPCへの送信データ長
								//const char* IP_oyatsu = "127.0.0.1";//おやつ
const char* IP_tale = "127.0.0.1";//arduino
const char* IP_LRF = "133.91.73.236";//LRF


// 確認用
int debug_SO = 0;			// 0: なし、1: ボール、2: ボール遊び(1)、3: ボール遊び(2)、4: 音源定位
bool log_save = false;		// ファイル名は "C:¥¥debugEE¥¥log-entire_%04d%02d%02d_%02d%02d.txt", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute



int gaze_time_th = 6;	//4
bool home = false; // 2016/06/01 ホーム機能追加（']'キー押下でディスプレイの前まで戻ってくる）
float dist_owner, dist_stranger;
float sound_angle_owner;



////////////////////////////////////////////////////////////


bool start_flag = true;
int motionFlag = 0;

bool explore_cut = false;

double NextX, NextY;				// setMotion、potentialPath
Position targetpose = InitPose();
Position passivearea;

#define SB 50					// 使用なし
double NextX_tk[SB], NextY_tk[SB];	// 使用なし

Position PassiveTarget = InitPose();
//これは、要はInitPoseの位置をpassivetargetに指定しているということ
//（単なる初期化ではなく、意味のある値を代入しているということ）

// 大域変数（金井さん、最新版では未使用の模様）
bool interactionFlag = false;
bool interactionFlag2 = false;//add

// 大域変数（おそらく洋平さん）
int owner_flag = -1;		// MonitoringThread、selectMotionFlag
int owner_flags = -1;		// MonitoringThread、selectMotionFlag

int ms_ball = 0;			// CountThread、MonitoringThread
int ms_r = 0;			// 使用なし


// 新MCに合わせて仮に必要
double temp_robot_deg = 0.0; // tk1701
bool stop_flag2 = false;		// 03n_TKpote3.cpp で使用：確実に止まるようにしている
//#define NEW_MC
double sim_speed = 6;

// OpenGL
int samplingTime = 30;


// ポテンシャルフィールド改良
bool barrier_set_flag = false;
bool barrier_removeAll_flag = true;
double barrier_x = 0, barrier_y = 0;

//	音源定位（2016/05/25、塚田）
#include "22_hark.hpp"
struct hark_struct hark = {	0,
							{0, 0, 0, 0, 0},
							{0.0, 0.0, 0.0, 0.0, 0.0},
							{0.0, 0.0, 0.0, 0.0, 0.0},
							{0.0, 0.0, 0.0, 0.0, 0.0},
							{0.0, 0.0, 0.0, 0.0, 0.0},
							{0.0, 0.0, 0.0, 0.0, 0.0},
							false,
							0.0 };

bool sound_gaze = false;
double sound_gaze_angle = 0;

clock_t time_hark_gaze_start = 0;
double gaze_time = 0;
clock_t soundMoveFinishTime = 0;
clock_t soundMoveBeginTime = 0;
bool soundMoveFinishFlag = 0;

//顔の向きを取るための変数
int human_face_direction = 0;

//人とロボットの距離を保持するためのベクトル
std::vector<double> move_distance(10, 0);

//人とロボットの距離を保持するためのベクトル
std::vector<double> move_distance_people(10, 0);

//速度計算ようの速度計数
double robot_velocity_parm = 0.0;

//確認用（後で消す）
double print_delta = 0.0;
double print_delta2 = 0.0;
//double time_duration = 0.0;

//移動速度確認用
double velocity = 0.0;

//ボーカリゼーションのためのストレスポテンシャルの最大値、最小値
double max_stress_potential = 0.0;
double min_stress_potential = 0.0;
DATA_xy max_pote, min_pote;
//ボーカリゼーションのための音フラグ
double sound_flag = 0.0;
