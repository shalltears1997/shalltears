#include <iostream>
#include <chrono>
#include <thread>
#include <random>  // 乱数

#include <vector>
#include <cmath>
#include <ctime>

#include "class.hpp"
#include "gv_extern.hpp"
#include "tk30_ActivateBehavior.hpp"
#include "l_tk-library.hpp"

#include "new_natsukido.hpp"
#include "nk_Vocalization.hpp"

#include "tk22_SelectBehavior.hpp"
#include "tk32_BasicBehaviors.hpp"
#include "tk51_RobotSimulator.hpp"
#include "tk51_SubGoalGenerator.hpp"

#include "tk_data_class.hpp"
#include "tk_Parameter.hpp"

int tmp_count = 0;

// 人・ボールの計画経路
std::vector<std::vector<double>> plan_o =
{
  {0.0, 4000, 4500, 1500},
  {1.9, 3250, 3750, 1500},
  {4.9, 3250, 3750, 1500},
  {6.8, 2500, 3000, 1500},
  {12.1, 0, 1500, 1500},
  {17.1, 0, 1500, 1500},
  {21.9, -2500, 2500, 1500},
  {22.9, -2500, 2500, 1500},
  {26.5, -2500, 500, 1500},
  {27.5, -2500, 500, 1500},
  {36.5, 2500, 500, 1500},
  {37.5, 2500, 500, 1500},
  {41.1, 2500, 2500, 1500},
  {42.1, 2500, 2500, 1500},
  {47.0, 0, 1500, 1500},
  {49.7, -1500, 1500, 1500},
  {128.8, -1500, 1500, 1500},
  {130.6, 0, 1500, 1500},
  {145.8, 0, 1500, 1500},
  {147.6, -1000, 1500, 1500},
  {149.4, -2000, 1500, 1500},
  {240.0, -2000, 1500, 1500},
  {248.5, 2500, 3000, 1500},
  {250.4, 3250, 3750, 1500},
  {253.4, 3250, 3750, 1500},
  {255.4, 4000, 4500, 1500},
  {257.4, 5000, 5000, 1500},
  {360.0, 4000, 4500, 1500},
  {361.9, 3250, 3750, 1500},
  {364.9, 3250, 3750, 1500},
  {366.8, 2500, 3000, 1500},
  {375.9, -2500, 2250, 1500},
  {400.0, -2500, 2250, 1500},
  {401.0, -2500, 2250, 500},
  {411.0, -2500, 2250, 500},
  {412.0, -2500, 2250, 1500},
  {460.0, -2500, 2250, 1500},
  {469.1, 2500, 3000, 1500},
  {471.0, 3250, 3750, 1500},
  {474.0, 3250, 3750, 1500},
  {475.9, 4000, 4500, 1500},
  {477.9, 5000, 5000, 1500},
  {1000.0, 5000, 5000, 1500}
};


std::vector<std::vector<double>> plan_s =
{
  {0.0, 5000, 5000, 1500},
  {120.0, 4000, 4500, 1500},
  {121.9, 3250, 3750, 1500},
  {124.9, 3250, 3750, 1500},
  {126.8, 2500, 3000, 1500},
  {130.6, 1000, 1500, 1500},
  {135.6, 1000, 1500, 1500},
  {138.2, 0, 500, 1500},
  {140.7, -1000, 1500, 1500},
  {143.3, 0, 2500, 1500},
  {145.8, 1000, 1500, 1500},
  {147.6, 1000, 1500, 1500},
  {149.4, 2000, 1500, 1500},
  {209.4, 2000, 1500, 1500},
  {255.4, 2000, 1500, 1500},
  {265.4, 2000, 1500, 1500},
  {271.7, -1500, 1500, 1500},
  {360.0, -1500, 1500, 1500},
  {366.8, -1500, 1500, 1500},
  {369.1, -2500, 750, 1500},
  {420.0, -2500, 750, 1500},
  {421.0, -2500, 750, 500},
  {431.0, -2500, 750, 500},
  {432.0, -2500, 750, 1500},
  {440.0, -2500, 750, 1500},
  {449.9, 2500, 3000, 1500},
  {451.8, 3250, 3750, 1500},
  {454.8, 3250, 3750, 1500},
  {456.7, 4000, 4500, 1500},
  {458.7, 5000, 5000, 1500},
  {1000.0, 5000, 5000, 1500}
};

std::vector<std::vector<double>> plan_b;

class Plan_class
{
	// 人を規定ルートで動かすための変数
	int p = 0;
	std::vector<std::vector<double>> *plan = nullptr;
	DATA_xy *data = nullptr;
	double *height = nullptr;

public:
	Plan_class(DATA_xy *data, double *height, std::vector<std::vector<double>> *plan)
	{
		this->plan = plan;
		this->data = data;
		this->height = height;
	}

