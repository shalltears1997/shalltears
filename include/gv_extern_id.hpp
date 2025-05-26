extern const short MIS;
extern const short ANX;
extern const short EXP;
extern const short S1_MIS;
extern const short O_ANX;
extern const short O_EXP;
extern const short S2_MIS;
extern const short S2_ANX;
extern const short S1_EXP;
extern const short B_MIS;
extern const short B_ANX;
extern const short S2_EXP;
extern const short OY_MIS;
extern const short OY_ANX;
extern const short B_EXP;
extern const short OY_EXP;
extern const short OBJ;

extern const int NonOwner;
extern const int Owner;
extern const int SemiOwner;

// extern const int EXPLORE;
// extern const int EXPLORE_TO_OWNER;
// extern const int EXPLORE_TO_STRANGER;
// extern const int EXPLORE_TO_PLACE;
// extern const int PASSIVE_BEHAVIOR;
// extern const int GO_TO_DOOR;
// extern const int GO_TO_OWNER;
// extern const int GO_TO_BEHIND;
// extern const int GREETING;
// extern const int PLAY_WITH_OWNER;
// extern const int PLAY_WITH_STRANGER;
// extern const int PLAY_WITH_UNFAMILIAR;
// extern const int LEADING_BEHAVIOR;
// extern const int ATTENTION_GETTING;
// extern const int DIRECTIONAL_SIGNAL;
// extern const int EXPLORE_TO_UNFAMILIAR;
// extern const int PLAY_USING_BALL;
// extern const int GREETING_STRANGER;
// extern const int MISSING;
// extern const int GREETING_STRANGER2;
// extern const int EXPLORE_TO_STRANGER2;
// extern const int GO_TO_semiOWNER;
// extern const int EXPLORE_TO_PERSON;
//
// extern const int SOUND_GAZE;
// extern const int SOUND_MOVE;
// extern const int _HOME;



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

extern double RoomPoint[4];
extern double NoEnter[4];
extern double NoSleep[4];
extern double NearDoor[4];
//extern const double PassiveArea[4] = {RoomPoint[0] - 1500 , RoomPoint[0] , RoomPoint[2] , RoomPoint[3] };
//extern const double PassiveArea[4] = {RoomPoint[0] - 1500 , RoomPoint[0] , RoomPoint[2] , RoomPoint[3]+1000.0 };
//extern const double PassiveArea[4] = {-2500.0 , -1000.0 , 3500.0 , 3000.0};

extern const double Door[3];
//extern const double Door[3] = {3500.0 , 3500.0 , 1000.0};//{ x , y , z }
//extern const double Door[3] = {-500.0 , 2900.0 , 1000.0};

//プロトタイプ
extern const double Door_x;
extern const double Door_y;

//デモ用
//extern const double Door_x = 500.0;
//extern const double Door_y = 2500.0;
