#include <iostream>
#include <cstdio>
#include <ctime>
#include <cmath>
#include "opengl.hpp"

#include "define.hpp"
#include "class.hpp"
#include "gv_extern_id.hpp"
#include "gv_extern.hpp"
// GLDataClass GLData;

#include "c_struct_position.hpp"


void threads_finish()
{
	// 別スレッドに処理終了を伝える
	GoOnFlag = false;

	// スレッドの数
	// std::cout << threads.size() << std::endl;

	// スレッド終了処理
	for (std::thread& tmp : threads)
	{
	tmp.join();
	}

	// クラス内に実装されているスレッドの終了
	// （デストラクタにも記述してあるので、なくても大丈夫ではある）
	connectWithZPS.receiveLoopThreadFinish();
	connectWithMC.connectLoopThreadFinish();
	RobotSimulator.moveLoopThreadFinish();
	Debug.ThreadFinish();

	std::cout << "threads are all killed." << std::endl;
}


void myKeyboard(unsigned char key, int x, int y)
{
	if(key == 27)  // Escキー
	{
		// スレッドに終了フラグを伝えて、ちゃんと終了する
		threads_finish();

		std::cout << "All processes except openGL have successfully finished their own task." << std::endl << std::endl;

		// プログラム終了（OpenGLを終了するには、こうするしかない）
		exit(0);
	}
	if(key == 'd')
	{
		if(GLData.display_switch == 1) GLData.display_switch = 0;
		else GLData.display_switch = 1;
	}
	else if(key == 'e')
	{
		if(GLData.data.nazo.expflag == 0) GLData.data.nazo.expflag = 1;
		else GLData.data.nazo.expflag = 0;
	}
	else if(key == 'h')
	{
		if(!GLData.data.home) GLData.data.home = true;
		else GLData.data.home = false;
	}
	else if(key == 's')
	{
		if(!GLData.data.pioneer_activate) GLData.data.pioneer_activate = true;
		else GLData.data.pioneer_activate = false;
	}
	else if(key == '0')
	{
		if(GLData.data.human[0].height > 1000) GLData.data.human[0].height = 500;
		else GLData.data.human[0].height = 1500;
	}
	else if(key == '1')
	{
		if(GLData.data.human[1].height > 1000) GLData.data.human[1].height = 500;
		else GLData.data.human[1].height = 1500;
	}
	else if(key == 'b')
	{
		if(GLData.distype > 4) GLData.distype = 0;
		else GLData.distype++;
	}
	else if(key == ']')
	{
		if (GLData.data.GoToHomeFlag) GLData.data.GoToHomeFlag = false;
		else GLData.data.GoToHomeFlag = true;
	}
	else if(key == 'p' || key == 'P')
	{
		GLData.data.plot_type++;
		if(GLData.data.plot_type > 2) GLData.data.plot_type = 0;
	}
}


