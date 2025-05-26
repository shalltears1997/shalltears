//#pragma once

#include <cmath>

#define sMAX 100

#define ESCAPE_OVER_NUM 12

#define PI (6*asin(0.5))

#define area1 6
#define area2 3
#define HEIGHT_ETHO 3000
#define WIDTH_ETHO 6000

#define STATIC 0
#define INCREASING 1
#define DECREASING 2

#define DISTANCE 0
#define BEHAVIOR 1

// 基本的にMonitoringThread内
#define STATIC 0
#define INCREASING 1
#define DECREASING 2 // これだけselectMotionFlagにも
#define DISTANCE 0

// 実行ファイルからの相対パス
#define LOG_PATH "./log"
#define DAT_PATH "./dat"

// ポテンシャルフィールド改良
#define PATH_NEXT_RESO 10
#define PATH_VALLEY_RESO 10
// #define tki_MAX 300
#define tki_MAX 60
#define lowest_MAX 1800


#ifndef gridnum1
#define gridnum1 60
#endif

#ifndef gridnum2
#define gridnum2 30
#endif
