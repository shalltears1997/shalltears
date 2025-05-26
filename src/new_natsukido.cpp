#include <iostream>
#include <cmath>
#include <chrono>
#include <ctime>
#include <termios.h>
#include "new_natsukido.hpp"
#include "class.hpp"
#include "gv_extern.hpp"

bool calcDeltaATA::calchumanmove(double human_position_x, double human_position_y, double robot_x, double robot_y)
{
	// 人のふるまい判定

	/*距離*/
	{
		// 人の速度ベクトル
		hvelocity.vx = human_position_x - prehumanpose.posex;
		hvelocity.vy = human_position_y - prehumanpose.posey;

		if (calVecVal(hvelocity) < 0.3 && calVecVal(hvelocity) > -0.3) hvelocity = { 0, 0 };//unityの誤差補正

		// ロボットの速度ベクトル
		rvelocity.vx = robot_x - prerobotpose.posex;
		rvelocity.vy = robot_y - prerobotpose.posey;

		// 相対速度ベクトル
		Vec difference = { hvelocity.vx - rvelocity.vx, hvelocity.vy - rvelocity.vy };

		double contribution = 0.0;

		// 人の距離変化への寄与度
		if (calVecVal(hvelocity) == 0.0) contribution = 0;
		else contribution = inProduct(difference, hvelocity) / (calVecVal(hvelocity) * calVecVal(hvelocity));

		// 人とロボットが近づいた距離
		distance.vx = robot_x - human_position_x;
		distance.vy = robot_y - human_position_y;

		double vary = predis - calVecVal(distance);

		// 人の動きによってロボットと近づいた距離
		double approach_distance_by_human = vary * contribution;

		//人によるロボットへの接近距離を記録
		move_distance.erase(move_distance.begin());
		move_distance.push_back(approach_distance_by_human);

		int move_num = 0;

		for (int i = 0; i < move_distance.size(); i++) move_num += (move_distance[i] > 0.3);

		if (move_num > 5)
		{
			// 人がロボットの方に動いている判定
			moving_to_dog = true;
		}
		else if (move_distance.size() + 1 - move_num > 5)
		{
			// 人はロボットの方には動いていない判定
			moving_to_dog = false;
		}
		else
		{
			// 判定不可：人の動きは前の瞬間と変わっていません
			moving_to_dog = moving_to_dog_pre;
		}
	
		// 近づいた判定
		if (moving_to_dog_pre && !moving_to_dog)
		{
			judge = true;
			K = 0.2;
			source_of_T = 10;
		}
		else
		{
			judge = false;
		}

		//このループの情報を保存する
		moving_to_dog_pre = moving_to_dog;
		prehumanpose.posex = human_position_x;
		prehumanpose.posey = human_position_y;
		prerobotpose.posex = robot_x;
		prerobotpose.posey = robot_y;
		predis = calVecVal(distance);

	}

	return judge;
}

bool calcDeltaATA::calchumanZ(double human_z)
{

	/*体勢*/
	{
		//しゃがんだ判定: 高さ80cmより小さくなった
		if (human_z <= 1100 && prez > 1100)
		{
			judge = true;
			K = 0.3;
			source_of_T = 15;
		}
		//しゃがみフラグの消滅判定
		if ((human_z > 1100) && prez <= 1100) judge = false;
		prez = human_z;
	}

	return judge;
}

bool calcDeltaATA::calchumanface(int direction)
{
	/*顔の向き*/
	{
		//顔向けた判定: 見てない⇒見た
		if ((direction == 0 || direction == 1) && hfdpre == 2)
		{
			judge = true;
			K = 0.1;
			source_of_T = 10;
		}
		//顔の向きフラグの消滅判定
		if (direction == 2 && (hfdpre == 0 || hfdpre == 1)) judge = false;
	
		hfdpre = direction;
	}


	return judge;
}