void myMouseFunc(int button, int state, int xx, int yy)
{
	double drawing_region_x_min = -3000;
	double drawing_region_x_max = 3000;

	double drawing_region_y_min = -850;
	double drawing_region_y_max = -250;

	double parameter_width = drawing_region_x_max - drawing_region_x_min;
	double parameter_height = drawing_region_y_max - drawing_region_y_min;

	double segment_n = 2;

	double segment_width = parameter_width*12.0/100.0;
	double segment_height = parameter_height/segment_n;

	double movable_region_x_min = drawing_region_x_min + segment_width/2.0;
	double movable_region_x_max = drawing_region_x_max - segment_width/2.0;
	double movable_region_width = movable_region_x_max - movable_region_x_min;
	double movable_region_center = movable_region_width/2 + movable_region_x_min;

	double Ro = (GLData.data.pa_human[0].ata - 50.0)/50.0;
	double Rs = (GLData.data.pa_human[1].ata - 50.0)/50.0;

	double segment_center_o = Ro * movable_region_width/2.0 + movable_region_center;
	double segment_center_s = Rs * movable_region_width/2.0 + movable_region_center;


	//Window coordinate -> GL coordinate

	// xx, yy はディスプレイの座標（左上が0, 0)
	double mouse_x = (double)xx * (data.gl_x1 - data.gl_x0) / data.gl_w + data.gl_x0;
	double mouse_y = (double)(data.gl_h-yy) * (data.gl_y1 - data.gl_y0) / data.gl_h + data.gl_y0;

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// 人のドーナッツ
		if(GLData.data.human[0].posi.x - 90 < mouse_x && mouse_x < GLData.data.human[0].posi.x + 90 && GLData.data.human[0].posi.y - 90 < mouse_y && mouse_y < GLData.data.human[0].posi.y + 90){
			GLData.drag_flag = 0;
			GLData.mouseFlag = GL_TRUE;
		}else if(GLData.data.human[1].posi.x - 90 < mouse_x && mouse_x < GLData.data.human[1].posi.x + 90 && GLData.data.human[1].posi.y -90 < mouse_y && mouse_y < GLData.data.human[1].posi.y + 90){
			GLData.drag_flag = 1;
			GLData.mouseFlag = GL_TRUE;
		}else if(GLData.data.human[2].posi.x - 90 < mouse_x && mouse_x < GLData.data.human[2].posi.x + 90 && GLData.data.human[2].posi.y -90 < mouse_y && mouse_y <GLData.data.human[2].posi.y + 90){
			GLData.drag_flag = 2;
			GLData.mouseFlag = GL_TRUE;
		}else if(GLData.data.ball_tk.x - 90 < mouse_x && mouse_x < GLData.data.ball_tk.x + 90 && GLData.data.ball_tk.y -90 < mouse_y && mouse_y <GLData.data.ball_tk.y + 90){
			GLData.drag_flag = 5;
			GLData.mouseFlag = GL_TRUE;
		}else if(GLData.data.object[1].posi.x - 90 < mouse_x && mouse_x < GLData.data.object[1].posi.x + 90 && GLData.data.object[1].posi.y -90 < mouse_y && mouse_y <GLData.data.object[1].posi.y + 90){
			GLData.drag_flag = 6;
			GLData.mouseFlag = GL_TRUE;
		}else if(GLData.data.potedama.posi.x - 90 < mouse_x && mouse_x < GLData.data.potedama.posi.x + 90 && GLData.data.potedama.posi.y -90 < mouse_y && mouse_y <GLData.data.potedama.posi.y + 90){
			GLData.drag_flag = 7;
			GLData.mouseFlag = GL_TRUE;
		}

		// roped-off area
		else if(NoEnter[1] - 30 < mouse_x && mouse_x < NoEnter[1] + 30 &&
			NoEnter[3] < mouse_y && mouse_y < NoEnter[2]){					//右側面の設定
			GLData.drag_flag = 10;
			GLData.mouseFlag = GL_TRUE;
		}else if(NoEnter[0] - 30 < mouse_x && mouse_x < NoEnter[0] + 30 &&
			NoEnter[3] < mouse_y && mouse_y < NoEnter[2]){					//左側面の設定
			GLData.drag_flag = 11;
			GLData.mouseFlag = GL_TRUE;
		}else if( NoEnter[0] < mouse_x && mouse_x < NoEnter[1] &&
			NoEnter[2] - 30 < mouse_y && mouse_y < NoEnter[2] + 30){			//上辺部の設定
			GLData.drag_flag = 12;
			GLData.mouseFlag = GL_TRUE;
		}else if(NoEnter[0] < mouse_y && mouse_x < NoEnter[1] &&
			NoEnter[3] - 30 < mouse_y && mouse_y < NoEnter[3] + 30 ){			//底辺部の設定
			GLData.drag_flag = 13;
			GLData.mouseFlag = GL_TRUE;
		}

		// area restricted lying
		else if(NoSleep[1] - 30 < mouse_x && mouse_x < NoSleep[1] + 30 &&
			NoSleep[3] < mouse_y && mouse_y < NoSleep[2]){					//右側面の設定
			GLData.drag_flag = 20;
			GLData.mouseFlag = GL_TRUE;
		}else if(NoSleep[0] - 30 < mouse_x && mouse_x < NoSleep[0] + 30 &&
			NoSleep[3] < mouse_y && mouse_y < NoSleep[2]){					//左側面の設定
			GLData.drag_flag = 21;
			GLData.mouseFlag = GL_TRUE;
		}else if( NoSleep[0] < mouse_x && mouse_x < NoSleep[1] &&
			NoSleep[2] - 30 < mouse_y && mouse_y < NoSleep[2] + 30){			//上辺部の設定
			GLData.drag_flag = 22;
			GLData.mouseFlag = GL_TRUE;
		}else if( NoSleep[0] < mouse_x && mouse_x < NoSleep[1] &&
			NoSleep[3] - 30 < mouse_y && mouse_y < NoSleep[3] + 30){			//底辺部の設定
			GLData.drag_flag = 23;
			GLData.mouseFlag = GL_TRUE;
		}



		// 信頼度
		else if(segment_center_o - segment_width/2 < mouse_x && mouse_x < segment_center_o + segment_width/2
			&& drawing_region_y_max - segment_height*(1+0) < mouse_y && mouse_y <  drawing_region_y_max - segment_height*0)
		{
			GLData.drag_flag = 30;
			GLData.mouseFlag = GL_TRUE;
		}
		else if(segment_center_s - segment_width/2 < mouse_x && mouse_x < segment_center_s + segment_width/2
			&& drawing_region_y_max - segment_height*(1+1) < mouse_y && mouse_y <  drawing_region_y_max - segment_height*1)
		{
			GLData.drag_flag = 31;
			GLData.mouseFlag = GL_TRUE;
		}

	}
	else if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN && GLData.mouseFlagR == false)
	{
		if(GLData.data.human[0].posi.x - 90 <  mouse_x && mouse_x < GLData.data.human[0].posi.x + 90 && GLData.data.human[0].posi.y - 90 < mouse_y && mouse_y < GLData.data.human[0].posi.y + 90)
		{
			GLData.drag_flag = 1000;
			GLData.mouseFlag = GL_TRUE;
			GLData.mouseFlagR = true;
			//cout<<GLData.drag_flag<<"mousefunction"<<endl;
		}
	}
	else
	{
		GLData.mouseFlag = GL_FALSE;
	}
}

