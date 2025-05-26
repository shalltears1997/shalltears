#include "tk51_SubGoalGenerator.hpp"

// コンストラクタ
SubGoalGenerator::SubGoalGenerator(std::vector<DATA_xy> p_route_point_in)
{
  p_route_point = p_route_point_in;
};

// 経路からサブゴール（次にロボットがいるべき位置）を算出
DATA_xy SubGoalGenerator::generateSubGoal(double speed_in)
{
	DATA_xy p_sub_goal;

	// デバッグ：上で算出された経路を逐一たどる（ロボットから一番近い経路上の点を辿っていく）
	// ロボットから一番近い経路上の点は、p_route_pointの要素1（要素0はロボットの現在位置）
	// p_sub_goal = p_route_point[1];
	// pioneer.setPosition(p_sub_goal.x, p_sub_goal.y);

	double ideal_distance = speed_in / 0.915;  // 速度のための「距離」

  if(ideal_distance == PATH_NEXT_RESO * tki_MAX)
	{
		// ここに来るようにPATH_NEXT_RESOとtki_MAXを設定しておけば、計算コストは最も低くなる
		// が、実際にそうすることは現実的ではない（のにこれを書いたのは、理屈の説明を兼ねてのことで、大切なのは下2つ）
		p_sub_goal.x = p_route_point[tki_MAX-1].x;
		p_sub_goal.y = p_route_point[tki_MAX-1].y;
	}
	// else if(ideal_distance <= p_tki_length * PATH_NEXT_RESO)
  // 下の条件が < ではなく <= だと、p_route_point.size()==1であり、のちに角度を計算するときにnanとなる
	else if(ideal_distance < (p_route_point.size() - 1) * PATH_NEXT_RESO)
	{
		// 計算した経路が ideal_distance より長い場合
		// 準備1（ideal_distance に最近傍の p_route_point[] 中の点）
		int tki2 = ideal_distance/PATH_NEXT_RESO;
		// 準備2（不足分）
		double shortage_distance = fmod(ideal_distance, PATH_NEXT_RESO);	// fmod:剰余
		double interval_angle = calDegree(p_route_point[tki2], p_route_point[tki2+1]);
		// サブゴール =（ideal_distance に最近傍の p_route_point[] 中の点）+（不足分）
		p_sub_goal.x = p_route_point[tki2].x + shortage_distance * cos(interval_angle*PI/180.0);
		p_sub_goal.y = p_route_point[tki2].y + shortage_distance * sin(interval_angle*PI/180.0);
	}
	else
	{
		// 計算した経路が ideal_distance より短い場合、その場所がゴール
		p_sub_goal.x = p_route_point[p_route_point.size()-1].x;
		p_sub_goal.y = p_route_point[p_route_point.size()-1].y;
	}

	return p_sub_goal;
}
