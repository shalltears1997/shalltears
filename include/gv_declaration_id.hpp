const short MIS = 0;
const short ANX = 1;
const short EXP = 2;
const short S1_MIS = 3;
const short O_ANX = 4;
const short O_EXP = 5;
const short S2_MIS = 6;
const short S2_ANX = 7;
const short S1_EXP = 8;
const short B_MIS = 9;
const short B_ANX = 10;
const short S2_EXP = 11;
const short OY_MIS = 12;
const short OY_ANX = 13;
const short B_EXP = 14;
const short OY_EXP = 15;
const short OBJ = 100;

const int NonOwner = - 1;
const int Owner = 0;
const int SemiOwner = 1;


// GO_TO_SITTING_OWNER
// PLAY_USING_BALL

// EXPLORE
// MISSING
// GO_TO_DOOR
// GO_TO_OWNER
// EXPLORE_STRANGER


#ifndef ID
  #define ID

  #define GO_TO_SITTING_OWNER 01
  #define PLAY_USING_BALL 02

  #define EXPLORE 21
  #define MISSING 22
  #define GO_TO_DOOR 23
  #define GO_TO_OWNER 24
  #define EXPLORE_STRANGER 25
  #define MONITORING_OBJECT 26
  #define MONITORING_AREA 27
  #define MONITORING_USER 28
  #define NOTIFY 29
  #define ROTATE 30
  #define ATTENTION_GETTING 31
  #define SHOWING_DIRECTION 32
  #define GAZE_ALTERNATION 33

  #define RANDOM 99

  // #define LEADING_BEHAVIOR 13
  // #define ATTENTION_GETTING 14
  // #define DIRECTIONAL_SIGNAL 15

  // #define SOUND_GAZE 101
  // #define SOUND_MOVE 102
  #define HOME 0
#endif

#ifndef ZPS_Xo
#define ZPS_Xo 1350.0
#endif

#ifndef ZPS_Yo
#define ZPS_Yo 4210.0
#endif
//#define ZPS_Yo 3210.0

double RoomPoint[4] = {-3000.0 , 3000.0 , 3000.0 , 0.0};
double NoEnter[4] = {1000.0, 3000.0, 3000.0, 2000.0};
double NoSleep[4] = {-3000.0, 3000.0, 2000.0, 0.0};
double NearDoor[4] = {1500.0 , 3000.0 , 3000.0 , 2000.0};
//const double PassiveArea[4] = {RoomPoint[0] - 1500 , RoomPoint[0] , RoomPoint[2] , RoomPoint[3] };
//const double PassiveArea[4] = {RoomPoint[0] - 1500 , RoomPoint[0] , RoomPoint[2] , RoomPoint[3]+1000.0 };
//const double PassiveArea[4] = {-2500.0 , -1000.0 , 3500.0 , 3000.0};

const double Door[3] = {2900.0 , 2900.0 , 1000.0};//{ x , y , z }
//const double Door[3] = {3500.0 , 3500.0 , 1000.0};//{ x , y , z }
//const double Door[3] = {-500.0 , 2900.0 , 1000.0};



//プロトタイプ
const double Door_x = 7800.0 - ZPS_Yo;
const double Door_y = ZPS_Xo - (-1700.0 - 1500.0);

//デモ用
//const double Door_x = 500.0;
//const double Door_y = 2500.0;
