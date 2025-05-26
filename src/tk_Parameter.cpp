
#include "gv_extern.hpp"
#include "tk30_ActivateBehavior.hpp"
#include "l_tk-library.hpp"
#include "tk_data_class.hpp"
#include "tk_Parameter.hpp"


double calcParmClass::adjust_delta(double value, double delta)
{
  if(value + delta > sMAX) delta = sMAX - value;
  if(value + delta < 0) delta = - value;
  return delta;
}

double calcParmClass::RefreshValue(double value, double delta)
{
  value = value + delta;
  if(value > sMAX) value = sMAX;
  if(value < 0) value =    0;
  return value;
}

calcParmClass::calcParmClass(DataClass data)
{
  owner.setPos(data.human[0].posi);
  stranger.setPos(data.human[1].posi);

  miss = data.pa_human[0].mis;
  anxiety = data.pa_human[1].anx;
  explore_human = data.pa_human[1].exp;
  explore_place = data.explore;

  attachment = data.pa_human[0].ata;
  acceptance = data.pa_human[1].ata;
  sensitivity = data.sensitivity_of_anxiety;

  std::vector<std::vector<double>> tmp(gridnum1, std::vector<double>(gridnum2, 0.05));
  add_value = tmp;

  std::vector<std::vector<double>> unknown_area_tmp(gridnum1, std::vector<double>(gridnum2, 50));
  this->dExplorePlace_MAX = dExplorePlace2({0, 1500}, &unknown_area_tmp);
}


double calcParmClass::approachDistance(double ata)
{
  return -ata/2 + 1.25;
}

double calcParmClass::distanceConverter(double x, double ata)
{
  return (x - approachDistance(ata/50-1)) / 2;
}


double calcParmClass::approachDistance_inverse(double ata)
{
  return ata/2 + 1.25;
}
double calcParmClass::distanceConverter_inverse(double x, double ata)
{
  return (x - approachDistance_inverse(ata/50-1)) / 2;
}


// ゲイン: 0.05 = 0.5/1s = 100/3m20s(=200s)
// 条件分岐は「<：未満」か「>=：以上」

// double calcParmClass::dMiss_Door(DATA_xy robot, PositionClass human)
// {
//   // ドア方向の単位ベクトル
//   DATA_xy door = {data.Door_pose.posex, data.Door_pose.posey};
//   double length = calDistance(human.pos(), door);
//   DATA_xy door_vec = {(door.x-human.x)/length, (door.y-human.y)/length};
//
//   // 人がドアに向かっているか
//   double direction_to_door_level = (door_vec.x * data.human_vec[0].x/100 + door_vec.y * data.human_vec[0].y/100) * data.human_move[0];
//   if(direction_to_door_level < 0) direction_to_door_level = 0;
//
//   // 人がドアに到達しそうか
//   double going_to_door_level = direction_to_door_level * (10-length/1000)/10;
//   if(isnan(going_to_door_level)) going_to_door_level = 0;
//   // PRINT("going_to_door_level: %10.5lf", going_to_door_level);
//   // PRINT("going_to_door_level: %10.5lf", (10-length/1000)/10);
//
//   // 人がドアに到達しそうか
//   // 0.111で割ることで、標準の速さ（4 km/h）で歩いた時に、100msでAnxietyが1上がるように正規化している
//   // --> ストレンジャーの存在による不安感と同じ上昇幅になる
//   // ここにKをかけることで、ストレンジャーの存在による不安感に対する上昇幅を調節できる
//   double K = 10;
//   coming_to_str_level = direction_to_str_level * K / 0.111 * exp(-pow(length/1000,2.0)/(2*pow(2.0,2.0)));
//
//   return going_to_door_level;
// }