//人の過度な接近を判定するための関数
 bool calcDeltaATA::attackness(double robot_x, double robot_y, double human_position_x, double human_position_y, double attach)
 {
	 	//ロボットと人の距離を算出
		distance2.vx = robot_x - human_position_x;
		distance2.vy = robot_y - human_position_y;

		double close = calVecVal(distance2);

	 //人の過度な接近判定
	 //ロボットの人との接近距離よりも近い状態であること、そしてポテンシャルフィールドによって囲まれてしまって動けず、ロボットの速度ベクトルが極端に小さくなることで過度な接近可能性が発生
	{
		 if(close < attachment2distance_tmp(attach) && calVecVal(rvelocity) < 0.3)
		{
			attackness_judge = true;
		}
		else
		{
			attackness_judge = false;
		}
	}

	 //接近判定が生じた時間を計測
	 if(attackness_judge == true)
	 {
		 time_begin = std::chrono::system_clock::now();
	 }
	 else //接近判定が生じなくなった時間を計測
	 {
		 time_end = std::chrono::system_clock::now();
	 }

	 //ストレスを与えてるような攻撃（過度な接近）が続いている時間を算出
	 //過度な接近が始まると正の方向に増加、接近が終わると0に戻ったあとに負の方向に増加
	 double delta_time = (double)(std::chrono::duration_cast<std::chrono::milliseconds>(time_begin - time_end).count()) / 1000;

	 //print_delta = delta_time;

	 //print_delta2 = calVecVal(rvelocity);

	 //仮の条件として10秒続いたら下がるようにする
	 if(delta_time >= 10.0)
	 {
		 K = -0.1;
		 source_of_T = 10;
		 time_end = std::chrono::system_clock::now();
		 judge = true;

	 }
	 else
	 {
	 	 judge = false;
	 }

	 return judge;
 }

/*
 bool calcDeltaATA::social_reference(double human0_position_x, double human0_position_y, double human1_position_x, double human1_position_y)
 {
	 	//上で書いてあった人のロボットへの接近判定を再利用した
	 	//理由としてはロボットへの接近判定は人がロボットに近づくことを参照してなつき度を変化させることを意味し、
	    //一方で社会的参照はロボットの信頼しやすいownerがstrangerに対して接近することを参照するため、判定の流れは同じ流れでいいではないかと考えたためである.
	 	//もし、これ以上の表現方法があったら変更してもOK

		// 人のふるまい判定

		/*距離*/
/*		{
			// ownerの速度ベクトル
			ow_velocity.vx = human0_position_x - prehuman0pose.posex;
			ow_velocity.vy = human0_position_y - prehuman0pose.posey;

			if (calVecVal(ow_velocity) < 0.3 && calVecVal(ow_velocity) > -0.3) hvelocity = { 0, 0 };//unityの誤差補正

			// strangerの速度ベクトル
			st_velocity.vx = human1_position_x - prehuman1pose.posex;
			st_velocity.vy = human1_position_y - prehuman1pose.posey;

			// 相対速度ベクトル
			Vec difference = { ow_velocity.vx - st_velocity.vx, ow_velocity.vy - st_velocity.vy };

			double contribution = 0.0;

			// ownerの距離変化への寄与度
			if (calVecVal(ow_velocity) == 0.0) contribution = 0;
			else contribution = inProduct(difference, ow_velocity) / (calVecVal(ow_velocity) * calVecVal(ow_velocity));

			// ownerとstrangerが近づいた距離
			distance_people.vx = human1_position_x - human0_position_x;
			distance_people.vy = human1_position_y - human0_position_y;

			double vary = predis - calVecVal(distance_people);

			// ownerの動きによってstrangerと近づいた距離
			double approach_distance_by_human = vary * contribution;

			//ownerによるstrangerへの接近距離を記録
			move_distance_people.erase(move_distance_people.begin());
			move_distance_people.push_back(approach_distance_by_human);

			int move_num = 0.0;

			for (int i = 0; i < move_distance_people.size(); i++) move_num += (move_distance_people[i] > 0.3);

			if (move_num > 5)
			{
				// ownerがstrangerの方に動いている判定
				moving_to_stranger = true;
			}
			else if (move_distance_people.size() + 1 - move_num > 5)
			{
				// ownerはstrangerの方には動いていない判定
				moving_to_stranger = false;
			}
			else
			{
				// 判定不可：人の動きは前の瞬間と変わっていません
				moving_to_stranger = moving_to_stranger_pre;
			}

			// 近づいた判定
			if (moving_to_stranger_pre && !moving_to_stranger)
			{
				judge = true;
				K = 0.4;
				source_of_T = 10;
			}
			else
			{
				judge = false;
			}

			//このループの情報を保存する
			moving_to_stranger_pre = moving_to_stranger;
			prehuman0pose.posex = human0_position_x;
			prehuman0pose.posey = human0_position_y;
			prehuman1pose.posex = human1_position_x;
			prehuman1pose.posey = human1_position_y;
			predis_people = calVecVal(distance_people);

		}

 	 return judge;
 }
*/

 bool calcDeltaATA::social_reference(double human0_position_x, double human0_position_y, double human1_position_x, double human1_position_y)
 {
	 	//ロボットと人の距離を算出
		double x = human0_position_x - human1_position_x;
		double y = human0_position_x - human1_position_y;

		double close = sqrt(pow(x,2) + pow(y,2));

		{
			 if(close <= 1200 && calVecVal(hvelocity) < 0.3)
			{
				time_begin2 = std::chrono::system_clock::now();
			}
			else
			{
				time_end2 = std::chrono::system_clock::now();
			}
		}

		 //strangerがownerに対して近づいた上で, それが長く続いているのであれば社会的参照としてstrangerに対する許容度は上がると考えた
		 double delta_time = (double)(std::chrono::duration_cast<std::chrono::milliseconds>(time_begin2 - time_end2).count()) / 1000;

		 if(delta_time > 60)
		 {
			K_stranger = 0.2;
			source_of_T_stranger = 10;
			judge = true;
		 }
		 else
		 {
			 judge = false;
		 }

	 return judge;
 }


