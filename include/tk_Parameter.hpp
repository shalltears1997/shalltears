#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>

#include "gv_extern.hpp"
#include "l_tk-library.hpp"

class calcParmClass
{
private:

  PositionClass owner, stranger;
  double miss, anxiety, explore_human, explore_place;
  double attachment, acceptance, sensitivity;

  // 仮処置（本来は別のクラスにおく）
	double Miss_a, Anxiety_a, Explore_a;

  // ロボットが環境の未知度を下げる範囲の直径
  double diameter = 1500;
  // はじめに、領域の中に、プラスしたら100を超えるものがないかを走査する。その、オーバー分の合計
  double surplus_sum = 0;
  // double increment = 0.02;
  // double decrement = 0.06;
  // double increment = 0.05;
  double increment = 0.15;
  double decrement = 0.5;
  double sigma = 0.3;
  // double sigma = 1.5/2.0/3.0;
  // double sigma = (1.5/2.0)/3.0;
  // double sigma = 0.1;
  // 環境の未知度を上げるときに使う
  std::vector<std::vector<double>> add_value;
  // 変化量そのものではなく、変化量のとりうる最大値と予想される変化量の比を見るようにしたい。そのために、最大値をここにいれる
  double dExplorePlace_MAX = 0;

public:

    // 値が0〜100になるように変化量を調整
    double adjust_delta(double value, double delta);
    // 値が0〜100になるように絶対値を調整
    double RefreshValue(double value, double delta);



  calcParmClass(DataClass data);

  void set_tmp_parm(double Miss_a_in, double Anxiety_in, double Explore_a_in);

  double approachDistance(double ata);
  double approachDistance_inverse(double ata);
  double distanceConverter(double x, double ata);
  double distanceConverter_inverse(double x, double ata);

  // パラメータ変化量
  double dMiss(DATA_xy robot);
  double dMiss_Door(DATA_xy robot, PositionClass human);
  double dAnxiety_Coming(DATA_xy robot, PositionClass human);
  //
  double dMissP(DATA_xy robot, PositionClass human, double ata, bool prediction);
  double dMissM(DATA_xy robot, PositionClass human, double ata, bool prediction);
  double dAnxietyP(DATA_xy robot, PositionClass human, double ata);
  double dAnxietyM(DATA_xy robot, PositionClass human, double ata);
  double dExploreP(DATA_xy robot, PositionClass human, double ata);
  double dExploreM(DATA_xy robot, PositionClass human, double ata);
  //
  double dExplorePlace(DATA_xy robot);

  // ふるまい選択から呼び出す
  double expectedDelta(char str[], DATA_xy goal, std::vector<std::vector<double>> *unknown_area_in);
  double expectedDelta_score(char str[], DATA_xy goal, std::vector<std::vector<double>> *unknown_area_in);

  // パラメータ更新
  void NextExplorePerPlace2(DATA_xy robot, std::vector<std::vector<double>>* tmp_unknown_area);
  double NextExplorePlace2(std::vector<std::vector<double>>* unknown_area);




  // 予測に使う
  // はじめに一回だけ行う
  void dExplorePlace2_surplus_sum(std::vector<std::vector<double>>* unknown_area_in);
  // 目的地候補ごとに行う
  double dExplorePlace2(DATA_xy robot, std::vector<std::vector<double>> *unknown_area_in);
  double NextExplorePerPlace2_decrement(DATA_xy robot, std::vector<std::vector<double>>* tmp_unknown_area);





  // むかしの
  std::vector<std::vector<double>> NextExplorePerPlace(DATA_xy robot);  // 領域のマスの値を更新
  double NextExplorePlace(std::vector<std::vector<double>> unknown_area);

};
