#include <iostream>
#include <cstdlib>
#include <thread>
#include <random>  // 乱数
#include <algorithm>

#include "gv_extern.hpp"
#include "c_struct_position.hpp"
#include "l_tk-library.hpp"
#include "tk32_BasicBehaviors.hpp"

void Rotate_BC::behavior()
{
	static DATA_xy position = {};
	static DATA_xy rota_goal = {};
	static int count=0;
	static int judge=0;

	position = (data->object[data->object_maxID].posi);

	// 本体
	if(!circle_around_obj(data->robot.posi,position))
	{
		//position = (data->object[data->object_maxID].posi);

		command.robot.path = GeneratePotePathToGoal(position);
		command.robot.speed = 300;
		// 本体・回転
		command.body.deg = bodyRotateTo(data->object[data->object_maxID].posi);
		command.body.speed = 20;
	  // 頭部角度参照元（0：EE、1：ボール）
		command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo(data->object[data->object_maxID].posi);
		command.pan.speed = 20;
		// 頭部・チルト
	    command.tilt.deg = tiltRotateTo(data->object[data->object_maxID].posi);
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;

		count= 0;
	}
	else
	{

		if(count == 0){
					rota_goal = fix_object_goal(position);
					PRINT("ここ入ってるよー");
				}


				count = 1;

				if(judge%2 == 0){
		rota_goal.x = data->object[data->object_maxID].posi.x + (rota_goal.x-data->object[data->object_maxID].posi.x)*cos(PI/40)-(rota_goal.y-data->object[data->object_maxID].posi.y)*sin(PI/40);
		rota_goal.y = data->object[data->object_maxID].posi.y + (rota_goal.x-data->object[data->object_maxID].posi.x)*sin(PI/40)+(rota_goal.y-data->object[data->object_maxID].posi.y)*cos(PI/40);
				}

		command.robot.path = GeneratePotePathToGoal({rota_goal.x,rota_goal.y});
		command.robot.speed = 300;
					// 本体・回転
					command.body.deg = bodyRotateTo({rota_goal.x,rota_goal.y});
					command.body.speed = 20;
					// 頭部角度参照元（0：EE、1：ボール）
					command.head_flag = 0;
					// 頭部・パン
					command.pan.deg = panRotateTo(data->object[data->object_maxID].posi);
					command.pan.speed = 20;
					// 頭部・チルト
					command.tilt.deg = tiltRotateTo(data->object[data->object_maxID].posi);
					command.tilt.speed = 20;
					// グリッパ
					command.gripper.deg = gripperOpen();
					command.gripper.speed = 20;
					count=1;

	judge++;
	if(judge%2==0)judge=0;

		/*if(count == 0)
		{
			rota_goal = fix_object_goal(position);
			PRINT("ここ入ってるよー");
		}


		count = 1;

		PRINT("x:%lf,y:%lf\n",data->object[data->object_maxID].posi.x,data->object[data->object_maxID].posi.y);
		PRINT("x:%lf,y:%lf\n",rota_goal.x,rota_goal.y);

		//PRINT("pi:%lf\n",cos(PI/3));

		if(rotate_area(data->robot.posi,rota_goal))
		{

			PRINT("true\n");
			rota_goal.x = data->object[data->object_maxID].posi.x + (rota_goal.x-data->object[data->object_maxID].posi.x)*cos(PI/3)-(rota_goal.y-data->object[data->object_maxID].posi.y)*sin(PI/3);
			rota_goal.y = data->object[data->object_maxID].posi.y + (rota_goal.x-data->object[data->object_maxID].posi.x)*sin(PI/3)+(rota_goal.y-data->object[data->object_maxID].posi.y)*cos(PI/3);

		}else
		{
			PRINT("false\n");
			command.robot.path = GeneratePotePathToGoal({rota_goal.x,rota_goal.y});
				command.robot.speed = 300;
				// 本体・回転
				command.body.deg = bodyRotateTo({rota_goal.x,rota_goal.y});
				command.body.speed = 20;
				// 頭部角度参照元（0：EE、1：ボール）
				command.head_flag = 0;
				// 頭部・パン
				command.pan.deg = panRotateTo(data->object[data->object_maxID].posi);
				command.pan.speed = 20;
				// 頭部・チルト
				command.tilt.deg = tiltRotateTo(data->object[data->object_maxID].posi);
				command.tilt.speed = 20;
				// グリッパ
				command.gripper.deg = gripperOpen();
				command.gripper.speed = 20;
		}*/
	}
}

