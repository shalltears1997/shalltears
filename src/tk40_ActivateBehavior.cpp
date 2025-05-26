#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>

#include "gv_extern.hpp"
#include "tk32_BasicBehaviors.hpp"
#include "tk33_BallPlayBehavior.hpp"
#include "tk30_ActivateBehavior.hpp"

#include "l_tk-library.hpp"

Component_ActivateBehavior::Component_ActivateBehavior(DataClass *data_in, DataClass *data_pre_in)
{
	data = data_in;
	data_pre = data_pre_in;

	// 初期値として仮に生成しておく
	behavior_p = new Home_BC(data);
}

void Component_ActivateBehavior::RE_ConstructBehavior()
{
	PRINT("RE-constructing the behavior: old %d to %d\n", data_pre->motion_id, data->motion_id);

	// まず ↓ このポインタが指す「ヒープ領域のある特定の場所」にある実態を削除する
	// 　　 ↓                 ^^^^^^^^^^
	delete behavior_p;  // -> 本来はプログラム終了間際にも実行したいが、glの関係があってできない・・？

	// 次に、新たにこのポインタが指す「ヒープ領域の特定の場所」に新しい実態を生成する

	if(random_model && data->motion_id != ATTENTION_GETTING && data->motion_id != SHOWING_DIRECTION && data->motion_id != GAZE_ALTERNATION)
	{
		if(data->motion_id == HOME)
		{
				behavior_p = new Home_BC(data);
		}
		else
		{
			behavior_p = new Exploring_BC(data);
		}
	}
	else
	{
		switch(data->motion_id)
		{
			case HOME:
				behavior_p = new Home_BC(data);
				break;

			// case SOUND_GAZE:
				// 塚田、2016/06/18、この処理はActivateBehaviorで実行（本来は反射スレッドで行うべき）
				// hark_gaze：このflagの管理は、falseにする作業も、受信のところで行う
				// sound_gaze_be();    動作を上書きする仕組みを作れば良い
				// break;

			// case SOUND_MOVE:
			// 	behavior_p = new SoundMove_BC(data);
			// 	break;
	    //
			// case PLAY_WITH_OWNER:
			// case PLAY_WITH_STRANGER:
			// 	behavior_p = new Play_BC(data);
			// 	break;

			// case LEADING_BEHAVIOR:
			// case ATTENTION_GETTING:
			// case DIRECTIONAL_SIGNAL:
			// 	behavior_p = new LeadingBehavior_BC(data);
			// 	break;
			case MONITORING_OBJECT:
				behavior_p = new Monitoring_Obj_BC(data);
				break;
			case MONITORING_AREA:
				behavior_p = new Monitoring_Area_BC(data);
				break;
			case MONITORING_USER:
				behavior_p = new Monitoring_User_BC(data);
				break;
			case NOTIFY:
				behavior_p = new Notify_BC(data);
				break;
			case ROTATE:
				behavior_p = new Rotate_BC(data);
				break;
			case ATTENTION_GETTING:
				behavior_p = new AttentionGetting_BC(data);
				break;
			case SHOWING_DIRECTION:
				behavior_p = new ShowingDirection_BC(data);
		        break;
			case GAZE_ALTERNATION:
				behavior_p = new GazeAlternation_BC(data);
				break;
			case GO_TO_SITTING_OWNER:
				behavior_p = new GoToOwner_BC(data);
				break;

			case PLAY_USING_BALL:
				behavior_p = new BallPlay_BC(data);
				break;

			case MISSING:
			case GO_TO_OWNER:
			case EXPLORE_STRANGER:
			case EXPLORE:
			case GO_TO_DOOR:
				behavior_p = new AttachmentBehavior_BC(data);
				break;

			// case GO_TO_DOOR:
			// 	behavior_p = new GoToDoor_BC(data);
			// 	break;

			default:
				behavior_p = new Halt_BC(data);
				break;
		}
	}
}

void Component_ActivateBehavior::act()
{
	// インスタンス更新が必要なときは、従来のインスタンスを破棄し、新たにコンストラクタを呼ぶ
	if(data->motion_id != data_pre->motion_id) RE_ConstructBehavior();

	// ふるまいインスタンスを実行
	// behavior_p はふるまいの基底クラスで、実態は異なる：ポリモーフィズム
	data->command = behavior_p->act();
}


// インスタンス更新（従来のものを削除し、新たなものを生成）
// std::cout << behavior_p;// << std::endl;
// if(data->motion_id == MISSING)
// {
// 	delete behavior_p;
// 	behavior_p = new BC_Missing();
// }
// else
// {
// 	delete behavior_p;
// 	behavior_p = new NONMissing_BC(data);
// }


// 4. 経路算出
// if(!stop_flag2)
// {
// 	// 経路生成に用いるポテンシャル生成（目的地あり）
// 	PotentialGenerator tmp({data.po_human[0].posex, data.po_human[0].posey}, data.po_human, data.pa_human, data.obstacle);
// 	data.pf_goal = tmp;
// 	debug_data.low_grids = tmp.getLowGrids();
//
// 	// 経路生成
// 	PathGenerator path(data.pf_goal, debug_data.low_grids, pioneer.getCurrnetPosition());
// 	data.command.robot.path = path.generate_path();
// }