double calcParmClass::dMiss_Door(DATA_xy robot, PositionClass human)
{
  // ストレンジャーから見たロボット方向の単位ベクトル
  DATA_xy door = {data.Door_pose.posex, data.Door_pose.posey};
  double length = calDistance(human.pos(), door);
  DATA_xy door_vec = {(door.x-human.x)/length, (door.y-human.y)/length};

  double going_to_door_level;
  // PRINT("%d", exist_judge_robot(human.pos()));
  double K = 5;

  if(!exist_judge_robot(human.pos()))
  {
    going_to_door_level = 1.0 * K / 0.111 * 1.0;
    going_to_door_level = 1.0 * K;
  }
  else if(std::isnan(door_vec.x) || std::isnan(door_vec.y))
  {
    going_to_door_level = 0;
  }
  else
  {
    // 方向がドアか否か
    // double direction_to_door_level = (door_vec.x * data.human_vec[0].x/1000 + door_vec.y * data.human_vec[0].y/1000) * data.human_move[0];
    double direction_to_door_level = door_vec.x * data.human_vec[0].x/1000 + door_vec.y * data.human_vec[0].y/1000;
    if(direction_to_door_level < 0) direction_to_door_level = 0;

    // ドアに近いか否か
    // 0.111で割ることで、標準の速さ（4 km/h）で歩いた時に、100msでAnxietyが1上がるように正規化している
    // --> ストレンジャーの存在による不安感と同じ上昇幅になる
    // ここにKをかけることで、ストレンジャーの存在による不安感に対する上昇幅を調節できる
    going_to_door_level = direction_to_door_level * K / 0.111 * exp(-pow(length/1000,2.0)/(2*pow(2.0,2.0)));
  }

  return going_to_door_level;
}




// 寂しさ増加
double calcParmClass::dMissP(DATA_xy robot, PositionClass human, double ata, bool prediction)
{
  // 距離
  double x;
  if(prediction && !human.exist())
  {
    // 人の残像：人が領域におらず、かつ、予想の時
    x = calDistance(robot, *data.human_leave_location) / 1000;
  }
  else
  {
    // 人の前方（人の現在位置 + 移動ベクトル）
    DATA_xy owner_front = {human.x + data.human_vec[0].x, human.y + data.human_vec[0].y};
    x = calDistance(robot, owner_front) / 1000;
  }

  double sigma = 6.0/3.0;
  double non_observable_level = 1 - exp(-pow(x,2.0)/(2*pow(sigma,2.0)));
  // PRINT("%lf", non_observable_level);
  // PRINT("%d", (prediction && !human.exist()));
  // PRINT("%d:  %lf  %lf", data.human_move[0], data.human_vec[0].x, data.human_vec[0].y);

  return non_observable_level;
}


// 寂しさ減少
double calcParmClass::dMissM(DATA_xy robot, PositionClass human, double ata, bool prediction)
{
  // 距離
  double x;
  if(prediction && !human.exist())
  {
    // 人の残像：人が領域におらず、かつ、予想の時
    x = calDistance(robot, *data.human_leave_location) / 1000;
  }
  else
  {
    // 人の前方（人の現在位置 + 移動ベクトル）
    DATA_xy owner_front = {human.x + data.human_vec[0].x, human.y + data.human_vec[0].y};
    x = calDistance(robot, owner_front) / 1000;
  }

  ata = (ata-50)/50;
  double non_concern_level_of_miss = 1 - ata/2.0;
// PRINT("%lf", non_concern_level_of_miss);
  return - non_concern_level_of_miss;
}

double calcParmClass::dAnxiety_Coming(DATA_xy robot, PositionClass human)
{
  // ストレンジャーから見たロボット方向の単位ベクトル
  double length = calDistance(human.pos(), robot);
  DATA_xy str_vec = {(robot.x-human.x)/length, (robot.y-human.y)/length};

  double coming_to_str_level;

  if(std::isnan(str_vec.x) || std::isnan(str_vec.y))
  {
    coming_to_str_level = 0;
  }
  else
  {
    // ストレンジャーがドアに向かっているか
    // double direction_to_str_level = (str_vec.x * data.human_vec[1].x/1000 + str_vec.y * data.human_vec[1].y/1000) * data.human_move[1];
    double direction_to_str_level = str_vec.x * data.human_vec[1].x/1000 + str_vec.y * data.human_vec[1].y/1000;
    if(direction_to_str_level < 0) direction_to_str_level = 0;

    // 人がこちらに向かっているか
    // 0.111で割ることで、標準の速さ（4 km/h）で歩いた時に、100msでAnxietyが1上がるように正規化している
    // --> ストレンジャーの存在による不安感と同じ上昇幅になる
    // ここにKをかけることで、ストレンジャーの存在による不安感に対する上昇幅を調節できる

    // double K = 10;
    // coming_to_str_level = direction_to_str_level * K / 0.111
    //                         * exp(-pow(length/1000,2.0)/(2*pow(2.0,2.0)));

    double d = attachment2distance_tmp(data.pa_human[1].ata);
    double y0 = 1 - exp(-pow(d/1000,2.0)/(2*pow(2.0,2.0))) + 1;
    double K = 5;
    coming_to_str_level = direction_to_str_level * K / 0.111
                                 * y0 * exp(-pow(length/1000/y0,2.0)/(2*pow(2.0,2.0)));

    //coming_to_str_level = direction_to_str_level * K / 0.111
                //                 * y0 * (1-tanh(length/1000*0.7/4));

    // double K = 10;
    // if(length < attachment2distance_tmp(data.pa_human[1].ata))
    // {
    //   coming_to_str_level = direction_to_str_level * K / 0.111;
    // }
    // else
    // {
    //   coming_to_str_level = direction_to_str_level * K / 0.111
    //                           * exp(-pow(length/1000,2.0)/(2*pow(2.0,2.0)));
    // }
    // PRINT("coming_to_str_level: %10.5lf  %10.5lf", direction_to_str_level);
  }

  return coming_to_str_level;
}