void Monitoring_User_BC::behavior()
{
	static DATA_xy position = {};
		static DATA_xy rota_goal = {};
		static int count=0;
		static int judge=0;

		position = (data->human[0].posi);

		// 本体
		if(!circle_around_obj(data->robot.posi,position))
		{
			//position = (data->object[data->object_maxID].posi);

			command.robot.path = GeneratePotePathToGoal(position);
			command.robot.speed = 300;
			// 本体・回転
			command.body.deg = bodyRotateTo(data->human[0].posi);
			command.body.speed = 20;
		  // 頭部角度参照元（0：EE、1：ボール）
			command.head_flag = 0;
			// 頭部・パン
			command.pan.deg = panRotateTo(data->human[0].posi);
			command.pan.speed = 20;
			// 頭部・チルト
		    command.tilt.deg = tiltRotateTo(data->human[0].posi);
			command.tilt.speed = 20;
			// グリッパ
			command.gripper.deg = gripperOpen();
			command.gripper.speed = 20;

			count= 0;
		}
		else
		{

			if(count == 0){
						rota_goal = fix_object_goal(position);
						PRINT("ここ入ってるよー");
					}


					count = 1;

					if(judge%2 == 0){
			rota_goal.x = data->human[0].posi.x + (rota_goal.x-data->human[0].posi.x)*cos(PI/40)-(rota_goal.y-data->human[0].posi.y)*sin(PI/40);
			rota_goal.y = data->human[0].posi.y + (rota_goal.x-data->human[0].posi.x)*sin(PI/40)+(rota_goal.y-data->human[0].posi.y)*cos(PI/40);
					}

			command.robot.path = GeneratePotePathToGoal({rota_goal.x,rota_goal.y});
			command.robot.speed = 300;
						// 本体・回転
						command.body.deg = bodyRotateTo({rota_goal.x,rota_goal.y});
						command.body.speed = 20;
						// 頭部角度参照元（0：EE、1：ボール）
						command.head_flag = 0;
						// 頭部・パン
						command.pan.deg = panRotateTo(data->human[0].posi);
						command.pan.speed = 20;
						// 頭部・チルト
						command.tilt.deg = tiltRotateTo(data->human[0].posi);
						command.tilt.speed = 20;
						// グリッパ
						command.gripper.deg = gripperOpen();
						command.gripper.speed = 20;
						count=1;

		judge++;
		if(judge%2==0)judge=0;


		}
}

void Notify_BC::behavior()
{
	static int count = 0;
	//家の中で死角になる場所っていうのは限られている（決まっている）。だから、NOTIFYする場所は決め打ちでいい。（自分で決めればいい）
	if(!Notify_area_judge())
	{
		command.robot.path = GeneratePotePathToGoal({-2300,500});
		command.robot.speed = 300;
		// 本体・回転
		command.body.deg = bodyRotateTo({-2300,500});
		command.body.speed = 20;
	  // 頭部角度参照元（0：EE、1：ボール）
	  command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo({-2300,500});
		command.pan.speed = 20;
		// 頭部・チルト
	    command.tilt.deg = tiltRotateTo({-2300,500});
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;
	}
	else
	{
		//command.robot.path = GeneratePotePathToGoal({-2300,500});
		//command.robot.speed = 300;
		if(count <50)
		{
			command.body.deg = bodyRotateTo({3000,3000});
		command.body.speed = 20;
		// 頭部角度参照元（0：EE、1：ボール）
		command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo(data->human[0].posi);
		command.pan.speed = 20;
		// 頭部・チルト
		command.tilt.deg = tiltRotateTo(data->human[0].posi);
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;
		count++;

		PRINT("count;%d\n",count);


		}
		else
		{
		command.body.deg = bodyRotateTo({3000,3000});
		command.body.speed = 20;
		// 頭部角度参照元（0：EE、1：ボール）
		command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo(data->object[data->object_maxID].posi);
		command.pan.speed = 20;
		// 頭部・チルト
		command.tilt.deg = tiltRotateTo(data->object[data->object_maxID].posi);
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;
		count++;
		if(count>100)count = 0;
		}
	}
}