double calcDeltaATA::calcDeltaAttachment(double pre_attach, double position_x, double position_y, double robot_x, double robot_y, double height, int face_direction)
{

	//人の顔の向きを判定
	face_judge = calchumanface(face_direction);

	//人の接近判定
	approach_judge= calchumanmove(position_x, position_y, robot_x, robot_y);

	//人のしゃがみ判定
	sitting_judge = calchumanZ(height);

	//人の過度な接近判定
	attackness_judge = attackness(robot_x, robot_y, position_x, position_y, pre_attach);

	//なつき度変化フラグのどれかが立ったかを確認するフラグ
	all_judge = approach_judge + sitting_judge + face_judge + attackness_judge;

	//なつき度の変化フラグによる分岐部分
	if(all_judge) //フラグ成立時の処理
	{
		time_ini = std::chrono::system_clock::now();

		double T = source_of_T + data.sensitivity_of_anxiety / 10;
	}

	//なつき度変化のフラグを初期化(しないとずっとフラグが続くので不自然)
	all_judge = false;

	// イベント発生からの時間経過
	std::chrono::system_clock::time_point time_now = std::chrono::system_clock::now();
	double t = (double)(std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_ini).count()) / 1000;

	//なつき度変化の発生(フラグが生じない場合にもこれを働かせている)
	//ポジティブな状況が起こったとしてポンと上がるだけじゃなくてそれが若干続く感じにした
	//でもなつき度が上がる状況が生じたとしても、その影響力は徐々に弱まっていくもの（忘却に近い感じ）であるので徐々に上がり幅は少なくなる　→　最終的には0
	double next_ata = pre_attach + K * exp(-t / T);

	//なつき度がオーバーフローしないようにする
	if(next_ata > 100)
	{
		next_ata = 100;
	}
	else if(next_ata < 0)
	{
		next_ata = 0;
	}

	// なつき度更新
	return next_ata;
}

double calcDeltaATA::calcDeltaAttachment_stranger(double pre_attach, double human0_position_x, double human0_position_y, double human1_position_x, double human1_position_y , double robot_x, double robot_y, double height, int face_direction)
{
/*
	//人の顔の向きを判定
	face_judge = calchumanface(face_direction);

	//人の接近判定
	approach_judge= calchumanmove(human1_position_x, human1_position_y, robot_x, robot_y);

	//人のしゃがみ判定
	sitting_judge = calchumanZ(height);

	//人の過度な接近判定
	attackness_judge = attackness(robot_x, robot_y, human1_position_x, human1_position_y, pre_attach);
*/
	//社会的参照を判定　現状この判定はstrangerの移動がないとちょっと近づくと上がる傾向にあるので改修必要？
	social_reference_judge = social_reference(human0_position_x, human0_position_y, human1_position_x, human1_position_y);

	//なつき度変化フラグのどれかが立ったかを確認するフラグ
	all_judge2 = social_reference_judge;

	//なつき度の変化フラグによる分岐部分
	if(all_judge2) //フラグ成立時の処理
	{
		time_ini2 = std::chrono::system_clock::now();

		T_stranger = source_of_T_stranger + data.sensitivity_of_anxiety / 10;
	}

	//なつき度変化のフラグを初期化(しないとずっとフラグが続くので不自然)
	all_judge2 = false;

	// イベント発生からの時間経過
	std::chrono::system_clock::time_point time_now = std::chrono::system_clock::now();
	double t = (double)(std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_ini2).count()) / 1000;

	//なつき度変化の発生(フラグが生じない場合にもこれを働かせている)
	double next_ata = pre_attach + K_stranger * exp(-t / T_stranger);

	//なつき度がオーバーフローしないようにする
	if(next_ata > 100)
	{
		next_ata = 100;
	}
	else if(next_ata < 0)
	{
		next_ata = 0;
	}

	// なつき度更新
	return next_ata;
}