// 不安さの変化量
double calcParmClass::dAnxietyP(DATA_xy robot, PositionClass human, double ata)
{
  // 不安増加
  // 人の前方（人の現在位置 + 移動ベクトル）
  DATA_xy human_front = {human.x + data.human_vec[1].x, human.y + data.human_vec[1].y};
  double x = calDistance(robot, human_front) / 1000;

  double sigma = 4.0/3.0;
  ata = (ata-50)/50;
  // double negative_aura = exp(-pow(x,2.0)/(2*pow(sigma,2.0))) - (1 + ata);
  double negative_aura = -ata * exp(-pow(x/ata,2.0)/(2*pow(sigma,2.0)));

  //double negative_aura = -ata * (1-tanh(x*0.7/2));

  return negative_aura * (negative_aura>0);
}

// 不安さの変化量
double calcParmClass::dAnxietyM(DATA_xy robot, PositionClass human, double ata)
{
  // 不安減少
  double x = calDistance(robot, human.pos()) / 1000;

  double sigma = 4.0/3.0;
  ata = (ata-50)/50;
  // double positive_aura = - exp(-pow(x,2.0)/(2*pow(sigma,2.0))) - (ata - 1);
  double positive_aura = -ata * exp(-pow(x/ata,2.0)/(2*pow(sigma,2.0)));

  //double positive_aura = -ata * (1- tanh(x*0.7/2));

  return positive_aura * (positive_aura<0);
}

// 探索欲求の変化量
double calcParmClass::dExploreP(DATA_xy robot, PositionClass human, double ata)
{
  // 距離
  double x = calDistance(robot, human.pos()) / 1000;

  double sigma = 6.0/3.0;
  double non_observable_level = 1 - exp(-pow(x,2.0)/(2*pow(sigma,2.0)));

  return non_observable_level;
}

// 探索欲求の変化量
double calcParmClass::dExploreM(DATA_xy robot, PositionClass human, double ata)
{
  // 距離
  double x = calDistance(robot, human.pos()) / 1000;

  ata = (ata-50)/50;
  // double non_concern_level_of_explore = (1 - ata)/4.0 * tanh(x-7) + (3+ata)/4.0;
  double non_concern_level_of_explore = -ata/4.0 * tanh(x-7) + 1 + ata/4.0;

  return - non_concern_level_of_explore;
}


// 領域に対する探索欲求
double calcParmClass::dExplorePlace(DATA_xy robot)
{
  // 次のループにおける各領域ごとの未知度を計算してしまう
  std::vector<std::vector<double>> tmp_unknown_area = NextExplorePerPlace(robot);
  double explore = NextExplorePlace(tmp_unknown_area);
  // 現在の値と次の値を引き算して差分を求める
  return explore - data.explore;
}




// 領域に対する探索欲求：領域の値の算出
std::vector<std::vector<double>> calcParmClass::NextExplorePerPlace(DATA_xy robot)
{
  std::vector<std::vector<double>> tmp_unknown_area(data.unknown_area);  // コピーコンストラクタ

  // 時間経過による未知度増加
  for(int i=0; i<area1; i++)
  {
  	for(int j=0; j<area2; j++)
  	{
  		tmp_unknown_area[i][j] += 0.05 * (1 - sensitivity/100);
  		if(tmp_unknown_area[i][j] > 100*10) tmp_unknown_area[i][j] = 100*10;
  	}
  }
  // ロボットのいる領域の未知度減少
  if(exist_judge_robot(robot))
  {
  	int i = (robot.x+WIDTH_ETHO/2)/1000;
  	int j = (HEIGHT_ETHO-robot.y)/1000;
  	tmp_unknown_area[i][j] -= 1.0;
  	if(tmp_unknown_area[i][j] < 0) tmp_unknown_area[i][j] = 0;
  }

  return tmp_unknown_area;
}

