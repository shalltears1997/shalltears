// ﻿#include <iostream>
// #include <ctime>
// #include <cmath>
// #include "opengl.hpp"
//
// #include "gv_extern_id.hpp"
//
// #include "c_struct_position.hpp"
// #include "c_dog.hpp"
//
// // コンストラクタ
// Dog::Dog()
// {
// 	current = InitPose();
// 	previous = InitPose();
// 	current.posex = 2500.0;
// 	current.posey = 2500.0;
// 	current.deg = 0.0;
// 	//head.posex = 2500.0;//峻治追加
// 	//head.posey = 2500.0;//峻治追加
// 	pan_deg = 0.0;//峻治追加
// 	tilt_deg = 0.0;//峻治追加
// 	sensitivity = 20.0;
// 	willingness = true;
// 	movingflag = false;
// }
//
// void Dog::setGoalPosition(Position setposition)
// {
// 	goal.posex = setposition.posex;
// 	goal.posey = setposition.posey;
// }
//
// void Dog::setPosition(double posX,double posY, double degree)
// {
// 	previous = current;
// 	current.posex = posX;
// 	current.posey = posY;
// 	current.deg = degree;
// 	//head.posex = posX;
// 	//head.posey = posY;
// //	pan_deg = degree;//峻治追加 //tkcomment out
// }
// void Dog::setPosition(double posX , double posY)
// {
// 	static Vec temp= InitVec();
// 	static int cnt = 0;
//
// 	previous = current;
// 	current.posex = posX;
// 	current.posey = posY;
// 	current.deg = calDegree(previous, current);
//
// 	if(cnt < 3)
// 	{
// 		Vec tempd = calVec(current , previous);
// 		temp.vx += tempd.vx;
// 		temp.vy += tempd.vy;
// 		cnt++;
// 	}
// 	if(cnt == 3){
// 		movvec.vx = temp.vx / 3.0;
// 		movvec.vy = temp.vy / 3.0;
// 		cnt = 0;
// 		temp = InitVec();
// 	}
// }
// void Dog::setPosition(double posX , double posY,int simu)
// {
// 	previous = current;
// 	current.posex = posX;
// 	current.posey = posY;
// 	//head.posex = posX;//峻治追加
// 	//head.posey = posY;//峻治追加
// }
//
//
//
// bool Dog::updateKnownLevel()
// {
// 	// 時間経過による既知度の減少
// 	for(int i=0; i<area1; i++)
// 	{
// 		for(int j=0; j<area2; j++)
// 		{
// 			if(known_level[i][j] > 0.0)	known_level[i][j] -= 0.00001*(120 - sensitivity);
// 		}
// 	}
//
// 	// ロボットのいる領域の既知度の増加
// 	int i = (int)(current.posex + WIDTH_ETHO/2)/(WIDTH_ETHO/area1);
// 	int j = (int)(HEIGHT_ETHO - current.posey)/(HEIGHT_ETHO/area2);
// 	if(i < 0 || area1 <= i || j < 0 || area2 <= j)
// 	{
// 		return false;
// 	}
// 	else
// 	{
// 		if(known_level[i][j] < 1.0) known_level[i][j] += 0.10;
// 	}
// 	return true;
// }
//
//
//
//
// // tk1701
// // 従来のsetHead()のほとんどは、本来はsetBodyDeg()役割を持たせるべき
// // この関数が呼ばれた時点で即座にロボットの向きが変化するのではなく「最終的な角度」として記憶しておく
// // シミュレータの場合、setBodyDeg()で向きを設定する。実機の場合、移動が終わったらこの角度になるように回転する。
//
//
// // 現在角度
// void Dog::setTransBodyDeg(double degree)
// {
// 	previous = current;
// 	current.deg = degree;
//
// 	trans_body_deg = degree;
// }
//
// double Dog::getTransBodyDeg()
// {
// 	return trans_body_deg;
// }
//
// // 目標角度
// void Dog::setFinalBodyDeg(double degree)
// {
// 	final_body_deg = degree;
// }
// void Dog::setFinalBodyDeg(Position target)
// {
// 	// final_body_deg = calDegree(goal, target);
// }
// double Dog::getFinalBodyDeg()
// {
// 	return final_body_deg;
// }
//
// // 描画角度
// void Dog::setDrawBodyDeg(double degree)
// {
// 	draw_body_deg = degree;
// }
//
//
// // tk1701
// // シミュレータのとき、ロボットの向きを変える
// void Dog::rotateSimRobot()
// {
// 	current.deg = final_deg;
// }
//
// void Dog::setRobotDeg(double degree)
// {
// 	robot_angle = degree;
// }
//
//
//
//
// void Dog::Head(Position target)//峻治追加
// {
// 	static double dist_pt = 0.0 ;//pionerrとtargetの距離
// 	static double  robot_height = 550;
// 	static double  human_height = 1600;
// 	static double  Difference_height = 0.0 ;
// 	dist_pt = calDistance(current, target);
// 	pan_deg = calDegree(current, target) ;
// 	Difference_height = human_height - robot_height ;
// 	if(motionID == PASSIVE_BEHAVIOR || motionID == EXPLORE || motionID == EXPLORE_TO_OWNER || motionID == EXPLORE_TO_STRANGER || motionID == EXPLORE_TO_PLACE)
// 	{
// 	tilt_deg = 0.0;
// 	}
// 	else tilt_deg = (atan2( Difference_height , dist_pt ) * 180.0 / PI ) ;
// }
//
//
// void Dog::Head(Position target, Position current, int motionID, double attachment)//峻治追加
// {
// 	static double dist_pt = 0.0 ;//pionerrとtargetの距離
// 	static double  robot_height = 550;
// 	static double  human_height = 1600;
// 	static double  Difference_height = 0.0 ;
// 	dist_pt = calDistance(current, target);
// 	pan_deg = calDegree(current, target);
// 	Difference_height = human_height - robot_height ;
// 	if(motionID == EXPLORE_TO_OWNER)
// 	{
// 	tilt_deg = (atan2( Difference_height , dist_pt ) * 180.0 / PI ) /** attachment /100*/ ;
// 	std::cout<<"Exploretilt"<<tilt_deg<< std::endl;
// 	}
// 	else tilt_deg = 0.0;
// 	FILE *fp;
// 	if( ( fp = fopen("dist&tilt.txt","a") ) == NULL){
// 			//printf("file open error!!\n");
// 			//exit(EXIT_FAILURE);
// 	}
// 	fprintf(fp,"dist_ot , tilt_deg: %lf , %lf\n",dist_pt ,tilt_deg );
// 	fclose(fp);
// }
//
// void Dog::Head(Position target, Position current, double attachment)//峻治追加
// {
// 	static double dist_pt = 0.0 ;//pionerrとtargetの距離
// 	static double  robot_height = 550;
// 	static double  human_height = 1600;
// 	static double  Difference_height = 0.0 ;
// 	dist_pt = calDistance(current, target);
// 	pan_deg = (calDegree(current, target)) /** attachment / 100*/ ;
// 	//cout<<"dist_ot"<<dist_pt<<endl;
// 	//cout<<"pan_deg"<<pan_deg<<endl;
// 	//cout<<"DIST_OT"<<dist_ot<<endl;
// 	Difference_height = human_height - robot_height ;
// 	tilt_deg = (atan2( Difference_height , dist_pt ) * 180.0 / PI ) * attachment /100 ;
// 	//cout<<"tilt_deg"<<tilt_deg<<endl;
// 	FILE *fp;
// 	if( ( fp = fopen("attach100&tilt.txt","a") ) == NULL){
// 			//printf("file open error!!\n");
// 			//exit(EXIT_FAILURE);
// 	}
// 	fprintf(fp,"dt , tilt_deg: %lf , %lf\n" , attachment ,tilt_deg );
// 	fclose(fp);
// }
// void Dog::HeadTime(double attachment)//峻治追加
// {
// 	headtime =500/* -30 * attachment + 2400 */; //峻治
// }
//
// void Dog::attachment_Head(double attachment)//峻治未完成今後
// {
// 	pan_deg = getPan_deg() * attachment/100 ; //峻治
// 	tilt_deg = getTilt_deg() * attachment/100 ;
// }
//
// // ポテンシャルとKnownレベルの値が条件をクリアした領域の数
// void Dog::setAgreeCnt(int a_cnt , int u_cnt)
// {
// 	//printf("cnt : %d , %d\n",a_cnt , u_cnt);
// 	if( (u_cnt > a_cnt/2.0 && a_cnt > 10) || (u_cnt > a_cnt/3.0 && a_cnt > 3) ) willingness = true;
// 	else willingness = false;
// }
//
// // 状態表示
// void Dog::disp(){
// 	std::cout << "explore:" << explore.value << std::endl;
// 	std::cout << "anxiety:" << anxiety.value << std::endl;
// 	std::cout << "miss:" << miss.value << std::endl;
// }
//
//
// bool Dog::exist(double area[])
// {
// 	if(area[0] <= current.posex && current.posex <= area[1] && area[3] <= current.posey && current.posey <= area[2] ){
// 		return true;
// 	}
// 	else {
// 		return false;
// 	}
// }
// float Dog::getValue(short type)
// {
// 	if(type == MIS)return miss.value;
// 	else if(type == O_ANX) return o_anxiety.value;
// 	else if(type == O_EXP) return o_explore.value;
//
// 	else if(type == S1_MIS)return s_miss.value;
// 	else if(type == ANX) return anxiety.value;
// 	else if(type == S1_EXP) return s_explore.value;
//
// 	else if(type == S2_MIS)return s2_miss.value;
// 	else if(type == S2_ANX) return s2_anxiety.value;
// 	else if(type == S2_EXP) return s2_explore.value;
//
// 	else if(type == B_MIS)return b_miss.value;
// 	else if(type == B_ANX) return b_anxiety.value;
// 	else if(type == B_EXP) return b_explore.value;
//
// 	else if(type == OY_MIS)return oy_miss.value;
// 	else if(type == OY_ANX) return oy_anxiety.value;
// 	else if(type == OY_EXP) return oy_explore.value;
//
// 	else if(type == EXP) return explore.value;
//
// 	else return -1;
// }
// short Dog::getState(short type)
// {
// 	/*if(type == MIS)return miss.state;
// 	else if(type == ANX) return anxiety.state;
// 	else if(type == EXP) return explore.state;*/
// 	if(type == MIS)return miss.state;
// 	else if(type == O_ANX) return o_anxiety.state;
// 	else if(type == O_EXP) return o_explore.state;
//
// 	else if(type == S1_MIS)return s_miss.state;
// 	else if(type == ANX) return anxiety.state;
// 	else if(type == S1_EXP) return s_explore.state;
//
// 	else if(type == S2_MIS)return s2_miss.state;
// 	else if(type == S2_ANX) return s2_anxiety.state;
// 	else if(type == S2_EXP) return s2_explore.state;
//
// 	else if(type == B_MIS)return b_miss.state;
// 	else if(type == B_ANX) return b_anxiety.state;
// 	else if(type == B_EXP) return b_explore.state;
//
// 	else if(type == OY_MIS)return oy_miss.state;
// 	else if(type == OY_ANX) return oy_anxiety.state;
// 	else if(type == OY_EXP) return oy_explore.state;
//
// 	else if(type == EXP) return explore.state;
// 	else return -1;
// }