// void myMouseFunc(int button, int state, int xx, int yy)
// {
// 	int parameter_x_min = -2000;
// 	int parameter_x_max = 3000;
// 	int parameter_y_min = -850;
// 	int parameter_y_max = -250;
// 	int parameter_width = parameter_x_max - parameter_x_min;
// 	int parameter_height = parameter_y_max - parameter_y_min;
//
// 	double ato = (parameter_width / 100) * GLData.data.pa_human[0].ata + parameter_x_min;
// 	double aos = (parameter_width / 100) * GLData.data.pa_human[1].ata + parameter_x_min;
// 	double aou = (parameter_width / 100) * GLData.data.pa_human[2].ata + parameter_x_min;
// 	double z = (parameter_width / 2000) * GLData.data.human[0].height + parameter_x_min;
// 	double soa = (parameter_width / 100) * GLData.data.sensitivity_of_anxiety + parameter_x_min;
//
// 	//Window coordinate -> GL coordinate
//
// 	// xx, yy はディスプレイの座標（左上が0, 0)
// 	// double mouse_x = (double)( xx - 509 )*( 5000 - (-3300) ) / 1280.0;
// 	// double mouse_y = (double)( 538 - yy )*( 4000 - (-1500) ) / 740.0;
// 	double mouse_x = (double)xx * (data.gl_x1 - data.gl_x0) / data.gl_w + data.gl_x0;
// 	double mouse_y = (double)(data.gl_h-yy) * (data.gl_y1 - data.gl_y0) / data.gl_h + data.gl_y0;
// 	// PRINT("%d  %d | %lf  %lf | %lf  %lf\n", xx, yy, mouse_x, mouse_y, data.gl_w, data.gl_h);
//
// 	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
// 	{
// 		// person position setting
// 		if(GLData.data.human[0].posi.x - 90 < mouse_x && mouse_x < GLData.data.human[0].posi.x + 90 && GLData.data.human[0].posi.y - 90 < mouse_y && mouse_y < GLData.data.human[0].posi.y + 90){
// 			GLData.drag_flag = 0;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if(GLData.data.human[1].posi.x - 90 < mouse_x && mouse_x < GLData.data.human[1].posi.x + 90 && GLData.data.human[1].posi.y -90 < mouse_y && mouse_y < GLData.data.human[1].posi.y + 90){
// 			GLData.drag_flag = 1;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if(GLData.data.human[2].posi.x - 90 < mouse_x && mouse_x < GLData.data.human[2].posi.x + 90 && GLData.data.human[2].posi.y -90 < mouse_y && mouse_y <GLData.data.human[2].posi.y + 90){
// 			GLData.drag_flag = 2;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if(GLData.data.ball_tk.x - 90 < mouse_x && mouse_x < GLData.data.ball_tk.x + 90 && GLData.data.ball_tk.y -90 < mouse_y && mouse_y <GLData.data.ball_tk.y + 90){
// 			GLData.drag_flag = 5;
// 			GLData.mouseFlag = GL_TRUE;
// 		}
// 		// roped-off area
// 		else if(NoEnter[1] - 30 < mouse_x && mouse_x < NoEnter[1] + 30 &&
// 			NoEnter[3] < mouse_y && mouse_y < NoEnter[2]){					//右側面の設定
// 			GLData.drag_flag = 10;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if(NoEnter[0] - 30 < mouse_x && mouse_x < NoEnter[0] + 30 &&
// 			NoEnter[3] < mouse_y && mouse_y < NoEnter[2]){					//左側面の設定
// 			GLData.drag_flag = 11;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if( NoEnter[0] < mouse_x && mouse_x < NoEnter[1] &&
// 			NoEnter[2] - 30 < mouse_y && mouse_y < NoEnter[2] + 30){			//上辺部の設定
// 			GLData.drag_flag = 12;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if(NoEnter[0] < mouse_y && mouse_x < NoEnter[1] &&
// 			NoEnter[3] - 30 < mouse_y && mouse_y < NoEnter[3] + 30 ){			//底辺部の設定
// 			GLData.drag_flag = 13;
// 			GLData.mouseFlag = GL_TRUE;
// 		}
//
// 		// area restricted lying
// 		else if(NoSleep[1] - 30 < mouse_x && mouse_x < NoSleep[1] + 30 &&
// 			NoSleep[3] < mouse_y && mouse_y < NoSleep[2]){					//右側面の設定
// 			GLData.drag_flag = 20;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if(NoSleep[0] - 30 < mouse_x && mouse_x < NoSleep[0] + 30 &&
// 			NoSleep[3] < mouse_y && mouse_y < NoSleep[2]){					//左側面の設定
// 			GLData.drag_flag = 21;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if( NoSleep[0] < mouse_x && mouse_x < NoSleep[1] &&
// 			NoSleep[2] - 30 < mouse_y && mouse_y < NoSleep[2] + 30){			//上辺部の設定
// 			GLData.drag_flag = 22;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if( NoSleep[0] < mouse_x && mouse_x < NoSleep[1] &&
// 			NoSleep[3] - 30 < mouse_y && mouse_y < NoSleep[3] + 30){			//底辺部の設定
// 			GLData.drag_flag = 23;
// 			GLData.mouseFlag = GL_TRUE;
// 		}
//
// 		// behavioral characteristic parameter
// 		else if(ato -100< mouse_x && mouse_x < ato+100 && parameter_y_min + parameter_height*3/4 < mouse_y && mouse_y < parameter_y_min + parameter_height*4/4){	//attachment to owner
// 			GLData.drag_flag = 30;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if(aos-100 < mouse_x && mouse_x < aos+100 && parameter_y_min + parameter_height*2/4 < mouse_y && mouse_y < parameter_y_min + parameter_height*3/4){	//acceptance of stranger
// 			GLData.drag_flag = 31;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if(aou - 100 < mouse_x && mouse_x < aou + 100 && parameter_y_min + parameter_height*1/4 < mouse_y && mouse_y < parameter_y_min + parameter_height*2/4){
// 			GLData.drag_flag = 32;
// 			GLData.mouseFlag = GL_TRUE;
// 		}else if(soa-100 < mouse_x && mouse_x < soa+100 && parameter_y_min + parameter_height*0/4 < mouse_y && mouse_y < parameter_y_min + parameter_height*1/4){	//sensitivity of anxiety
// 			GLData.drag_flag = 33;
// 			GLData.mouseFlag = GL_TRUE;
// 		}
// 	}
// 	else if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN && GLData.mouseFlagR == false){
// 		if(GLData.data.human[0].posi.x - 90 <  mouse_x && mouse_x < GLData.data.human[0].posi.x + 90 && GLData.data.human[0].posi.y - 90 < mouse_y && mouse_y < GLData.data.human[0].posi.y + 90){
// 			GLData.drag_flag = 1000;
// 			GLData.mouseFlag = GL_TRUE;
// 			GLData.mouseFlagR = true;
// 			//cout<<GLData.drag_flag<<"mousefunction"<<endl;
// 		}
// 	}
// 	else
// 	{
// 		GLData.mouseFlag = GL_FALSE;
// 	}
// }

