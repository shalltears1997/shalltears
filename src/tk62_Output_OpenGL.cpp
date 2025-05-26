#include <iostream>
#include <cstdio>
#include <ctime>
#include <cmath>
#include "opengl.hpp"


// GLの描画保存
#include <opencv2/opencv.hpp>
// #include <opencv/cv.h>
// #include <opencv/highgui.h>
// #include <opencv2/core/core.hpp>


#include "define.hpp"
#include "class.hpp"
#include "gv_extern_id.hpp"
#include "gv_extern.hpp"
#include "tk_data_GL.hpp"
// GLDataClass GLData;

#include "c_struct_position.hpp"
#include "l_tk_pote.hpp"

#include "tk_data_class.hpp"
#include "22_hark.hpp"


////// OpenGL //////

// 変更箇所

// void myDisplay(void)
// - human2の何か -> human0の何か を表示する

// void myKeyboard(unsigned char key, int x, int y)
// - longtermの何かを出力するようにした
// - human1の何か -> human0の何か を変更する

// void myMouseFunc(int button, int state, int xx, int yy)
// ファイルの出力を有効にした

void drawHuman(int i)
{
	glPushMatrix();
		// 色指定
		if(exp_condition)
		{
			if(data.OSinverse==0)
			{
				if(i==0) glColor3d(0, 1, 0);
				else if(i==1) glColor3d(0, 0, 1);
			// }
			// else
			// {
			// 	if(i==0) glColor3d(0, 0, 1);
			// 	else if(i==1) glColor3d(0, 1, 0);
			}
		}
		else
		{
			if(i==0) glColor3d(1, 0, 0);
			else if(i==1) glColor3d(0, 0, 1);
			else glColor3d(0, 1, 0);
		}
		// 現在位置
		glTranslated(data.human[i].posi.x*s, data.human[i].posi.y*s, 0.0);

		// ドーナッツ
		glutSolidTorus(50, 150, 30, 30);

		// 座ってるとき、中身も塗りつぶす
		if(GLData.data.human[i].height < 1000) glutSolidSphere(120, 30, 30);

		// glutSolidSphere(25, 300, 300);
		// 移動ベクトル
		glLineWidth(3.0);
		glBegin(GL_LINES);
			glVertex3d(0 , 0.0 , 0);
			glVertex3d(3*GLData.data.human_vec[i].x, 3*GLData.data.human_vec[i].y, 0);
		glEnd();
	glPopMatrix();
}


void drawGoal(Position current)
{
	glPushMatrix();
		glTranslated(current.posex, current.posey, 0.0);
		glColor3d(1.0, 0.5, 0.3);
		glutSolidTorus(20, 100, 30, 30);
	glPopMatrix();
}

void drawFootMark(Position point, bool lr)
{
	int pad[5] = {};
	if(lr){
		pad[0] = 200;
		pad[1] = 150;
		pad[2] = 100;
		pad[3] = 70;
		pad[4] = 135;
	}
	else{
		pad[0] = -200;
		pad[1] = -150;
		pad[2] = -100;
		pad[3] = -70;
		pad[4] = -135;
	}

	glColor3d(0.0, 0.0, 0.0);
	glPushMatrix();
		glTranslated(point.posex, point.posey, -10.0);
		glRotated(point.deg-90, 0.0, 0.0, 1.0);
		//glScaled(1300, 1000, 1000);
		glPushMatrix();
			glTranslated(pad[0], -280, 0.0);
			//glScaled(1300, 1000, 1000);
			glutSolidSphere(25, 30, 30);
		glPopMatrix();

		glPushMatrix();
			glTranslated(pad[1], -245, 0.0);
			glRotated(30.0, 0.0, 0.0, 1.0);
			//glScaled(1000, 1200, 1000);
			glutSolidSphere(25, 30, 30);
		glPopMatrix();

		glPushMatrix();
			glTranslated(pad[2], -255, 0.0);
			glRotated(15.0, 0.0, 0.0, 1.0);
			//glScaled(1000, 1200, 1000);
			glutSolidSphere(25, 30, 30);
		glPopMatrix();


		glPushMatrix();
			glTranslated(pad[3], -275, 0.0);
			glRotated(-15.0, 0.0, 0.0, 1.0);
			//glScaled(1000, 1200, 1200);
			glutSolidSphere(25, 30, 30);
		glPopMatrix();

		glPushMatrix();
			glTranslated(pad[4], -330, 0.0);
			glRotated(-30.0, 0.0, 0.0, 1.0);
			//glScaled(1000, 1200, 1200);
			glutSolidSphere(45, 30, 30);
		glPopMatrix();

	glPopMatrix();

}