	Plan_class(DATA_xy *data, std::vector<std::vector<double>> *plan)
	{
		this->plan = plan;
		this->data = data;
		this->height = height;
	}

	void follow(double time_duration)
	{
		// 現在参照すべき計画を見つける
		while(!((*plan)[p][0] <= time_duration && time_duration < (*plan)[p+1][0])) p++;
		double r = (time_duration - (*plan)[p][0]) / ((*plan)[p+1][0] - (*plan)[p][0]);

		// 値を計算し代入する
		if( !std::isinf((*plan)[p+1][1]) && !std::isinf((*plan)[p+1][2]) )
		{
			(*data).x = (*plan)[p][1] + r * ((*plan)[p+1][1] - (*plan)[p][1]);
			(*data).y = (*plan)[p][2] + r * ((*plan)[p+1][2] - (*plan)[p][2]);
			if(height != nullptr) *height = (*plan)[p][3];
		}
	}

};

void calcBallParm(DataClass *data_in)
{
	double ball_x = 0;
	double ball_y = 0;

	//ボールの位置計算
	ball_x = data_in->robot.posi.x + data_in->ball_st.ball_distance[0] * cos( (data_in->ball_st.ball_theta[0] + data_in->robot.body_deg )*(PI/180) );
	ball_y = data_in->robot.posi.y + data_in->ball_st.ball_distance[0] * sin( (data_in->ball_st.ball_theta[0] + data_in->robot.body_deg )*(PI/180) );
	//nlabのiSpace空間内に収まるようにボール座標を修正
	if ( ball_x > 3000 ) ball_x = 3000;
	else if ( ball_x < -3000 ) ball_x = -3000;
	if ( ball_y > 3000 ) ball_y = 3000;
	else if ( ball_y < 0 ) ball_y = 0;

	// データクラスに代入
	if (data_in->ball_st.confidence_sum[0] > 10 && data_in->ball_st.circularity[0] > 0.5)
	{
		// ボール座標を代入 2015/12/26	//ボールらしさがあるとき
		// ball.setData(0, ball_x, ball_y);
		data_in->ball_st.ball_presence = true;
		data_in->ball_tk.x = ball_x;
		data_in->ball_tk.y = ball_y;
	}
	else
	{
		data_in->ball_st.ball_presence = false;
	}

	// ◇2 人がボールを投げる
	if (data_in->ball_st.ball_presence && (data_in->ball_st.confidence_sum[0]>60&&data_in->ball_st.SD[0]>10) )
	{
		data_in->ball_yo.ball_t = 1;
		data_in->ball_yo.t_id = 1;
	}
}