void myMouseMotion(int xx , int yy)
{
	if (GLData.mouseFlag == GL_FALSE) return;


		double drawing_region_x_min = -3000;
		double drawing_region_x_max = 3000;
		double parameter_width = drawing_region_x_max - drawing_region_x_min;

		double segment_width = parameter_width*12.0/100.0;
		double movable_region_x_min = drawing_region_x_min + segment_width/2.0;
		double movable_region_x_max = drawing_region_x_max - segment_width/2.0;
		double movable_region_width = movable_region_x_max - movable_region_x_min;


	//Window coordinate -> GL coordinate
	// mouse_x = (double)( xx - 509 )*( 5000.0 - (-3300.0) ) / (1280);
	// mouse_y = (double)( 538 - yy )*( 4000.0 - (-1500.0) ) / (740);
		double mouse_x = (double)xx * (data.gl_x1 - data.gl_x0) / data.gl_w + data.gl_x0;
		double mouse_y = (double)(data.gl_h-yy) * (data.gl_y1 - data.gl_y0) / data.gl_h + data.gl_y0;
	// PRINT("%7.3lf  %7.3lf | %7.3lf  %7.3lf\n", xx, yy, mouse_x, mouse_y);

	//person position setting
	if(GLData.drag_flag == 0)
	{
    GLData.data.human[0].posi = {mouse_x, mouse_y};
	}
	else if(GLData.drag_flag == 1)
	{
    GLData.data.human[1].posi = {mouse_x, mouse_y};
	}
	else if(GLData.drag_flag == 2)
	{
    GLData.data.human[2].posi = {mouse_x, mouse_y};
	}
	else if(GLData.drag_flag == 3)
	{
		// obs[0].setPosition(mouse_x,mouse_y);
	}
	//ball
	else if(GLData.drag_flag == 5)
	{
		GLData.data.ball_tk = {mouse_x, mouse_y};
	}	
	else if(GLData.drag_flag == 6)
	{
		GLData.data.object[1].posi = {mouse_x, mouse_y};
	}else if(GLData.drag_flag == 7)
	{
		GLData.data.potedama.posi = {mouse_x, mouse_y};
	}
	//roped-off area
	else if(GLData.drag_flag == 10){					//RIGHT side
		NoEnter[1] = mouse_x;
	}else if(GLData.drag_flag == 11){					//LEFT side
		NoEnter[0] = mouse_x;
	}else if(GLData.drag_flag == 12){					//TOP side
		NoEnter[2] = mouse_y;
	}else if(GLData.drag_flag == 13){					//BOTTOM side
		NoEnter[3] = mouse_y;
	}
	//area restricted lying
	else if(GLData.drag_flag == 20){					//RIGHT side
		NoSleep[1] = mouse_x;
	}else if(GLData.drag_flag == 21){					//LEFT side
		NoSleep[0] = mouse_x;
	}else if(GLData.drag_flag == 22){					//TOP side
		NoSleep[2] = mouse_y;
	}else if(GLData.drag_flag == 23){					//BOTTOM side
		NoSleep[3] = mouse_y;
	}
	//Behavioral Characteristic Parameter
	else if(GLData.drag_flag == 30)
	{
	 if(mouse_x < movable_region_x_min) mouse_x = movable_region_x_min;
	 if(mouse_x > movable_region_x_max) mouse_x = movable_region_x_max;
    GLData.data.pa_human[0].ata = (mouse_x - movable_region_x_min) / movable_region_width * 100;
	}
	else if(GLData.drag_flag == 31)
	{
	 if(mouse_x < movable_region_x_min) mouse_x = movable_region_x_min;
	 if(mouse_x > movable_region_x_max) mouse_x = movable_region_x_max;
    GLData.data.pa_human[1].ata = (mouse_x - movable_region_x_min) / movable_region_width * 100;
	}


	// else if(GLData.drag_flag == 30 && movable_region_x_min<= mouse_x && mouse_x <= movable_region_x_max)
	// {
  //   GLData.data.pa_human[0].ata = (mouse_x - movable_region_x_min) / movable_region_width * 100;
	// }
	// else if(GLData.drag_flag == 31 && movable_region_x_min<= mouse_x && mouse_x <= movable_region_x_max)
	// {
  //   GLData.data.pa_human[1].ata = (mouse_x - movable_region_x_min) / movable_region_width * 100;
	// }

	glutPostRedisplay();
}
