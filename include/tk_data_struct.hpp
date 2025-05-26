#pragma once

#include <ctime>
#include <limits>
#include <vector>
#include "c_struct_position.hpp"

typedef struct
{
	double x;
	double y;
}DATA_xy;

class PositionClass
{
private:

	int region_xXyY[4] = {-3000, 4500, 0, 4500};

	bool exist_x(double pos_x)
	{
		return !(pos_x < region_xXyY[0]) * !(pos_x > region_xXyY[1]);
	}

	bool exist_y(double pos_y)
	{
		return !(pos_y < region_xXyY[2]) * !(pos_y > region_xXyY[3]);
	}

public:

	// 領域の外か中かを判別した値をここで管理する
  // 呼び出すときはこれを直接覗く（外部から代入できてしまうので、しないように注意）：演算子を使おうか？
	double x;
	double y;

	// 領域判定
	bool exist()
	{
		return exist_x(this->x) * exist_y(this->y);
	}

	// 値セット
	void setX(double pos_x)
	{
		if(exist_x(pos_x)) x = pos_x;
		else x = std::numeric_limits<double>::infinity();
	}
	void setY(double pos_y)
	{
		if(exist_y(pos_y)) y = pos_y;
		else y = std::numeric_limits<double>::infinity();
	}
	void setPos(DATA_xy pos)
	{
		setX(pos.x);
		setY(pos.y);
	}

	// 仮に必要（今後、位置の管理を全てPosition型に任せてしまえばこれは不要になる）
	DATA_xy pos()
	{
		return (DATA_xy){x, y};
	}


};


typedef struct
{
	double time;
	DATA_xy human[2];
	DATA_xy ball;
}PREARRANGED;


typedef struct
{
	double x0;
	double y0;
	double x1;
	double y1;
}DATA_obstacle;

typedef struct
{
	DATA_xy posi;
	int oID;
	int inmap;
	double monitoring_necessity;
}DATA_object;

typedef struct
{
	int i;
	int j;
}DATA_grid;



typedef struct
{
	DATA_grid grid;
	double value;
}DATA_grid_part;


typedef struct
{
	DATA_xy grid;
	double value;
}DATA_xy_part;


typedef struct
{
	DATA_xy posi;
	double body_deg;
	double pan_deg;
	double tilt_deg;
}DATA_robot;


typedef struct
{
	DATA_xy posi;
	double height;
	double deg;
	double monitoring_necessity;
}DATA_human;





typedef struct
{
	std::vector<DATA_xy> path;
	double speed;
}COMMAND_path;

typedef struct
{
	double deg;
	double speed;
}COMMAND_deg;

typedef struct
{
	COMMAND_path robot;
	COMMAND_deg body;
	int head_flag;
	COMMAND_deg pan;
	COMMAND_deg tilt;
	COMMAND_deg gripper;
}COMMAND_set;



// これは従来からあったやつ
typedef struct
{
	short state;
	float value;
}InnerState;

// typedef struct
// {
// 	// Position pose;
// 	double attachment;
// 	InnerState miss;
// 	InnerState anxiety;
// }PARAMETER;


typedef struct
{
	double ata;

	// 増減の観測値
	double danxP;
	double danxM;
	double dmisP;
	double dmisM;
	double dexpP;
	double dexpM;

	// 認知値（差分）
	double danx;
	double dmis;
	double dexp;

	// 認知値（履歴）
	double anx_c;
	double mis_c;
	double exp_c;

	// 実効値
	double anx;
	double mis;
	double exp;

}PARAMETER;

typedef struct
{
	int ball_hold;			// myKeyboard、myMouseFunc、myMouseMotion、PrintfThread、BallThread、selectMotionFlag、MonitoringThread
	int ball_t;			// myKeyboard、myMouseFunc、myMouseMotion、recvFromPioneer、PrintfThread、BallThread
	int t_id;				// myKeyboard、myMouseFunc、recvFromPioneer、PrintfThread、CountThread、selectMotionFlag、MotionTread
	int ball_flag;			// BallThread、selectMotionFlag
	int t_flag;			// PrintfThread
}BALL_yoheisan;



/*
洋平さんの定義のメモ
int ball_hold = 0;				// 現在の保持者	0：誰も持っていない、1：owner、2：stranger、3：robot
int t_id = 0;					// 前の保持者		0：誰も持っていない、1：ownerが投げた、2：strangerが投げた、3：robotが渡せた
int t_flag = 0;					// 追う必要の有無	0：追う必要なし、1：追う必要あり
//int ball_t = 0;				// ボールの属性	0：投げられていない、1：投げられている
//int ball_flag = 0;
*/
typedef struct
{
	// 実験用
	bool ball_priority = true;	// ボールが見えたら懐き度などの条件に関わらずボール遊びを選択する

	int no_ball_count;
	clock_t last_ball_exist_time;
	double no_ball_time;
	bool ball_presence;
	clock_t ball_previous_time;
	double ball_time_interval_th = 5;	//7
	double ball_time_interval;
	double ball_distance[11];
	double ball_theta[11];
	bool catch_flag;
	int hand_flag;
	int stop_flag;
	double circularity[16];
	double confidence_sum[16];
	double SD[16];
}BALL;


  // 謎仕様
typedef struct
{
  int expflag = 0;			// selectMotionFlag、myKeyboard
	int ms_d = 0;			// CountThread、MonitoringThread
}NAZO;