// valueは[-1, 1]で指定
void setColor(double value, double alpha)
{
	value = -value;
	value = (value+1)*240/2;

	double r = (1                 ) * (  0<=value && value<  60)
	         + (1 - (value- 60)/60) * ( 60<=value && value< 120)
				   + (0                 ) * (120<=value && value<=240);
	double g = (0 + (value-  0)/60) * (  0<=value && value<  60)
				   + (1                 ) * ( 60<=value && value< 180)
					 + (1 - (value-180)/60) * (180<=value && value<=240);
	double b = (0                 ) * (  0<=value && value< 120)
				   + (0 + (value-120)/60) * (120<=value && value< 180)
					 + (1                 ) * (180<=value && value<=240);
	// PRINT("value: %lf | rgb: %lf  %lf  %lf  %d, %d, %d", value, r*255.0, g*255.0, b*255.0, (  0<=value && value<  60), ( 60<=value && value< 180), (180<=value && value<=240));
	glColor4d(r, 1, b, alpha);
}


void myDisplay(void)
{
	int i = 0;
	nlab.drawBackground();
	//nlab.draw_room();
	if(GLData.display_switch == 1){
		//nlab.drawNoEnterArea();
		//nlab.drawNoSleepArea();
		// nlab.drawParameter( human_0.getAttachment() , human_1.getAttachment(),/*human_2.getAttachment()*/human_0.getZ(),pioneer.getSens());
		nlab.drawParameter( GLData.data.pa_human[0].ata, GLData.data.pa_human[1].ata, GLData.data.human[0].height, GLData.data.sensitivity_of_anxiety);
		// nlab.drawKnownPlace(pioneer.getNumberOfKnownPlace());
	}
	// nlab.drawTime();
	//nlab.draw_place();


	double min = INFINITY;
	double max = -INFINITY;
	for(int i=0; i < gridnum1; i++)
	{
		for(int j=0; j < gridnum2; j++)
		{
			if(min>GLData.data.stress_parm_grid[i][j]) min = GLData.data.stress_parm_grid[i][j];
			if(max<GLData.data.stress_parm_grid[i][j]) max = GLData.data.stress_parm_grid[i][j];
		}
	}

	double band;
	if(abs(min)>abs(max)) band = min;
	else band = max;




	// ストレスポテンシャルの描画
	double size = WIDTH_ETHO/gridnum1;
	for(int i=0; i < gridnum1; i++)
	{
		for(int j=0; j < gridnum2; j++)
		{

			// パラメータ変化量
			// if(GLData.data.stress_pote[i][j] > 0) glColor4d(255.0/255.0,   0.0/255.0,   0.0/255.0, 0.0005*abs(GLData.data.unknown_area_finer[i][j]));
			// else                                  glColor4d(  0.0/255.0,   0.0/255.0, 255.0/255.0, 0.0005*abs(GLData.data.unknown_area_finer[i][j]));

			if(random_model)
			{
				// ストレスポテンシャル
				if(GLData.data.stress_pote[i][j] > 0) glColor4d(255.0/255.0,   0.0/255.0,   0.0/255.0, 0.05*abs(GLData.data.stress_pote[i][j]));
				if(GLData.data.stress_pote[i][j] < 0) glColor4d(  0.0/255.0,   0.0/255.0, 255.0/255.0, 0.05*abs(GLData.data.stress_pote[i][j]));
			}
			else
			{
				// 移動必要度
				if(data.mode==2)
				{
					if(GLData.data.stress_parm_grid[i][j] > 0) glColor4d(255.0/255.0,   0.0/255.0,   0.0/255.0, 0.005*abs(GLData.data.stress_parm_grid[i][j]));
					else                                       glColor4d(  0.0/255.0,   0.0/255.0, 255.0/255.0, 0.005*abs(GLData.data.stress_parm_grid[i][j]));
				}
				else
				{
					glColor4d(255.0/255.0, 255.0/255.0, 255.0/255.0, 1.0);
				}
			}

			// カラーマップ
			// setColor(GLData.data.stress_parm_grid[i][j]/band, 0.5);
			// PRINT("%lf  %lf  %lf", GLData.data.stress_parm_grid[i][j]/band, GLData.data.stress_parm_grid[i][j], band);

			glBegin(GL_POLYGON);
				glVertex2d(RoomPoint[0] + i*size, RoomPoint[2] - j*size);
				glVertex2d(RoomPoint[0] + (i+1)*size, RoomPoint[2] - j*size);
				glVertex2d(RoomPoint[0] + (i+1)*size, RoomPoint[2] - (j+1)*size);
				glVertex2d(RoomPoint[0] + i*size, RoomPoint[2] - (j+1)*size);
			glEnd();
		}
	}



	// ストレスパラメータ
	// for(int k=0; k<data.stress_min.size(); k++)
	// {
	// 	double size = WIDTH_ETHO/gridnum1;
	// 	glBegin(GL_POLYGON);
	// 		glColor4d(1.0, 1.0, 0.0, 1.0);	// RGBa
	// 		glVertex2d(RoomPoint[0] + data.stress_min[k].grid.i*size, RoomPoint[2] - data.stress_min[k].grid.j*size);
	// 		glVertex2d(RoomPoint[0] + (data.stress_min[k].grid.i + 1)*size, RoomPoint[2] - data.stress_min[k].grid.j*size);
	// 		glVertex2d(RoomPoint[0] + (data.stress_min[k].grid.i + 1)*size, RoomPoint[2] - (data.stress_min[k].grid.j + 1)*size);
	// 		glVertex2d(RoomPoint[0] + data.stress_min[k].grid.i*size, RoomPoint[2] - (data.stress_min[k].grid.j + 1)*size);
	// 	glEnd();
	// }


	nlab.drawGrid(gridnum1, gridnum2);

	// if(GLData.distype == 0){
	// 	nlab.drawParameterBar(MIS,GLData.data.pa_human[0].mis);
	// 	//nlab.drawParameterBar(EXP,pioneer.getValue(O_EXP));
	// 	nlab.drawParameterBar(ANX,GLData.data.pa_human[0].anx);
	// 	nlab.drawParameterBar(OBJ, (float)GLData.distype);
	// }
	// else if(GLData.distype == 1){
	// 	nlab.drawParameterBar(MIS, GLData.data.pa_human[1].mis);
	// 	//nlab.drawParameterBar(EXP,pioneer.getValue(S1_EXP));
	// 	nlab.drawParameterBar(ANX, GLData.data.pa_human[1].anx);
	// 	nlab.drawParameterBar(OBJ, (float)GLData.distype);
	// }
	// else if(GLData.distype == 2){
	// 	nlab.drawParameterBar(MIS,GLData.data.pa_human[2].mis);
	// 	//nlab.drawParameterBar(EXP,pioneer.getValue(S2_EXP));
	// 	nlab.drawParameterBar(ANX,GLData.data.pa_human[2].anx);
	// 	nlab.drawParameterBar(OBJ, (float)GLData.distype);
	// }
	// else if(GLData.distype == 3){
	// 	nlab.drawParameterBar(MIS,pioneer.getValue(B_MIS));
	// 	//nlab.drawParameterBar(EXP,pioneer.getValue(B_EXP));
	// 	nlab.drawParameterBar(ANX,pioneer.getValue(B_ANX));
	// 	nlab.drawParameterBar(OBJ, (float)GLData.distype);
	// }
	// else if(GLData.distype == 4){
	// 	nlab.drawParameterBar(MIS,pioneer.getValue(OY_MIS));
	// 	//nlab.drawParameterBar(EXP,pioneer.getValue(OY_EXP));
	// 	nlab.drawParameterBar(ANX,pioneer.getValue(OY_ANX));
	// 	nlab.drawParameterBar(OBJ, (float)GLData.distype);
	// }
	// else if(GLData.distype == 5){
  //
	// 	//nlab.drawParameterBar(MIS,pioneer.getValue(S1_MIS));
	// 	nlab.drawParameterBar(EXP,pioneer.getValue(EXP));
	// 	//nlab.drawParameterBar(ANX,pioneer.getValue(ANX));
	// 	nlab.drawParameterBar(OBJ, (float)GLData.distype);
	// }


	//ゴール位置を示す赤い犬（これはデバッグなのでcommand内のデータを用いる）
	// void Dog::draw2(double RED, double GREEN, double BLUE,double radius, clock_t time)
	{
		double RED = 1, GREEN = 0, BLUE = 0;
		double radius = 0;
		clock_t time = nlab.getTime();

		float i = 0.0;
		float n = 100.0;
		double a,b,c,d,e;
		static clock_t old = 0;
		static Position lleg = InitPose(),rleg = InitPose();
		static bool l_or_r = false; // false : left leg, true : right leg

		glColor4d(RED, GREEN, BLUE, 0.3);

		// 体，尾
		glPushMatrix();
		glTranslated(GLData.data.robot.posi.x, GLData.data.robot.posi.y, 0.0);
		// glTranslated(-750, 3500, 0.0);
		glRotated(GLData.data.command.body.deg-90, 0.0, 0.0, 1.0);
		glPushMatrix();
			glutSolidSphere(180, 300, 30);
			glBegin(GL_TRIANGLES);
			glVertex2d(120, 0);
			glVertex2d(- 120, 0);
			glVertex2d(0, -500);
		glEnd();
		glPopMatrix();
		glPopMatrix();

		// 頭
		glPushMatrix();
		glTranslated(GLData.data.robot.posi.x, GLData.data.robot.posi.y, 0.0);
		// glTranslated(-750, 3500, 0.0);
		glRotated(GLData.data.command.body.deg-90, 0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);
			glVertex2d(200, 100);
			glVertex2d(- 200, 100);
			glVertex2d(0, 450);
		glEnd();

		// 鼻
		glColor3d(0.0, 0.0, 0.0);
			glTranslated(0.0, 420, 0.0);
			glutSolidSphere(45, 30, 30);
		glPopMatrix();
		glColor3d(0.0, 0.0, 0.0);
		glFlush();
	}

	// void Dog::draw(double RED, double GREEN, double BLUE,double radius, clock_t time, int robot_sim_or_not, double dis)
	{
		double RED = 0.5636, GREEN = 0.29411, BLUE = 0.207843;
		clock_t time = nlab.getTime();


		static clock_t old;
		static Position lleg = InitPose(), rleg = InitPose();
		static bool l_or_r = false; // false : left leg, true : right leg

		// if((time - old) > 200)
		{
			if(l_or_r)
			{
				rleg = {GLData.data.robot.posi.x, GLData.data.robot.posi.y, 0, GLData.data.robot.body_deg};
				l_or_r = false;
			}
			else
			{
				lleg = {GLData.data.robot.posi.x, GLData.data.robot.posi.y, 0, GLData.data.robot.body_deg};
				l_or_r = true;
			}
			old = time;
		}

		drawFootMark(lleg, false);
		drawFootMark(rleg, true);

    //
		// // チルト表示（従来）
		// glColor4d(0.0/255.0, 0.0/255.0, 0.0/255.0, 1.0);
		// int base_x=-1200, base_y=3100;
		// double human_height=1600*(5.0/10.0), robot_height=550*(5.0/10.0);
		// double dist=calDistance(GLData.data.human[0].posi, GLData.data.robot.posi)*(5.0/10.0);
		// // 垂直線（人）
		// glBegin(GL_POLYGON);
		// 	glVertex2d(base_x,    base_y);
		// 	glVertex2d(base_x-20, base_y);
		// 	glVertex2d(base_x-20, base_y+human_height);
		// 	glVertex2d(base_x,    base_y+human_height);
		// glEnd();
		// // 水平線
		// glBegin(GL_POLYGON);
		// 	glVertex2d(base_x,      base_y);
		// 	glVertex2d(base_x+dist, base_y);
		// 	glVertex2d(base_x+dist, base_y+20);
		// 	glVertex2d(base_x,      base_y+20);
		// glEnd();
		// // ロボットの頭
		// glColor4d(RED, GREEN, BLUE, 0.3);
		// glPushMatrix();
		// glTranslated(base_x+dist, base_y+robot_height, 0.0);
		// glRotated(90 - GLData.data.robot.tilt_deg, 0.0, 0.0, 1.0);
		// glBegin(GL_TRIANGLES);
		// 	glVertex2d(200, 100);
		// 	glVertex2d(- 200, 100);
		// 	glVertex2d(0, 450);
		// glEnd();
		// 	glColor3d(0.0, 0.0, 0.0);
		// 	glTranslated(0.0, 420, 0.0);
		// 	glutSolidSphere(45, 30, 30);
		// glPopMatrix();
		// glFlush();
		// // ロボットの体
		// glColor4d(RED, GREEN, BLUE, 0.3);
		// glBegin(GL_POLYGON);
		// 	glVertex2d(base_x+dist-200, base_y);
		// 	glVertex2d(base_x+dist+300, base_y);
		// 	glVertex2d(base_x+dist+300, base_y+robot_height*1.5);
		// 	glVertex2d(base_x+dist-200, base_y+robot_height*1.5);
		// glEnd();

			// チルト表示（従来）
			glColor4d(0.0/255.0, 0.0/255.0, 0.0/255.0, 1.0);
			int base_x=3900, base_y=-850;
			double robot_height=550*(5.0/10.0);
			double dist=0;
			// ロボットの頭
			glColor4d(RED, GREEN, BLUE, 0.3);
			glPushMatrix();
			glTranslated(base_x+dist, base_y+robot_height, 0.0);
			glRotated(90 - GLData.data.robot.tilt_deg, 0.0, 0.0, 1.0);
			glBegin(GL_TRIANGLES);
				glVertex2d(200, 100);
				glVertex2d(- 200, 100);
				glVertex2d(0, 450);
			glEnd();
				glColor3d(0.0, 0.0, 0.0);
				glTranslated(0.0, 420, 0.0);
				glutSolidSphere(45, 30, 30);
			glPopMatrix();
			glFlush();
			// ロボットの体
			glColor4d(RED, GREEN, BLUE, 0.3);
			glBegin(GL_POLYGON);
				glVertex2d(base_x+dist-200, base_y);
				glVertex2d(base_x+dist+300, base_y);
				glVertex2d(base_x+dist+300, base_y+robot_height*1.5);
				glVertex2d(base_x+dist-200, base_y+robot_height*1.5);
			glEnd();


		if(std::isnan(GLData.data.robot.posi.x) || std::isnan(GLData.data.robot.posi.y))
		{
			std::this_thread::sleep_for(std::chrono::seconds(10));
			PRINT("NAN GL: %7.3lf  %7.3lf\n", GLData.data.robot.posi.x, GLData.data.robot.posi.y);
		}
		//printf("robot_x %lf robot_y %lf",GLData.data.robot.posi.x, GLData.data.robot.posi.y);


		glColor3d(RED, GREEN, BLUE);

		// 体，尾
		glPushMatrix();
		glTranslated(GLData.data.robot.posi.x, GLData.data.robot.posi.y, 0.0);
		glRotated(GLData.data.robot.body_deg - 90, 0.0, 0.0, 1.0);  // tk1701
		glPushMatrix();
			glutSolidSphere(180, 300, 30);
			glBegin(GL_TRIANGLES);
			glVertex2d(120, 0);
			glVertex2d(- 120, 0);
			glVertex2d(0, -500);
		glEnd();
		glPopMatrix();
		glPopMatrix();

		// 頭
		glPushMatrix();
		glTranslated(GLData.data.robot.posi.x, GLData.data.robot.posi.y, 0.0);
		glRotated(GLData.data.robot.body_deg - 90 + GLData.data.robot.pan_deg, 0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);
			glVertex2d(200, 100);
			glVertex2d(- 200, 100);
			glVertex2d(0, 450);
		glEnd();

		// 鼻
		glColor3d(0.0, 0.0, 0.0);
			glTranslated(0.0, 420, 0.0);
			glutSolidSphere(45, 30, 30);
		glPopMatrix();
		glColor3d(0.0, 0.0, 0.0);
		glFlush();
	}



	human_0.drawInnerState( 0 );
	human_1.drawInnerState( 1 );
	human_2.drawInnerState( 2 );
	// pioneer.drawInnerState();
	// pioneer.drawMode();

	//human_2.draw();
	// human_1.draw();
	// human_0.draw();
	// ball.draw();
	// ball.drawPower();

	drawHuman(0);
	drawHuman(1);
	drawHuman(2);

	// 人が「いた」場所
	if(data.human_leave_location != nullptr)
	{
		glPushMatrix();
			// 色指定
			glColor3d(0.5, 0, 0);
			// 現在位置
			glTranslated((*data.human_leave_location).x*s, (*data.human_leave_location).y*s, 0.0);
			glutSolidTorus(50, 150, 30, 30);
		glPopMatrix();
	}


	// ボールの描画
	// if(GLData.data.ball_st.ball_presence)
	{
		glPushMatrix();
			glTranslated(GLData.data.ball_tk.x, GLData.data.ball_tk.y, 0.0);
			glColor3d(0, 127.0/255.0, 0);
			glutSolidSphere(120, 30, 30);
		glPopMatrix();
		// PRINT("ball: %lf  %lf", GLData.data.ball_tk.x, GLData.data.ball_tk.y);
	}

//---------------------------------------------------------------------------
//	音源位置の描画、2016/05/26、塚田
//---------------------------------------------------------------------------
	/*
	// 音源方向
	if ( hark.size[0] > 0 ) {

		glPushMatrix();
			glTranslated(pioneer.getX(), pioneer.getY(), 0.0);
	//		glRotated(pioneer.getDeg() + hark.angle[0] - 90, 0.0, 0.0, 1.0);
			glRotated(sound_gaze_angle - 90, 0.0, 0.0, 1.0);
			//glTranslated(0, 1500, 0.0);
			//glRotated(current.deg-90, 0.0, 0.0, 1.0);

			glScaled(2000, 2000, 2000);	// 倍率
			glBegin(GL_TRIANGLES);	// 三角形の形（40度の鋭角を持つ三角形）
				glColor4ub(150, 255, 0, 120); glVertex2d(0, 0);
				glColor4ub(150, 255, 0,  10); glVertex2d(-tan(20*PI/180), 1);
				glColor4ub(150, 255, 0,  10); glVertex2d( tan(20*PI/180), 1);
			glEnd();
		glPopMatrix();
		glFlush();

	}

	// 実行ステータス
	int s = 0;
	char display[32] ={};

	s = 0; strcpy(display, "");

	if (GLData.sound_move)
	{
		if (!soundMoveFinishFlag) sprintf(display, "sound move: %3.1lfs left at most", 45-(double)(clock()-soundMoveBeginTime)/CLOCKS_PER_SEC);
		else sprintf(display, "sound move: %3.1lfs left", 2-(double)(clock()-soundMoveFinishTime)/CLOCKS_PER_SEC);
		glPushMatrix();
		glColor3ub(128, 0, 128);
			glTranslated(-3000.0, 3100.0, 0.0);
			glScaled(1.5, 1.5, 1.5);
			while (display[s] != '\0') { glutStrokeCharacter(GLUT_STROKE_ROMAN, display[s]); s++; }
		glPopMatrix();
	}

	s = 0; strcpy(display, "");


	if (sound_gaze)
	{
		sprintf(display, "sound gaze: %3.1lfs left", gaze_time_th-gaze_time);
		glPushMatrix();
		glColor3ub(128, 0, 128);
			glTranslated(1000.0, 3100.0, 0.0);
			glScaled(1.5, 1.5, 1.5);
			while (display[s] != '\0') { glutStrokeCharacter(GLUT_STROKE_ROMAN, display[s]); s++; }
		glPopMatrix();
	}


	if (sound_gaze || GLData.sound_move)
	{
		double sound_x, sound_y;
		sound_x = pioneer.getX() + 700*cos(sound_gaze_angle*(PI/180));
		sound_y = pioneer.getY() + 700*sin(sound_gaze_angle*(PI/180));

		// 黄色い円
		glPushMatrix();
			glTranslated(sound_x, sound_y, 0.0);
			glColor4ub(255, 255, 0, 180);
			glutSolidSphere(150, 30, 30);
		glPopMatrix();

		// 文字（M:Move か G:Gaze）
		glPushMatrix();
			glTranslated(sound_x-100, sound_y-100, 0.0);
			glColor4ub(255, 0, 0, 255);
			glScaled(2, 2, 2);
			glutStrokeCharacter(GLUT_STROKE_ROMAN, 'G');
		glPopMatrix();
	}
	*/


	// 経路
	for (int t=0; t<GLData.data.command.robot.path.size(); t++)
	{
		// ドット（円）
		glPushMatrix();
			glTranslated(GLData.data.command.robot.path[t].x, GLData.data.command.robot.path[t].y, 0.0);
			//glColor4ub(255, 255, 0, 255);
			glColor4d(0.0, 1.0, 0.0, ((double)(tki_MAX-t)/tki_MAX)*0.7 + 0.3);
			glutSolidSphere(50, 30, 30);
		glPopMatrix();
	}

	// サブゴール
	glPushMatrix();
		glTranslated(GLData.data.sub_goal.x, GLData.data.sub_goal.y, 0.0);
		//glColor4ub(255, 255, 0, 255);
		glColor4d(1.0, 0.5, 0.0, 1);
		glutSolidSphere(50, 30, 30);
	glPopMatrix();

	//壁を設置するためのポテンシャル玉
	glPushMatrix();
	glTranslated(GLData.data.potedama.posi.x, GLData.data.potedama.posi.y,0.0);
	glColor3d(0.0,1.0,1.0);
	glutSolidSphere(100,16.7,16.7);
	glPopMatrix();

	// 動物体
	if(map == 0)
	{
				glPushMatrix();
				glTranslated(GLData.data.object[1].posi.x, GLData.data.object[1].posi.y,0.0);
				glColor3d(1.0,0.0,1.0);
				glutSolidSphere(100,16.7,16.7);
				glPopMatrix();
	}

	if(map == 1)
	{
		for(int l=1;l<20/*l<data.object.size()+1*/;l++)
		{
			if(data.object[l].inmap == 1)
			{
				glPushMatrix();
				glTranslated(GLData.data.object[l].posi.x, GLData.data.object[l].posi.y,0.0);
				glColor3d(1.0,0.0,1.0);
				glutSolidSphere(100,16.7,16.7);
				glPopMatrix();
			}
		}
	}


	for(int a=0;a<60;a++)
	{
		for(int b=0;b<30;b++)
		{
			glColor4d(0.0,1.0,0.0,data.weight[a][b]/10000.0);
			glBegin(GL_QUADS);
			glVertex2d(-3000 + 100*a,3000 - 100*b);
			glVertex2d(-3000 + 100*(a+1), 3000 - 100*b);
			glVertex2d(-3000 + 100*(a+1), 3000-100*(b+1));
			glVertex2d(-3000 + 100*a, 3000 - 100*(b+1));
			glEnd();

		}
	}

	/*for(int a=0;a<300;a++)
	{
		for(int b=0;b<150;b++)
		{
			glColor4d(0.0,1.0,0.0,data.weight[a][b]/10000.0);
			glBegin(GL_QUADS);
			glVertex2d(-3000 + 20*a,3000 - 20*b);
			glVertex2d(-3000 + 20*(a+1), 3000 - 20*b);
			glVertex2d(-3000 + 20*(a+1), 3000-20*(b+1));
			glVertex2d(-3000 + 20*a, 3000 - 20*(b+1));
			glEnd();

		}
	}*/

	// 最小グリッドも、一度計算しただけで出して置いて、
	// 出力の時はすでにある配列の値を参照するだけにする！


	// 目標到達地点（ポテンシャルが最も低い点（2017/10/28、塚田））
	// for(int k=0; k<data.low_grids.size(); k++)
	// {
	// 	double size = WIDTH_ETHO/gridnum1;
	// 	glBegin(GL_POLYGON);
	// 		glColor4d(1.0, 1.0, 0.0, 1.0);	// RGBa
	// 		glVertex2d(RoomPoint[0] + data.low_grids[k].grid.i*size, RoomPoint[2] - data.low_grids[k].grid.j*size);
	// 		glVertex2d(RoomPoint[0] + (data.low_grids[k].grid.i + 1)*size, RoomPoint[2] - data.low_grids[k].grid.j*size);
	// 		glVertex2d(RoomPoint[0] + (data.low_grids[k].grid.i + 1)*size, RoomPoint[2] - (data.low_grids[k].grid.j + 1)*size);
	// 		glVertex2d(RoomPoint[0] + data.low_grids[k].grid.i*size, RoomPoint[2] - (data.low_grids[k].grid.j + 1)*size);
	// 	glEnd();
	// }


	// 目的地
	glPushMatrix();
		glTranslated(GLData.data.tmp_goal.x, GLData.data.tmp_goal.y, 0.0);
		// glTranslated(1500, 1500, 0.0);
		//glColor4ub(255, 255, 0, 255);
		glColor4d(0.0, 1.0, 0.0, 1);
		glutSolidSphere(50, 30, 30);
	glPopMatrix();


//---------------------------------------------------------------------------


/*
	// cv::Mat cvmtx(cv::Size(data.gl_w, data.gl_h), CV_8UC4, cv::Scalar(0, 0, 0));  // BGRA
	cv::Mat cvmtx(cv::Size(data.gl_w, data.gl_h), CV_8UC3, cv::Scalar(0, 0, 0));  // BGR

	glReadBuffer(GL_FRONT);// フロントを読み込む様に設定する
	// glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 初期値は4

	// glReadPixels(0, 0, data.gl_w, data.gl_h, GL_BGRA, GL_UNSIGNED_BYTE, (void*)cvmtx.data);  // BGRA
	glReadPixels(0, 0, data.gl_w, data.gl_h, GL_BGR, GL_UNSIGNED_BYTE, (void*)cvmtx.data);  // BGR

	//上下逆にする
	cv::flip(cvmtx, cvmtx, 0);



	static std::chrono::system_clock::time_point time_ini;
	if(data.fig_loop==0) time_ini = std::chrono::system_clock::now();

	std::chrono::system_clock::time_point time_now = std::chrono::system_clock::now();
	double time_duration;

	time_duration = (double)std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_ini).count()/1000;


	char fig_name[128] = "";
	// sprintf(fig_name, "fig/fig_%05d_%010.3lf.png", data.fig_loop++, time_duration);
	// sprintf(fig_name, "fig/fig_%010.3lf_%010d.png", time_duration, (int)(time_duration*1000));
	sprintf(fig_name, "fig/fig_%010d.png", (int)(time_duration*1000));
	data.fig_loop++;


	// cv::imwrite("output.png", cvmtx);
  cv::imwrite(fig_name, cvmtx);*/

	//---------------------------------------------------------------------------

	//	drawGoal(targetpose);//ここに書いた
	glutSwapBuffers();
}




void myReshape(int ww, int hh)
{
	glViewport(0, 0, ww, hh);
	glMatrixMode(GL_PROJECTION);

	data.gl_w = ww;
	data.gl_h = hh;
}


void myInit()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	// glutInitWindowSize(1280*s, 740*s);
	// glutInitWindowSize(1280, 740);
	glutInitWindowSize(data.gl_w, data.gl_h);
	glutInitWindowPosition(data.display_x_from_main, data.display_y_from_main);
	glutCreateWindow("Etho-Engine Simulator");
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutReshapeFunc(myReshape);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glMatrixMode(GL_MODELVIEW);

	// glOrtho(-3300, 5000, -1500, 4000, -10, 200);
	glOrtho(data.gl_x0, data.gl_x1, data.gl_y0, data.gl_y1, -10, 200);
}


void myTimer(int samplingTime)
{
	// 再描画
	glutPostRedisplay();
	// 再実行
	glutTimerFunc(samplingTime, myTimer, samplingTime);
}