// 領域に対する探索欲求：パラメータの算出
double calcParmClass::NextExplorePlace(std::vector<std::vector<double>> unknown_area)
{
	double explore = 0;
	for(int i=0; i<area1; i++)
	{
		for(int j=0; j<area2; j++)
		{
			explore += unknown_area[i][j];
		}
	}
	return explore / (6*3);
}



        // 領域に対する探索欲求
        double calcParmClass::dExplorePlace2(DATA_xy robot, std::vector<std::vector<double>> *unknown_area_in)
        {
          // 次のループにおける各領域ごとの未知度を計算してしまう
          // std::vector<std::vector<double>> tmp_unknown_area;
          // std::vector<std::vector<double>> tmp_unknown_area(data.unknown_area_finer);  // コピーコンストラクタ
          // NextExplorePerPlace2(robot, &tmp_unknown_area);
          // double explore = NextExplorePlace2(tmp_unknown_area);
          // // 現在の値と次の値を引き算して差分を求める
          // return explore - data.explore;

          // 全グリッドの変化量の合計
          double plus_sum  = increment * 30*60 - this->surplus_sum;
          double minus_sum = NextExplorePerPlace2_decrement(robot, unknown_area_in);
          double grid_delta_sum = plus_sum + minus_sum;

          // パラメータ "ExplorePlace" の変化量
          double dExplorePlace2 = grid_delta_sum / (60 * 30);
          // PRINT("%lf | %lf = %lf %+lf", dExplorePlace2, grid_delta_sum, plus_sum, minus_sum);

          return dExplorePlace2;
        }

        // 従来手法の見かけのマイナスゲイン：-0.065291
        // 新方式の見かけのマイナスゲインの理論値：-0.065
        // 新方式の見かけのマイナスゲインの実効値：-0.061894

        // プラスゲイン：0.03

        void calcParmClass::dExplorePlace2_surplus_sum(std::vector<std::vector<double>>* unknown_area_in)
        {
          this->surplus_sum = 0;
          for(int i=0; i<gridnum1; i++)
          {
            for(int j=0; j<gridnum2; j++)
            {
              if((*unknown_area_in)[i][j] + increment > 100*10) surplus_sum += (*unknown_area_in)[i][j] + increment - 100*10;
            }
          }
          // PRINT("surplus_sum: %lf", surplus_sum);
        }

        // 環境の未知度_グリッド：減算
        double calcParmClass::NextExplorePerPlace2_decrement(DATA_xy robot, std::vector<std::vector<double>>* unknown_area_in)
        {
          double delta_sum = 0;

          // 指定された位置を元に、計算するべき矩形範囲を算出
          DATA_xy start = adjust_point((DATA_xy){robot.x - diameter/2, robot.y + diameter/2});
          DATA_xy   end = adjust_point((DATA_xy){robot.x + diameter/2, robot.y - diameter/2});
          int i_start = double2grid_x(start.x), i_end = double2grid_x(end.x);
          int j_start = double2grid_y(start.y), j_end = double2grid_y(end.y);
          // PRINT("%d  %d  %d  %d", i_start, i_end, j_start, j_end);

          // 上で算出した矩形範囲の中で、かつ、指定された径の中の領域について、その値をガウス関数(3σ=直径)で算出
          // 最大25回のループ
          for(int i=i_start; i<i_end; i++)
          {
          	for(int j=j_start; j<j_end; j++)
          	{
              double distance = calDistance((DATA_xy){grid2double_x(i), grid2double_y(j)}, robot);
              if(distance < diameter/2)
              {
                // double delta = - 2.0 * exp(-pow(distance/1000, 2.0)/0.2);
                double delta = - decrement/(2*PI*pow(sigma,2.0)) * 30*60/100 * exp(-pow(distance/1000,2.0)/(2*pow(sigma, 2.0)));
                if((*unknown_area_in)[i][j] + delta < 0) delta = - (*unknown_area_in)[i][j];
                delta_sum += delta;
              }
            }
          }

          return delta_sum;
        }

        // 環境の未知度：グリッド更新
        void calcParmClass::NextExplorePerPlace2(DATA_xy robot, std::vector<std::vector<double>>* unknown_area_in)
        {
          for(int i=0; i<gridnum1; i++)
          {
            for(int j=0; j<gridnum2; j++)
            {
              // 加算
              (*unknown_area_in)[i][j] += increment * (1 - sensitivity/100);
              if((*unknown_area_in)[i][j] > 100*10) (*unknown_area_in)[i][j] = 100*10;
              // 減算
              double distance = calDistance((DATA_xy){grid2double_x(i), grid2double_y(j)}, robot);
              if(distance < diameter/2)
              {
                // (*unknown_area_in)[i][j] -= 2.0 * exp(-pow(distance/1000, 2.0)/0.2);
                (*unknown_area_in)[i][j] -= decrement/(2*PI*pow(sigma,2.0)) * 30*60/100 * exp(-pow(distance/1000,2.0)/(2*pow(sigma, 2.0)));
            		if((*unknown_area_in)[i][j] < 0) (*unknown_area_in)[i][j] = 0;
              }
            }
          }
        }

        // 環境の未知度：算出
        double calcParmClass::NextExplorePlace2(std::vector<std::vector<double>>* unknown_area)
        {
        	double explore = 0;
        	double calc_result = 0;

        	for(int i=0; i<gridnum1; i++)
        	{
        		for(int j=0; j<gridnum2; j++)
        		{
        			explore += (*unknown_area)[i][j];
        		}
        	}
        	calc_result = explore / (60 * 30);

        	if(calc_result > 100)
        	{
        		calc_result = 100;
        	}

        	return calc_result;
        }