void AttentionGetting_BC::behavior()
{
	DATA_xy position = {};
	position = (data->object[data->object_maxID].posi); ////これやめたいよーーー
	DATA_xy fix_human_posi ={};
	static int count=0;

	fix_human_posi = fix_object_goal(data->human[0].posi);

	if(calDistance(data->robot.posi.x-position.x,data->robot.posi.y-position.y)<1500 && (calDistance(data->robot.posi.x-position.x,data->robot.posi.y-position.y)<calDistance(data->human[0].posi.x-position.x,data->human[0].posi.y-position.y)))
	{
		data->flag_attention = true;
	}

	if(data->flag_attention)//人に1800まで近寄る
	{

	//	if(count <50)
		//{
			// 本体
					//	PRINT("%lf  %lf",grid_to_coordinate(data->max_area_grid).x,grid_to_coordinate(data->max_area_grid).y);
					command.robot.path = GeneratePotePathToGoal(fix_human_posi);
					command.robot.speed = 300;
					// 本体・回転
					command.body.deg = bodyRotateTo(data->human[0].posi);
					command.body.speed = 20;
					// 頭部角度参照元（0：EE、1：ボール）
					command.head_flag = 0;
					// 頭部・パン
					command.pan.deg = panRotateTo(data->human[0].posi);
					command.pan.speed = 20;
					// 頭部・チルト
					command.tilt.deg = tiltRotateTo(data->human[0].posi);
					command.tilt.speed = 20;
					// グリッパ
					command.gripper.deg = gripperOpen();
					command.gripper.speed = 20;
	/*	count++;
		//PRINT("count;%d\n",count);
		}
		else
		{
		command.body.deg = bodyRotateTo(position);
		command.body.speed = 20;
		// 頭部角度参照元（0：EE、1：ボール）
		command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo(data->object[data->object_maxID].posi);
		command.pan.speed = 20;
		// 頭部・チルト
		command.tilt.deg = tiltRotateTo(data->object[data->object_maxID].posi);
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;
		count++;
		if(count>100)count = 0;
		}*/

	}
	else if(data->flag_attention==false)
	{
		// 本体
		//	PRINT("%lf  %lf",grid_to_coordinate(data->max_area_grid).x,grid_to_coordinate(data->max_area_grid).y);
				command.robot.path = GeneratePotePathToGoal(position);
				command.robot.speed = 300;
				// 本体・回転
				command.body.deg = bodyRotateTo(position);
				command.body.speed = 20;
			  // 頭部角度参照元（0：EE、1：ボール）
			  command.head_flag = 0;
				// 頭部・パン
				command.pan.deg = panRotateTo(position);
				command.pan.speed = 20;
				// 頭部・チルト
			    command.tilt.deg = tiltRotateTo(position);
				command.tilt.speed = 20;
				// グリッパ
				command.gripper.deg = gripperOpen();
				command.gripper.speed = 20;


	}
}

