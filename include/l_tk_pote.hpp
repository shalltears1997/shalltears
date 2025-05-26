#pragma once

#include "define.hpp"
#include "tk_data_struct.hpp"

#define gridnum1 60
#define gridnum2 30

class PotentialFunction
{
private:
	// 内部メソッド（ポテンシャルを計算するために用いるライブラリ）
	double calBarrier(double x, double y, double approach_distance);
	double attachment2distance(int attachment);
	double calSlope(double x, double y, DATA_xy origin);
	double calMountain(double x, double y, DATA_xy origin, double approach_distance);
	double calObstacle(double x, double y, DATA_obstacle obstacle);

	// 外側領域
	// この領域の外には、ロボットが来ることを一切想定しない
	// この領域の中には、何らかの事故でロボットが飛び出てしまうことを想定している
	// この枠上に反比例関数の中心をおいて、壁に寄らないバリアとする
	//
	// バリアの傾きは、通常の部屋の領域の枠上においても、人を中心とするポテンシャルのほとんどの強度に対応できる（バリアの方が勝ち、領域の外には出ない）
	// ただし、人もその中心では無限大に発散するため、万が一ではあるが、部屋の領域から出てしまうことも考えられる
	// そのとき、部屋の領域の外を一律に無限大にしていると、一度部屋の領域から出てしまったロボットはポテンシャルの勾配を得られず、経路を引けない
	int external_region_xXyY[4] = {-3500, 3500, -500, 3500};

protected:
	// これら変数をセットすることがPotentialGeneratorの役目
	// PotentialFunctionには、これら値をセットするメソッドがない（し、非公開変数なのでいじりようがない）
	bool condition;  // trueのとき、ゴールも計算する
	DATA_xy p_targetpose;
	DATA_obstacle *p_obstacle;
	DATA_xy po_owner, po_stranger;
	double pa_owner, pa_stranger;

public:
	// 値を指定し、直接PotentialFunctionのインスタンスを生成することはできない

	// コンストラクタに引数はない
	PotentialFunction(){};
	// これらが「関数」で、値を出力するだけ
	double potential(double x, double y);
	double potential_grid(int i, int j);
};

class PotentialGenerator : public PotentialFunction
{
public:
	// コンストラクタから「ジェネレータ」を呼び、関数を作ることができる
	PotentialGenerator();
	PotentialGenerator(std::vector<DATA_human> human, PARAMETER human_p[], DATA_obstacle obstacle[]);
	PotentialGenerator(DATA_xy targetpose, std::vector<DATA_human> human, PARAMETER human_p[], DATA_obstacle obstacle[]);

	// 関数を作る「ジェネレータ」：正確には、privateである親クラスの変数に値をセットする（引数の多少によってゴールを使うか使わないかが別れる）
	void setParm(std::vector<DATA_human> human, PARAMETER human_p[], DATA_obstacle obstacle[]);
	void setParm(DATA_xy targetpose, std::vector<DATA_human> human, PARAMETER human_p[], DATA_obstacle obstacle[]);

	// グリッドを作る「ジェネレータ」
	std::vector<std::vector<double>> getPoteGridMap();

	// 最小値・最小値群を作る「ジェネレータ」
	DATA_grid_part getlowestGrid();
	std::vector<DATA_grid_part> getLowGrids();

};