void MainThread()
{
  data_pre.motion_id = 0;
  data.motion_id = 0;

	// 新・時間（2017/10/28、塚田）
	std::chrono::system_clock::time_point time_ini = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point time_last = time_ini;
  	std::chrono::system_clock::time_point time_now;
	std::chrono::system_clock::time_point time_almost_finish;
	double time_interval, time_duration;

	double interval = 0;

	double str_move = 0.0;

	// コンポーネント作成
	// 1. データ入力
	//..
	// 2. パラメータ算出
	//..
	// 3. ふるまい選択
	BehaviorSelectorClass BehaviorSelector;
	// 3. ふるまい選択
	calcDeltaATA calcATA;
	// 4. ふるまい実行
	Component_ActivateBehavior c4_ab(&data, &data_pre);
	// 5. 司令値算出
	//ボーカリゼーションの実行
	Vocalization Vocal;
	//..


	data.human_leave_location = new DATA_xy;
	*data.human_leave_location = {4500, 4500};

	Plan_class own_plan(&data.human[0].posi, &data.human[0].height, &plan_o);
	Plan_class str_plan(&data.human[1].posi, &data.human[1].height, &plan_s);
	Plan_class ball_plan(&data.ball_tk, &plan_b);

	while(GoOnFlag)
	{
		time_now = std::chrono::system_clock::now();
		time_interval = (double)(std::chrono::duration_cast<std::chrono::microseconds>(time_now - time_last).count())/1000.0;
		time_duration = (double)std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_ini).count()/1000;

		interval  = (double)std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_ini).count();
    
	// GLの保存のため
	    data.time_duration = time_duration;

		// シミュレータのときは指定時間ごとに、実機のときはロボットの位置がZPSによって更新されたとき(約100ms間隔)に実行
    // if( (ROBOT==2 && time_interval > simRefreshRatio_clock) || (ROBOT==1 && data.zps_robot_new) )
		if( (ROBOT==2 && time_duration*10 > data.loop_num+1) || (ROBOT==1 && data.zps_robot_new) )
		{
			time_last = time_now;
			data.loop_num++;
			data.zps_robot_new = false;

			// 1. コンポーネント：データ入力
			{
				// コマンド
				{
					// キーボードからの入力（描画）
					data.GoToHomeFlag = GLData.data.GoToHomeFlag;
					data.pioneer_activate = GLData.data.pioneer_activate;
          data.plot_type = GLData.data.plot_type;
          data.home = GLData.data.home;

					// マウスからの入力（描画）
					data.pa_human[0] = GLData.data.pa_human[0];
					data.pa_human[1] = GLData.data.pa_human[1];
					data.pa_human[2] = GLData.data.pa_human[2];
					data.sensitivity_of_anxiety = GLData.data.sensitivity_of_anxiety;
					data.ball_tk = GLData.data.ball_tk;
/*
					//なつき度をマウスからの入力にあわせて更新
					if(GLData.drag_flag == 30)
					{
						//calcATA.ata_ini = GLData.data.pa_human[0].ata;
					}
*/
				}

				// ロボット角度・ボール情報
				{
					if(ROBOT == 1)
					{
						// MotionControlからの入力
						connectWithMC.dataInput(&data);
					}
					else
					{
						// 描画からの入力
					  data.ball_st = GLData.data.ball_st;
					  data.sound_move = GLData.data.sound_move;
						// ロボットシミュレータからの入力
						RobotSimulator.receiveDataDeg(&data);
					}
				}

				// パンチルト角度の代入（仮措置）
				data.pan = data_pre.command.pan.deg;
				data.tilt = data_pre.command.pan.deg;


				// 位置
				// if(poO == 1 || poS == 1 || poF == 1) connectWithZPS.dataInput(&data);  // ZPSからの入力
				// else data.human = GLData.data.human;  // 描画からの入力

				// 位置
				{
					// 人
					// とりあえず全部描画から取得
					{
						data.human = GLData.data.human;
						for(int i=1;i<100;i++)data.object[i] = GLData.data.object[i];
					}
					// ZPSの場合はデータ入力
					int human_from_zps[3] = {poO, poS, poF};
					for(int i=0; i<3; i++)
					{
						if(human_from_zps[i] == 1)
						{
							// ZPSから
							data.human[i].posi   = connectWithUnity.dataInput_human(i);
							data.human[i].height = connectWithUnity.dataInput_human_height(i);
						}
					}

					if(map==1)
					{
						for(int i=1;i<100;i++)GLData.data.object[i]=data.object[i];
					}


					//PRINT("Unity data x:%f, y:%f, z:%f\n", data.human[0].posi.x, data.human[0].posi.y, data.human[0].height);

					// 人：規定ルートを辿る
					// own_plan.follow(time_duration);
					// str_plan.follow(time_duration);
					// ball_plan.follow(time_duration);

					// ロボット
					if(poR == 1)
					{
						data.robot.posi = connectWithUnity.dataInput_robot();
					}
					else
					{
						// ロボットシミュレータから
						RobotSimulator.receiveDataLocation(&data);
					}
				}


				// 環境地図
				{
					// 環境地図からの入力
					// DATA_obstacle obstacle[10]
				}
				// else
				{
					// 描画からの入力
					// DATA_obstacle obstacle[10]
				}


				// ベクトルの算出
				for(int i=0; i<data.human_num; i++)
				{
					double vec_x = data.human[i].posi.x - data_pre.human[i].posi.x;
					double vec_y = data.human[i].posi.y - data_pre.human[i].posi.y;

					// 歩行速度：4 km/h = 10/9 m/s = 10000/90 mm/100ms = 1000/9 mm/100ms = 111mm/100ms
					if( sqrt(pow(vec_x, 2) + pow(vec_y, 2)) > 50 )
					{
						data.human_vec[i] = {vec_x, vec_y};
						// data.human_move[i] = true;
					}
					else
					{
            data.human_vec[i] = {0, 0};
						// data.human_move[i] = false;
            // printf("%d\n", data.human_move[i]);
					}
				}


				// data.robot = {pioneer.current.posex, pioneer.current.posey, 0, pioneer.draw_body_deg};
			}

			// 2. コンポーネント：パラメータ算出
			{
				// 人が「いた場所」の管理
				// 人がいなくなったら「いた場所」を削除する
				if(exist_judge_human(data.human[0].posi))
				{
					if(data.human_leave_location != nullptr)
					{
						delete data.human_leave_location;
						data.human_leave_location = nullptr;
					}
				}

				// 人がいなくなった（＝今いなくて、前回いた）なら、「いた場所」を生成
				if(!exist_judge_human(data.human[0].posi) && exist_judge_human(data_pre.human[0].posi))
				{
					data.human_leave_location = new DATA_xy;
					*data.human_leave_location = data_pre.human[0].posi;
				}

				// 人がいない時間を管理
				for(int i=0; i<2; i++)
				{
					if(exist_judge_human(data.human[i].posi))
					{
						data.human_non_exist_duration[i] = 0;
					}
					else
					{
						data.human_non_exist_duration[i] += time_interval/1000;
					}
				}


				// 実機のボール位置等の算出
				if(ROBOT == 1)
				{
					//calcBallParm(&data);
				}
				else
				{
					// ボールが領域にあればボールありとする
					if(exist_judge_robot(data.ball_tk) && calDistance(data.robot.posi, data.human[0].posi) < 2000)
					{
						double theta_ro = calDegree(data.robot.posi, data.human[0].posi);
						double theta_rb = calDegree(data.robot.posi, data.ball_tk);
						double theta_diff = theta_ro - theta_rb;
						if(theta_diff > 360) theta_diff -= 360;
						if(theta_diff <-360) theta_diff += 360;

						if(abs(theta_diff) < 20) data.ball_st.ball_presence = true;
					}
					else
					{
						data.ball_st.ball_presence = false;
					}

					// ボール遊びの必要度を更新
					if(data.ball_st.ball_presence)
					{
						data.ball_play_required_level = 100;
					}
					else
					{
						data.ball_play_required_level = data_pre.ball_play_required_level;
					}

					// ボールがロボットのそばにあって、かつ、グリッパーが閉じられているなら、ボールはロボットと共に動く
					if(calDistance(data_pre.robot.posi, data_pre.ball_tk) < 50 && data_pre.command.gripper.deg > 90) data.ball_tk = data.robot.posi;
				}

				//行動特性パラメータ（なつき度）の変化を行う箇所
				if (interval > 100)
				{
					//data.pa_human[0].ata = calcATA.calcDeltaAttachment(data.pa_human[0].ata, data.human[0].posi.x, data.human[0].posi.y, data.robot.posi.x, data.robot.posi.y, data.human[0].height, human_face_direction);
/*
					if(exist_judge_human(data.human[1].posi))
					{
						data.pa_human[1].ata = calcATA.calcDeltaAttachment_stranger(data.pa_human[1].ata, data.human[0].posi.x, data.human[0].posi.y, data.human[1].posi.x, data.human[1].posi.y, data.robot.posi.x, data.robot.posi.y, data.human[1].height, human_face_direction);
					}*/
				}

				// ストレスポテンシャルグリッド生成（目的地なし）
				PotentialGenerator stress_pote_tmp(data.human, data.pa_human, data.obstacle);
				data.stress_pote = stress_pote_tmp.getPoteGridMap();


				// 行動要因パラメータ
				calcParmClass parm(data);

				// // owner：寂しさ
				// data.pa_human[0].mis = parm.NextMiss(data.robot.posi);
				// // stranger：不安
				// data.pa_human[1].anx = parm.NextAnxiety(data.robot.posi);
				// // stranger：探索欲求
				// data.pa_human[1].exp = parm.NextExploreHuman(data.robot.posi);

		// 認知値 = 前の認知値 + 感受性 * 観測値P + 観測値M)
		// 実効値 = 離別時間 * 認知値

				PositionClass own, str;
				own.setPos(data.human[0].posi);
				str.setPos(data.human[1].posi);


			// 観測値（増減値）
				// owner
				data.pa_human[0].danxP = 0;
				data.pa_human[0].danxM = parm.dAnxietyM(data.robot.posi, own, data.pa_human[0].ata);

				data.pa_human[0].dexpP = 0;
				data.pa_human[0].dexpM = 0;

				data.pa_human[0].dmisP = parm.dMissP(data.robot.posi, own, data.pa_human[0].ata, false)
																	+ parm.dMiss_Door(data.robot.posi, own);
				data.pa_human[0].dmisM = parm.dMissM(data.robot.posi, own, data.pa_human[0].ata, false);

				// stranger
				data.pa_human[1].danxP = parm.dAnxietyP(data.robot.posi, str, data.pa_human[1].ata)
																	+ parm.dAnxiety_Coming(data.robot.posi, str);
				data.pa_human[1].danxM = 0;

				data.pa_human[1].dexpP = parm.dExploreP(data.robot.posi, str, data.pa_human[1].ata);
				data.pa_human[1].dexpM = parm.dExploreM(data.robot.posi, str, data.pa_human[1].ata);

				data.pa_human[1].dmisP = 0;
				data.pa_human[1].dmisM = 0;

				str_move = parm.dAnxiety_Coming(data.robot.posi, str);

				// PRINT("parm: %7.3lf  %7.3lf  %7.3lf  %7.3lf  %7.3lf  %7.3lf", data.pa_human[0].dmisP)


			// 認知値（差分）
				// owner
				data.pa_human[0].danx = 0;
				data.pa_human[0].dexp = 0;
				data.pa_human[0].dmis = parm.adjust_delta(data_pre.pa_human[0].mis_c,
																									data.pa_human[0].dmisP + data.pa_human[0].dmisM)
																	;//* exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_mis));

				// stranger
				data.pa_human[1].danx = parm.adjust_delta(data_pre.pa_human[1].anx_c,
																									data.pa_human[1].danxP + data.pa_human[0].danxM)
																	;//* exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_anx))
																	//* exp(-data.human_non_exist_duration[1]/(data.TimeConstant2*data.K_anx));
				data.pa_human[1].dexp = parm.adjust_delta(data_pre.pa_human[1].exp_c,
																									data.pa_human[1].dexpP + data.pa_human[1].dexpM)
																	;//* exp(-data.human_non_exist_duration[1]/(data.TimeConstant2*data.K_exp));
				data.pa_human[1].dmis = 0;


			// 認知値（履歴）
				// owner
				data.pa_human[0].anx_c += 0;
				data.pa_human[0].exp_c += 0;
				data.pa_human[0].mis_c += data.pa_human[0].dmis;
				// data.pa_human[0].mis_c = 0;

				// stranger
				data.pa_human[1].anx_c += data.pa_human[1].danx;
				// data.pa_human[1].anx_c = 100;
				data.pa_human[1].exp_c += data.pa_human[1].dexp;
				data.pa_human[1].mis_c = 0;

			// 実効値
				// owner
				data.pa_human[0].anx = 0;
				data.pa_human[0].exp = 0;
				data.pa_human[0].mis = data.pa_human[0].mis_c
																* exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_mis));
				// stranger
				data.pa_human[1].anx = data.pa_human[1].anx_c
																* exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_anx))
																* exp(-data.human_non_exist_duration[1]/(data.TimeConstant2*data.K_anx));
				data.pa_human[1].exp = data.pa_human[1].exp_c
																* exp(-data.human_non_exist_duration[1]/(data.TimeConstant2*data.K_exp));
				data.pa_human[1].mis = 0;


				// 領域：探索欲求
				parm.NextExplorePerPlace2(data.robot.posi, &data.unknown_area_finer);
				data.explore = parm.NextExplorePlace2(&data.unknown_area_finer);

        if(data_pre.pa_human[0].mis > 0.01 && data.pa_human[0].mis < 0.01) data.last_MorA = 0;
        if(data_pre.pa_human[1].anx > 0.01 && data.pa_human[1].anx < 0.01) data.last_MorA = 1;
			}

			{
				//監視必要度の変化

			//領域

				CalWeight.weight_area();
				CalWeight.max_weight();
				CalWeight.min_weight();
				CalWeight.normalize_weight();

				data.max_area_grid = {};
				data.max_necessity_area = 0.0;

				//PRINT("weightarea[0][0]:%lf\n",data.weigth_area[0][0]);
				for(int i=0;i<6;i++)
				{
					for(int j=0;j<3;j++)
					{
						if(data.monitoring_necessity[i][j] < 1.0)
						{
							//data.monitoring_necessity[i][j] += 0.00001 *(1/(data.weigth_area[i][j]+1.0)) + CalWeight.getblind(i,j)*0.0005;

						}
						//PRINT("mon[0][3]:%lf\n",data.monitoring_necessity[i][j]);
					}
				}

     			/*if(visible_area(data.human[0].posi) || judge_in_ellipse(data.human[0].posi.x,data.human[0].posi.y) || grid_in_judge(data.max_area_grid.i,data.max_area_grid.j,data.robot.posi))//|| data.motion_id == MONITORING_AREA)
				{
					if(data.monitoring_necessity[0][2] > 0.0)data.monitoring_necessity[0][2] -= 0.01;
					PRINT("x:%d,y:%d\n",data.max_area_grid.i,data.max_area_grid.j);
				}*/

				DATA_grid owner ={};
				DATA_grid robot ={};

				owner = owner_grid(data.human[0].posi);
				robot = owner_grid(data.robot.posi);

				if(owner.i != -1 && owner.j != -1)
				{
					if(data.monitoring_necessity[owner.i][owner.j] > 0.0)data.monitoring_necessity[owner.i][owner.j] -= 0.01;
				}


				if(robot.i != -1 && robot.j != -1)
				{
					if(data.monitoring_necessity[robot.i][robot.j] > 0.0)data.monitoring_necessity[robot.i][robot.j] -= 0.01;
				}


				//変な選ばれ方する→一回対象になったら最後までそこを見続ける処理しないと

				//PRINT("moni:%lf\n",data.object[0].monitoring_necessity);

				for(int i=0;i<6;i++)
				{
					for(int j=0;j<3;j++)
					{
						if(data.max_necessity_area < data.monitoring_necessity[i][j])
						{
							data.max_necessity_area = data.monitoring_necessity[i][j];
							data.max_area_grid.i = i;
							data.max_area_grid.j = j;
						}
					}
				}


				/*else
				{
					if(data.monitoring_necessity[data.max_area_grid.i][data.max_area_grid.j] )
					{

					}
				}*/

				//PRINT("monitoring:%lf\n",data.human[0].monitoring_necessity);
				//	PRINT("%lf %lf",grid_to_coordinate_x(data.max_area_grid.i),grid_to_coordinate_y(data.max_area_grid.j));


				//動物体
				for(int i=1;i<20/*data.object.size()+1*/;i++)
				{
					C_object[i].initialize_data(i);//////////ほんとは動的配列の引数付きコンストラクタを定義したいよー
					C_object[i].setblind();

					data.object[i].inmap = C_object[i].recog_inmap(i);

					if(data.object[i].inmap == 1)
					{
						if(exist_judge_human(data.human[0].posi) == false && exist_judge_human(data.human[1].posi) == false)
						{
							data.object[i].monitoring_necessity = 1.0;
						}
						//PRINT("x:%d y:%d\n",C_object[i].grid_object().i,C_object[i].grid_object().j);

						if(data.object[i].monitoring_necessity <= 1.0)
						{
							data.object[i].monitoring_necessity += 0.1*1/(data.weigth_area[C_object[i].grid_object().i][C_object[i].grid_object().j]+1) + C_object[i].getblind()*0.01;

							//data.object[i].monitoring_necessity += 0.0001*1/(data.weigth_area[C_object[i].grid_object().i][C_object[i].grid_object().j]+1) + C_object[i].getblind()*0.01;
						}
						if(data.object[i].monitoring_necessity >= 0.0 && calDistance(data.object[i].posi,data.human[0].posi) < 900.0 )
						{
							//	data.object[i].monitoring_necessity -= 0.010;
						}
					}
				}

				data.object_maxID = 0;
				data.max_necessity_obj = 0.0;

				C_object[0].owner_necessity(owner_grid(data.human[0].posi));

				for(int i=1;i<20/*C_object.size()+1*/;i++)
				{
					//C_object[i].setnecessity();//こここうしちゃうと結局新行動モード選択でもしきい値もうけてるのと一緒　いくつ以上で必要性がある、とかじゃない

					if(/*C_object[i].getnecessity() == true &&*/ data.object[i].inmap)
					{
						data.max_necessity_obj = C_object[i].change_max(data.max_necessity_obj);
						data.object_maxID = i;
					}
				}
				//ほんとは一回監視対象になったら0になるまで監視対象で在り続ける処理
				if(data.object_maxID == 0)
				{
					data.object_maxID = -1;
				}

				if(data.human[0].monitoring_necessity >0.9)
				{
					data.object_maxID = -2;
				}

			}


			// 3. コンポーネント：ふるまい選択
			{
				// 行動選択
				data.motion_id = BehaviorSelector.select(data.human, data.pa_human, data.ball_yo, data.ball_st, data.sound_move, config_data, data.nazo);
			}

			//ボーカリゼーションのフラグ選択
			if(data.object[1].monitoring_necessity < 0.5)
			{
				sound_flag = Vocal.activation(data.robot.posi, data.pa_human[0].mis, data.pa_human[1].anx, str_move) * 10;
			}
			else
			{
				sound_flag = 0.0;
			}

      // if(random_model)
      // {
      //   std::random_device rd;
      //   std::mt19937 mt(rd());
      //   std::uniform_real_distribution<double> score(0.0, 1.0);
      //   double change = score(mt);
      //
      //   double denominator = 10.0 * 60 * 10;
      //   double r_change = 30.0;
      //
      //   // MISSING, GO_TO_OWNER, EXPLORE_STRANGER, GO_TO_DOOR, EXPLORE
      //   int behavior_list[5] = {1, 1, 1, 1, 1};
      //   int behavior_list_th[5] = {0, 0, 0, 0, 0};
      //
      //   // オーナーがいない場合、MSSINGとGO_TO_OWNERは選択しない
      //   if(!exist_judge_human(data.human[0].posi))
      //   {
      //     behavior_list[0] = 0;
      //     behavior_list[1] = 0;
      //   }
      //   // ストレンジャーがいない場合、EXPLORE_STRANGERは選択しない
      //   if(!exist_judge_human(data.human[1].posi))
      //   {
      //     behavior_list[2] = 0;
      //   }
      //
      //   int sum = 0;
      //   for(int i=0; i<5; i++)
      //   {
      //     sum += behavior_list[i];
      //   }
      //   double ratio = 1.0 / (double)sum;
      //
      //
      //   for(int i=0; i<5; i++)
      //   {
      //     for(int j=0; j<i+1; j++)
      //     {
      //       behavior_list_th[i] += behavior_list[j];
      //     }
      //   }
      //
      //   if(change < r_change * ratio * behavior_list_th[0] / denominator)
      //   {
      //     tmp_count++;
      //     data.motion_id = MISSING;
      //   }
      //   else if(change < r_change * ratio * behavior_list_th[1] / denominator)
      //   {
      //     tmp_count++;
      //     data.motion_id = GO_TO_OWNER;
      //   }
      //   else if(change < r_change * ratio * behavior_list_th[2] / denominator)
      //   {
      //     tmp_count++;
      //     data.motion_id = EXPLORE_STRANGER;
      //   }
      //   else if(change < r_change * ratio * behavior_list_th[3] / denominator)
      //   {
      //     tmp_count++;
      //     data.motion_id = GO_TO_DOOR;
      //   }
      //   else if(change < r_change * ratio * behavior_list_th[4] / denominator)
      //   {
      //     tmp_count++;
      //     data.motion_id = EXPLORE;
      //   }
      //   else
      //   {
      //     data.motion_id = data_pre.motion_id;
      //   }
      //
      //   // PRINT("%d", tmp_count);
      //   // // PRINT("%d", sum);
      //   // PRINT("%lf", r_change * ratio * behavior_list_th[0] / denominator);
      //   // PRINT("%lf", r_change * ratio * behavior_list_th[1] / denominator);
      //   // PRINT("%lf", r_change * ratio * behavior_list_th[2] / denominator);
      //   // PRINT("%lf", r_change * ratio * behavior_list_th[3] / denominator);
      //   // PRINT("%lf", r_change * ratio * behavior_list_th[4] / denominator);
      //   // PRINT("%d  %d  %d  %d  %d", behavior_list_th[0], behavior_list_th[1], behavior_list_th[2], behavior_list_th[3], behavior_list_th[4]);
      // }

      // ランダムモデル
      // 目的地をランダムに選択する
      if(random_model && !data.home)
      {
  			data.motion_id = RANDOM;

      	std::random_device rd;
      	std::mt19937 mt(rd());
      	std::uniform_real_distribution<double> score(0.0, 1.0);
      	double change = score(mt);

        // 10分間に60回目的地を変更する計算
      	if (change < 60.0 / (10.0 * 60 * 10) )
      	{
          tmp_count++;

          {
          	std::random_device rd;
          	std::mt19937 mt(rd());
          	std::uniform_real_distribution<double> score(-3000, 3000);
          	data.tmp_goal.x = score(mt);
          }

          {
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distribution<double> score(0, 3000);
            data.tmp_goal.y = score(mt);
          }
      	}
        else
        {
          data.tmp_goal.x = data_pre.tmp_goal.x;
          data.tmp_goal.y = data_pre.tmp_goal.y;
        }

      	PRINT("%d  %lf: %lf  %lf", tmp_count, change, data.tmp_goal.x, data.tmp_goal.y);
      }


			// 4. コンポーネント：ふるまい実行（然るべきコンストラクタの呼び出しと実行）
			// 現状だとふるまいのクラス名がDEFINE文によって置き換えられている状態なので要解決
			c4_ab.act();


			// 5. コンポーネント：司令値算出
			{
				if(!stop_flag2)
				{
					// サブゴール算出
					SubGoalGenerator route(data.command.robot.path);
					//data.sub_goal = route.generateSubGoal(300);

					//double v = 450;

					//data.sub_goal = route.generateSubGoal(v * BehaviorSelector.robot_velocity(data.tmp_goal, data.robot.posi, data.sensitivity_of_anxiety));

//					//print_delta = 0.915 * sqrt(pow(data.robot.posi.x - data.sub_goal.x, 2) + pow(data.robot.posi.y - data.sub_goal.y, 2));

					//ココらへんに速度に繋がるサブゴールを作成させたい（ストレスポテンシャルフィールドを参照したい）
					data.sub_goal = route.generateSubGoal(data.command.robot.speed * BehaviorSelector.robot_velocity(data.tmp_goal, data.robot.posi, data.sensitivity_of_anxiety));
					// data.sub_goal = {0, 1500};

					/*if(mode == 1)
					{
						data.sub_goal = route.generateSubGoal(data.command.robot.speed * BehaviorSelector.robot_velocity(data.tmp_goal, data.robot.posi, data.sensitivity_of_anxiety));
					}
					else
					{
						data.sub_goal = route.generateSubGoal(data.command.robot.speed * BehaviorSelector.robot_velocity(data.tmp_goal, data.robot.posi, data.sensitivity_of_anxiety));
					}
*/
					velocity = 0.915 * sqrt(pow(data.robot.posi.x - data.sub_goal.x, 2) + pow(data.robot.posi.y - data.sub_goal.y, 2));

				  // 今度はサブゴールまでの角度をもとに、角度が大きいときはサブゴールまでの距離を縮めるようにする
					if(calDistance(data.command.robot.path[0], data.sub_goal) > 100)
					{
            double d_max = 100;
					  double deg = calDegree(data.robot.posi, data.sub_goal);
						double deg_diff = deg - data.robot.body_deg;
						if(deg_diff > 180) deg_diff -= 360;
						if(deg_diff <-180) deg_diff += 360;
						deg_diff = abs(deg_diff);
						if(deg_diff>d_max) deg_diff=d_max;

            // PRINT("%lf", calDistance(data.command.robot.path[0], data.sub_goal));
					  double dis = (calDistance(data.command.robot.path[0], data.sub_goal)-100) * (1-deg_diff/d_max) + 100;

					  data.sub_goal.x = data.robot.posi.x + dis * cos(deg*PI/180.0);
					  data.sub_goal.y = data.robot.posi.y + dis * sin(deg*PI/180.0);

						// PRINT("%lf", calDistance(data.command.robot.path[0], data.sub_goal));
					}

					//print_delta = 0.915 * sqrt(pow(data.robot.posi.x - data.sub_goal.x, 2) + pow(data.robot.posi.y - data.sub_goal.y, 2));

					// 発進・停止 判断
					if(ROBOT == 1)
					{
						if(data.motion_id == PLAY_USING_BALL)
						{
							data.robot_move_or_halt = (calDistance(data.robot.posi, data.sub_goal) >  30 - 1) * data.pioneer_activate;
						}
						else
						{
							data.robot_move_or_halt = (calDistance(data.robot.posi, data.sub_goal) > 100 - 1) * data.pioneer_activate;
						}
					}
					else
					{
						if(data.motion_id == PLAY_USING_BALL)
						{
							data.robot_move_or_halt = (calDistance(data.robot.posi, data.sub_goal) >  30 - 1);
						}
						else
						{
							data.robot_move_or_halt = (calDistance(data.robot.posi, data.sub_goal) > 100 - 1);
						}
					}

				}
			}

			// 6. 出力
			{
				// ロボットを動かす
				if(ROBOT == 1)
				{
					// MotionControlへの出力
					connectWithMC.dataOutput(&data);
				}
				else
				{
					// シミュレータロボットへのサブゴール出力
					RobotSimulator.sendData(data.sub_goal, data.command);
				}

        // おおよその所要時間を算出
        time_almost_finish = std::chrono::system_clock::now();
        double computing_time = (double)(std::chrono::duration_cast<std::chrono::microseconds>(time_almost_finish - time_last).count()) / 1000.0;

        // printf("%lf  %lf\n", time_interval, computing_time);
        // デバッグ
				Debug.printDebug(data.loop_num, time_duration, time_interval, computing_time);


				//ここのprintfは確認用のものです　変化確認後消す予定
				//printf("monitoring = %lf\n", print_delta);
				//printf("time = %lf\n", print_delta2);
				//printf("velocity = %lf\n", velocity);
				//printf("sound_Flag = %lf\n", sound_flag);
				//printf("monitoring:%lf\n",data.human[0].monitoring_necessity);
				//printf("\nmonitoring_necessity:%lf\n",data.object[1].monitoring_necessity);


				Debug.logSave(data.loop_num, time_duration, time_interval, computing_time);

				// 描画への出力
				// GLData.data.robot = data.robot;
				// GLData.data.pa_human[0] = data.pa_human[0];
				// GLData.data.pa_human[1] = data.pa_human[1];
				// GLData.data.pa_human[2] = data.pa_human[2];
				// GLData.data.stress_pote = data.stress_pote;
				// GLData.data.pf_goal = data.pf_goal;
				// GLData.data.ball_yo = data.ball_yo;
				// GLData.data.ball_tk = data.ball_tk;
				// GLData.data.ball_is_Known_tk = data.ball_is_Known_tk;
				// GLData.data.motion_id = data.motion_id;
				// GLData.data.command = data.command;
				// GLData.data.sub_goal = data.sub_goal;
				// GLData.data.pioneer_activate = data.pioneer_activate;
				GLData.data = data;

				// データ履歴の保存
				data_pre = data;
			}

			// これでループ処理は一通り終了
			// 実機のときのために、更新フラグを立てる（これを受け、MCにデータを送信する）
			data.is_new = true;

		}
	}

	// ここはループの外：スレッド処理終了
	std::cout << std::endl << "The MainThread finished." << std::endl;
}