// double calcParmClass::expectedDelta(char str[], DATA_xy goal, std::vector<std::vector<double>> *unknown_area_in)
// {
//   double dMiss = adjust_delta(miss, this->dMiss(goal, true));
//   double dAnxiety = adjust_delta(anxiety, this->dAnxiety(goal));
//   double dExploreHuman = adjust_delta(explore_human, this->dExploreHuman(goal));
//   double dExplorePlace = this->dExplorePlace2(goal, unknown_area_in);//adjust_delta(explore_place, this->dExplorePlace2(goal, unknown_area_in));
//   // double dExplorePlace = adjust_delta(explore_place, this->dExplorePlace2(goal, unknown_area_in))/abs(this->dExplorePlace_MAX);
//
//   double dSum = dMiss + dAnxiety + dExploreHuman + dExplorePlace;
//
//   if(goal.x<-2900 && goal.y>2900)
//   {
//     PRINT("(%lf, %lf): %10.7lf = %+10.7lf %+10.7lf %+10.7lf %+10.7lf",
//           goal.x, goal.y, dSum, dMiss, dAnxiety, dExploreHuman, dExplorePlace);
//     // PRINT("%lf", dExplorePlace);
//   }
//   // return dSum;
//   return dExplorePlace;
// }
//
// double calcParmClass::expectedDelta_score(char str[], DATA_xy goal, std::vector<std::vector<double>> *unknown_area_in)
// {
//   double dMiss_score = Miss_a * adjust_delta(miss, this->dMiss(goal, true));
//   double dAnxiety_score = Anxiety_a * adjust_delta(anxiety, this->dAnxiety(goal));
//   double dExploreHuman_score = Explore_a * adjust_delta(explore_human, this->dExploreHuman(goal));
//   double dExplorePlace_score = explore_place * adjust_delta(explore_place, this->dExplorePlace2(goal, unknown_area_in));
//
//   double dSum_score = dMiss_score + dAnxiety_score + dExploreHuman_score + dExplorePlace_score;
//
//   if(goal.x<-2900 && goal.y>2900)
//   {
//     // PRINT("(%lf, %lf): %10.7lf \n   = %+10.7lf %+10.7lf %+10.7lf %+10.7lf",
//     //       goal.x, goal.y, str, dSum_score, dMiss_score, dAnxiety_score, dExploreHuman_score, dExplorePlace_score);
//   }
//   return dSum_score;
//   // return dExplorePlace_score;
// }

void calcParmClass::set_tmp_parm(double Miss_a_in, double Anxiety_a_in, double Explore_a_in)
{
  Miss_a = Miss_a_in;
  Anxiety_a = Anxiety_a_in;
  Explore_a = Explore_a_in;
}