void ShowingDirection_BC::behavior()
{
	DATA_xy position={};
	position = (data->object[data->object_maxID].posi);
	DATA_xy fix_position={};
	fix_position = fix_object_goal2(position);

	if(calDistance(data->robot.posi.x-data->human[0].posi.x,data->robot.posi.y-data->human[0].posi.y)<1100)
	{
		command.robot.path = GeneratePotePathToGoal(fix_position);
		command.robot.speed = 300;
		// 本体・回転
		command.body.deg = bodyRotateTo(position);
		command.body.speed = 20;
		// 頭部角度参照元（0：EE、1：ボール）
		command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo(position);
		command.pan.speed = 20;
		// 頭部・チルト
		command.tilt.deg = tiltRotateTo(position);
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;

		data->flag_attention=false;
		data->flag_direction = true;
		data->count = 0;
	}
	else// if(/*追いかけてきてない*/)
	{
		command.body.deg = bodyRotateTo(data->human[0].posi);
		command.body.speed = 20;
		// 頭部角度参照元（0：EE、1：ボール）
		command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo(data->human[0].posi);
		command.pan.speed = 20;
		// 頭部・チルト
		command.tilt.deg = tiltRotateTo(data->human[0].posi);
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;

		data->count += 1;
		if(data->count >200)data->flag_direction=false;

	}
}

void GazeAlternation_BC::behavior()
{
	static int count =0;
	DATA_xy position={};
	position = (data->object[data->object_maxID].posi);
	DATA_xy fix_position={};
	fix_position = fix_object_goal2(position);

	if(count <50)
	{
		command.body.deg = bodyRotateTo(position);
		command.body.speed = 20;
		// 頭部角度参照元（0：EE、1：ボール）
		command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo(position);
		command.pan.speed = 20;
		// 頭部・チルト
		command.tilt.deg = tiltRotateTo(position);
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;
		count++;
		//PRINT("count;%d\n",count);
		data->flag_direction=false;
	}
	else
	{
		command.body.deg = bodyRotateTo(data->human[0].posi);
		command.body.speed = 20;
		// 頭部角度参照元（0：EE、1：ボール）
		command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo(data->human[0].posi);
		command.pan.speed = 20;
		// 頭部・チルト
		command.tilt.deg = tiltRotateTo(data->human[0].posi);
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;
		count++;
		if(count>100)count = 0;
	}
}

void Monitoring_Area_BC::behavior()
{
	// 本体
//	PRINT("%lf  %lf",grid_to_coordinate(data->max_area_grid).x,grid_to_coordinate(data->max_area_grid).y);
		command.robot.path = GeneratePotePathToGoal(grid_to_coordinate(data->max_area_grid));
		command.robot.speed = 300;
		// 本体・回転
		command.body.deg = bodyRotateTo(grid_to_coordinate(data->max_area_grid));
		command.body.speed = 20;
	  // 頭部角度参照元（0：EE、1：ボール）
	  command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo(grid_to_coordinate(data->max_area_grid));
		command.pan.speed = 20;
		// 頭部・チルト
	    command.tilt.deg = tiltRotateTo(grid_to_coordinate(data->max_area_grid));
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;
}


void Monitoring_Obj_BC::behavior()
{

	//PRINT("%lf  %lf",data->object[data->object_maxID].posi.x,data->object[data->object_maxID].posi.y);
	// 本体
		command.robot.path = GeneratePotePathToGoal(fix_object_goal(data->object[data->object_maxID].posi));
		command.robot.speed = 300;
		// 本体・回転
		command.body.deg = bodyRotateTo(data->object[data->object_maxID].posi);
		command.body.speed = 20;
	  // 頭部角度参照元（0：EE、1：ボール）
	  command.head_flag = 0;
		// 頭部・パン
		command.pan.deg = panRotateTo(data->object[data->object_maxID].posi);
		command.pan.speed = 20;
		// 頭部・チルト
	    command.tilt.deg = tiltRotateTo(data->object[data->object_maxID].posi);
		command.tilt.speed = 20;
		// グリッパ
		command.gripper.deg = gripperOpen();
		command.gripper.speed = 20;
}


void GoToDoor_BC::behavior()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal({3000, 3000});
	command.robot.speed = 500;
	// 本体・回転
	command.body.deg = bodyRotateTo({3000, 3000});
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panRotateTo({3000, 3000});
	command.pan.speed = 20;
	// 頭部・チルト
  if(tiltRotateTo({3000, 3000}) > 50) command.tilt.deg = 50;
  else command.tilt.deg = tiltRotateTo({3000, 3000});
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

void GoToOwner_BC::behavior()
{
  // 本体
	if(data->pa_human[0].ata >= 50 && data->pa_human[1].ata <= 50 && exist_judge_human(data->human[1].posi))
	{
  	command.robot.path = GeneratePotePathToGoal(human_around(data->human[0].posi, data->human[1].posi));
  	command.robot.speed = 500;
	}
	else
	{
  	command.robot.path = GeneratePotePathToGoal(data->human[0].posi);
  	command.robot.speed = 500;
	}
	command.robot.path = GeneratePotePathToGoal(data->human[0].posi);

  // 本体・回転
  command.body.deg = bodyRotateTo(data->human[0].posi);
  command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panRotateTo(data->human[0].posi);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->human[0].posi);;
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

void Missing_BC::behavior()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal({human_0.FrontPosition().posex, human_0.FrontPosition().posey});
	// command.robot.path = GeneratePotePathToGoal(data->human[0].posi);
  command.robot.speed = 500;
	// 本体・回転
	command.body.deg = bodyRotateTo(data->human[0].posi);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
  command.pan.deg = panRotateTo(data->human[0].posi);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->human[0].posi);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

void Play_BC::behavior()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal({human_0.FrontPosition().posex, human_0.FrontPosition().posey});
	command.robot.speed = 500;
	// 本体・回転
	command.body.deg = bodyRotateTo(data->human[0].posi);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panRotateTo(data->human[0].posi);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->human[0].posi);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

void Greets_BC::behavior()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal({human_0.FrontPosition().posex, human_0.FrontPosition().posey});
	command.robot.speed = 500;
	// 本体・回転
	command.body.deg = bodyRotateTo(data->human[0].posi);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panRotateTo(data->human[0].posi);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->human[0].posi);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

void GreetsToStranger_BC::behavior()
{
	// 本体
	// command.robot.path = GeneratePotePathToGoal({human_1.FrontPosition().posex, human_1.FrontPosition().posey});
	command.robot.path = GeneratePotePathToGoal(data->human[1].posi);
	command.robot.speed = 500;
	// 本体・回転
	command.body.deg = bodyRotateTo(data->human[1].posi);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panRotateTo(data->human[1].posi);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->human[1].posi);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

void GreetsToStranger2_BC::behavior()
{
	// 本体
	// command.robot.path = GeneratePotePathToGoal({human_2.FrontPosition().posex, human_2.FrontPosition().posey});
	command.robot.path = GeneratePotePathToGoal(data->human[1].posi);
	command.robot.speed = 500;
	// 本体・回転
	command.body.deg = bodyRotateTo(data->human[1].posi);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panRotateTo(data->human[1].posi);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->human[1].posi);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}


void ExploreStranger_BC::behavior()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal({data->human[1].posi.x, data->human[1].posi.y});
	command.robot.speed = 500;
	// 本体・回転
	command.body.deg = bodyRotateTo(data->human[1].posi);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panRotateTo(data->human[1].posi);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->human[1].posi);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}


// ヘッダの代わり（tk32_Behavior_definition.cpp）
DATA_xy goalDecision2(bool cognition);

void AttachmentBehavior_BC::behavior()
{
	DATA_xy goal = data->tmp_goal;

	// 顔を向ける場所決定
	DATA_xy look_posi;

	// 最も近い人を割り出す
	std::vector<double> temp;
	for(int i=0; i<data->human_num; i++) temp.push_back(calDistance(goal, data->human[i].posi));
	std::vector<double>::iterator temp_max = std::min_element(temp.begin(), temp.end());
	int n = std::distance(temp.begin(), temp_max);

	// 最も近い人が、至近距離なら、その人を向くように、フラグと場所を変更
	bool look_flag = false;
	double th = attachment2distance_tmp(data->pa_human[n].ata) * 1.2;
	if(calDistance(goal, data->human[n].posi) < th)
	{
		look_flag = true;
		look_posi = data->human[n].posi;
	}

	// 本体
	command.robot.path = GeneratePotePathToGoal(goal);
	command.robot.speed = 500;
	// 本体・回転
	if(look_flag) command.body.deg = bodyRotateTo(look_posi);
	else command.body.deg = bodyRotateTo(goal);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	if(look_flag) command.pan.deg = panRotateTo(look_posi);
	else command.pan.deg = panRotateTo(goal);
	command.pan.speed = 20;
	// 頭部・チルト
	if(look_flag) command.tilt.deg = tiltRotateTo(look_posi);
	else command.tilt.deg = 0;
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;

  // CognitionFlag = false;
}

void PassiveBehavior_BC::behavior()
{
	// double pfirst = 0.0;
	// if(passivefirst == true)
	// {
	// 	pfirst = nlab.getTime();
	// 	passivefirst = false;
	// }
	// double now = nlab.getTime();
	// while((now - pfirst) < 1000)
	// {
	// 	//pioneer.setHead(passivearea);
	// 	now = nlab.getTime();
	// }

	// 最低値の算出
	DATA_grid_part lowest_grid = {{0,0}, INFINITY};
	for(int i = 0; i < gridnum1; i++)
	{
		for(int j = 0; j < gridnum2; j++)
		{
			double value = data->stress_pote[i][j];
			if(value < lowest_grid.value)
			{
				lowest_grid.value = value;
				lowest_grid.grid.i = i;
				lowest_grid.grid.j = j;
			}
		}
	}
  // パッシブエリア
  DATA_xy goal = {grid2double_x(lowest_grid.grid.i), grid2double_y(lowest_grid.grid.j)};

	// 本体
	command.robot.path = GeneratePotePathToGoal(goal);
	command.robot.speed = 500;
	// 本体・回転
	command.body.deg = bodyRotateTo(data->human[0].posi);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	if(exist_judge_human(data->human[0].posi)) panRotateTo(data->human[0].posi);
  else command.tilt.deg = panSetRelativeAngle(0);
	command.pan.speed = 20;
	// 頭部・チルト
	if(exist_judge_human(data->human[0].posi)) tiltRotateTo(data->human[0].posi);
  else command.tilt.deg = 0;
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

void PlayUsingBall_BC::behavior()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal({0, 1500});
	command.robot.speed = 500;
	// 本体・回転
	command.body.deg = bodyRotateTo(data->human[0].posi);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panRotateTo(data->human[0].posi);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->human[0].posi);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

//////////

void Halt_BC::behavior()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal(data->robot.posi);
	command.robot.speed = 0;
	// 本体・回転
	command.body.deg = data->robot.body_deg;
	command.body.speed = 0;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panSetRelativeAngle(0);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = 0;
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

void Home_BC::behavior()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal({-2500, 500});
	command.robot.speed = 500;
	// 本体・回転
	command.body.deg = 0;
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panSetRelativeAngle(0);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = 0;
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

void SoundGaze_BC::behavior()
{
}

void SoundMove_BC::behavior()
{
}


void Exploring_BC::behavior()
{
	DATA_xy goal = data->tmp_goal;

	// MISSING, GO_TO_OWNER, EXPLO
	// 本体
	command.robot.path = GeneratePotePathToGoal(goal);
  command.robot.speed = 500;
	// 本体・回転
	command.body.deg = bodyRotateTo(goal);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
  command.pan.deg = panRotateTo(goal);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(goal);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}

/*void AttentionGetting_BC::behavior()
{
}

void DirectionalSignal_BC::behavior()
{
}*/

void LeadingBehavior_BC::behavior()
{
}

void SoundBehavior_BC::behavior()
{
}
