#include <cstring>
#include <cstdio>
#include <cmath>
#include "opengl.hpp"

#include "gv_extern_id.hpp"

#include "c_room.hpp"

Room::Room()
{
	Width = 6000.0;
	Hight = 3000.0;
}

void Room::drawBackground()
{
	// 背景(白)
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3d(0.0, 0.0, 0.0);
}

void Room::drawRoom()
{
	glColor3d(0.0, 0.0, 0.0);
	glLineWidth(30.0);

	// 部屋の枠線
	glBegin(GL_LINE_LOOP);
		glVertex2d(RoomPoint[0], RoomPoint[3]);
		glVertex2d(RoomPoint[1], RoomPoint[3]);
		glVertex2d(RoomPoint[1], RoomPoint[2]);
		glVertex2d(RoomPoint[0], RoomPoint[2]);
	glEnd();

	// 部屋の領域
	glColor3d(0.8784, 1.0, 1.0);
	glBegin(GL_POLYGON);
		glVertex2d(RoomPoint[0], RoomPoint[3]);
		glVertex2d(RoomPoint[1], RoomPoint[3]);
		glVertex2d(RoomPoint[1], RoomPoint[2]);
		glVertex2d(RoomPoint[0], RoomPoint[2]);
	glEnd();

	// 縦の仕切り線
	for(double m = 1000; m < 6000; m += 1000){
		glColor3d(0.9, 0.9, 0.9);
		glBegin(GL_LINES);
			glVertex2d(RoomPoint[0] + m, RoomPoint[3]);
			glVertex2d(RoomPoint[0] + m, RoomPoint[2]);
		glEnd();
	}

	// 横の仕切り線
	for(double n = 1000; n < 3000; n  += 1000){
		glColor3d(0.9, 0.9, 0.9);
		glBegin(GL_LINES);
			glVertex2d(RoomPoint[0], RoomPoint[3] + n);
			glVertex2d(RoomPoint[1], RoomPoint[3] + n);
		glEnd();
	}

	// ドア
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
		glVertex2d(RoomPoint[1] - 1000, RoomPoint[2] + 100);
		glVertex2d(RoomPoint[1] - 1000, RoomPoint[2] - 100);
		glVertex2d(RoomPoint[1], RoomPoint[2] - 100);
		glVertex2d(RoomPoint[1], RoomPoint[2] + 100);
	glEnd();
}

void Room::drawNoEnterArea()
{
//	glColor4d(1.0, 0.0, 0.0, 0.1);
	glColor4d(1.0, 0.0, 0.0, 0.4);
	glBegin(GL_POLYGON);
		glVertex2d(NoEnter[0], NoEnter[3]);
		glVertex2d(NoEnter[1], NoEnter[3]);
		glVertex2d(NoEnter[1], NoEnter[2]);
		glVertex2d(NoEnter[0], NoEnter[2]);
	glEnd();
}

void Room::drawNoSleepArea()
{
//	glColor4d(1.0, 0.0, 0.0, 0.05);
	glColor4d(1.0, 0.0, 0.0, 0.2);
	glBegin(GL_POLYGON);
		glVertex2d(NoSleep[0], NoSleep[3]);
		glVertex2d(NoSleep[1], NoSleep[3]);
		glVertex2d(NoSleep[1], NoSleep[2]);
		glVertex2d(NoSleep[0], NoSleep[2]);
	glEnd();
}

void Room::drawParameterBar(short type, float value)
{
	int i = 0;
	char display[16] = "";
	strcpy(display, "");
	glPushMatrix();
		if(type == EXP){
			glColor3d(1.0, 0.0, 0.0);
			glTranslated(3500, 3000, 0.0);
			sprintf(display, "explore %d", (int)value);
		}
		else if(type == ANX){
			glColor3d(0.0, 1.0, 0.0);
			glTranslated(3500, 2750, 0.0);
			sprintf(display, "anxiety %d", (int)value);
		}
		else if(type == MIS){
			glColor3d(0.0, 0.0, 1.0);
			glTranslated(3500, 2500, 0.0);
			sprintf(display, "miss %d", (int)value);
		}
		else if(type == OBJ){
			glColor3d(0, 0, 0);
			glTranslated(3500, 3500, 0.0);
			if((int)value == 0){
				sprintf(display, "owner");
			}
			else if((int)value == 1){
				sprintf(display, "stranger1");
			}
			else if((int)value == 2){
				sprintf(display, "stranger2");
			}
			else if((int)value == 3){
				sprintf(display, "ball");
			}
			else if((int)value == 4){
				sprintf(display, "oyatsu");
			}
			else if((int)value == 5){
				sprintf(display, "area");
			}
		}

		glScaled(1.7, 1.7, 1.7);
		while(display[i] != '\0'){
			glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);
			i++;
		}
	glPopMatrix();

}


void Room::drawTime()
{
	int i = 0;
	start = clock();
	char display[32] = {};
	glPushMatrix();
		glColor3d(0.0, 0.0, 0.0);
		glTranslated(-3100, 3400, 0.0);
		glScaled(2.0, 2.0, 2.0);
		strcpy(display, "");
		sprintf(display, " time %.2f", (double)start / CLOCKS_PER_SEC);
		while(display[i] != '\0'){
			glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);
			i++;
		}
	glPopMatrix();
}

void Room::drawKnownPlace(int number_of_place)
{
	int i = 0;
	char display[32]= {};
	glPushMatrix();
	glColor3d(0.0, 0.0, 0.0);
		glTranslated(3500, 2000, 0.0);
		glScaled(1.7, 1.7, 1.7);
		strcpy(display, "");
		sprintf(display, "place %d/18", number_of_place);
		while(display[i] != '\0'){
			glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);
			i++;
		}
	glPopMatrix();
}

//
// void Room::drawParameter(double value1 , double value2 , double value3 , double value4)
// {
// 	// int parameter_x_min = -2000;
// 	int parameter_x_min = -1750;
// 	int parameter_x_max = 2750;
// 	// int parameter_y_min = -1300;
// 	// int parameter_y_max = -500;
//
// 	int parameter_y_min = -850;
// 	int parameter_y_max = -250;
//
// 	int parameter_height = parameter_y_max - parameter_y_min;
// 	int parameter_width = parameter_x_max - parameter_x_min + (parameter_height / 6);
//
// 	double ato;
// 	double aos;
// 	double aou;
// 	double soa;
//
// 	char display[32] = "";
//
// 	// 枠
// 	glColor3d(0.90, 0.90, 0.90);
// 	glBegin(GL_QUAD_STRIP);
// 		glVertex2d(parameter_x_min, parameter_y_min);
// 		glVertex2d(parameter_x_max, parameter_y_min);
// 		glVertex2d(parameter_x_min, parameter_y_max);
// 		glVertex2d(parameter_x_max, parameter_y_max);
// 	glEnd();
//
// 	for(int j = 0; j < 3; j++)
// 	{
// 	// 左四角
// 	glColor3d(0.75, 0.75, 0.75);
// 	glBegin(GL_QUAD_STRIP);
// 		glVertex2d(parameter_x_min - (parameter_height / 3), parameter_y_max - (parameter_height / 3) * j);
// 		glVertex2d(parameter_x_min, parameter_y_max - (parameter_height / 3) * j);
// 		glVertex2d(parameter_x_min - (parameter_height / 3), parameter_y_max - ((parameter_height / 3) * (j + 1)));
// 		glVertex2d(parameter_x_min, parameter_y_max - ((parameter_height / 3) * (j + 1)));
//
// 	glEnd();
// 	}
//
// 	for(int j = 0; j < 3; j++)
// 	{
// 	// 右四角
// 	glColor3d(0.75, 0.75, 0.75);
// 	glBegin(GL_QUAD_STRIP);
// 		glVertex2d(parameter_x_max, parameter_y_max - (parameter_height / 3) * j);
// 		glVertex2d(parameter_x_max + (parameter_height / 3), parameter_y_max - (parameter_height / 3) * j);
// 		glVertex2d(parameter_x_max, parameter_y_max - ((parameter_height / 3) * (j + 1)));
// 		glVertex2d(parameter_x_max + (parameter_height / 3), parameter_y_max - ((parameter_height / 3) * (j + 1)));
// 	glEnd();
// 	}
//
// 	// attachment_to_owner
// 	//ato = attachment_to_owner / 25 - 2.0;
// 	// 仕切り線
// 	glColor3d(0.5, 0.5, 0.5);
// 	glLineWidth(2.0);
// 	for(int i = 1 ; i < 4 ; i++){
// 		glBegin(GL_LINES);
// 			glVertex2d(parameter_x_min - (parameter_height / 3), parameter_y_max - (parameter_height / 4)*i);
// 			glVertex2d(parameter_x_max + (parameter_height / 3), parameter_y_max - (parameter_height / 4)*i);
// 		glEnd();
// 	}
// 	glBegin(GL_LINE_LOOP);
// 		glVertex2d(parameter_x_min - (parameter_height / 3), parameter_y_max);
// 		glVertex2d(parameter_x_min - (parameter_height / 3), parameter_y_max - (parameter_height / 3) * 3);
// 		glVertex2d(parameter_x_min, parameter_y_max - ((parameter_height / 3) * 3));
// 		glVertex2d(parameter_x_min, parameter_y_max);
// 	glEnd();
// 	glColor3d(0.5, 0.5, 0.5);
// 	glBegin(GL_LINE_LOOP);
// 		glVertex2d(parameter_x_max, parameter_y_max);
// 		glVertex2d(parameter_x_max, parameter_y_max - parameter_height);
// 		glVertex2d(parameter_x_max + (parameter_height / 3), parameter_y_max - parameter_height);
// 		glVertex2d(parameter_x_max + (parameter_height / 3), parameter_y_max);
// 	glEnd();
//
// 	ato = (parameter_width / 100) * value1 + parameter_x_min;
//
// 	glColor4d(0.7, 0.0, 0.0 , 0.5);
// 	glBegin(GL_QUAD_STRIP);
// 		glVertex2d(ato - (parameter_height / 6), parameter_y_max);
// 		glVertex2d(ato + (parameter_height / 6), parameter_y_max);
// 		glVertex2d(ato - (parameter_height / 6), parameter_y_max - (parameter_height / 4));
// 		glVertex2d(ato + (parameter_height / 6), parameter_y_max - (parameter_height / 4));
// 	glEnd();
// 	glColor4d(0.7,0.0, 0.0, 1.0);
// 	glLineWidth(2.0);
// 	glBegin(GL_LINE_LOOP);
// 		glVertex2d(ato - (parameter_height / 6), parameter_y_max);
// 		glVertex2d(ato - (parameter_height / 6), parameter_y_max - (parameter_height / 4));
// 		glVertex2d(ato + (parameter_height / 6), parameter_y_max - (parameter_height / 4));
// 		glVertex2d(ato + (parameter_height / 6), parameter_y_max);
// 	glEnd();
//
// 	int i = 0;
// 	glPushMatrix();
// 		glColor3d(0.0, 0.0, 0.0);
// 		glTranslated(-3000, parameter_y_min + parameter_height*(3.0/4+1.0/8), 0.0);
// 		glScaled(0.7, 0.7, 0.7);
// 		strcpy(display, "");
// 		sprintf(display, "owner : %3.0f", value1);
// 		while(display[i] != '\0'){
// 			glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);
// 			i++;
// 		}
// 	glPopMatrix();
//
// 	// acceptance_of_stranger
// 	aos =  (parameter_width / 100) * value2 + parameter_x_min;
// 	glColor4d(0.0,0.0, 0.7, 0.5);
// 	glBegin(GL_QUAD_STRIP);
// 		glVertex2d(aos - (parameter_height / 6), parameter_y_max - (parameter_height / 4));
// 		glVertex2d(aos + (parameter_height / 6), parameter_y_max - (parameter_height / 4));
// 		glVertex2d(aos - (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 2);
// 		glVertex2d(aos + (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 2);
// 	glEnd();
// 	glColor4d(0.0, 0.0,0.7, 1.0);
// 	glLineWidth(2.0);
// 	glBegin(GL_LINE_LOOP);
// 		glVertex2d(aos - (parameter_height / 6), parameter_y_max - (parameter_height / 4));
// 		glVertex2d(aos - (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 2);
// 		glVertex2d(aos + (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 2);
// 		glVertex2d(aos + (parameter_height / 6), parameter_y_max - (parameter_height / 4));
// 	glEnd();
//
// 	i = 0;
// 	glPushMatrix();
// 		glColor3d(0.0, 0.0, 0.0);
// 		glTranslated(-3000, parameter_y_min + parameter_height*(2.0/4+1.0/8), 0.0);
// 		glScaled(0.7, 0.7, 0.7);
// 		strcpy(display, "");
// 		sprintf(display, "stranger : %3.0f", value2);
// 		while(display[i] != '\0'){
// 			glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);
// 			i++;
// 		}
// 	glPopMatrix();
//
// 	//for Unfamiliar
// 	aou =  (parameter_width / 100) * value3 + parameter_x_min;
// 	glColor4d(0.0, 0.7, 0.0 , 0.5);
// 	glBegin(GL_QUAD_STRIP);
// 		glVertex2d(aou - (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 2);
// 		glVertex2d(aou + (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 2);
// 		glVertex2d(aou - (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 3);
// 		glVertex2d(aou + (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 3);
// 	glEnd();
// 	glColor4d(0.0, 0.7, 0.0 , 1.0);
// 	glLineWidth(2.0);
// 	glBegin(GL_LINE_LOOP);
// 		glVertex2d(aou - (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 2);
// 		glVertex2d(aou - (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 3);
// 		glVertex2d(aou + (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 3);
// 		glVertex2d(aou + (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 2);
// 	glEnd();
//
// 	i = 0;
// 	glPushMatrix();
// 		glColor3d(0.0, 0.0, 0.0);
// 		glTranslated(-3000, parameter_y_min + parameter_height*(1.0/4+1.0/8), 0.0);
// 		glScaled(0.7, 0.7, 0.7);
// 		strcpy(display, "");
// 		sprintf(display, "Unfamiliar : %3.0f", value3);
// 		while(display[i] != '\0'){
// 			glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);
// 			i++;
// 		}
// 	glPopMatrix();
//
// 	// sensitivity_of_anxiety
// 	soa = (parameter_width / 100) * value4 + parameter_x_min;
// 	glColor4d(0.0, 1.0, 0.0 , 0.5);
// 	glBegin(GL_QUAD_STRIP);
// 		glVertex2d(soa - (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 3);
// 		glVertex2d(soa + (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 3);
// 		glVertex2d(soa - (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 4);
// 		glVertex2d(soa + (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 4);
// 	glEnd();
// 	glColor4d(0.0, 1.0, 0.0 , 1.0);
// 	glLineWidth(2.0);
// 	glBegin(GL_LINE_LOOP);
// 		glVertex2d(soa - (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 3);
// 		glVertex2d(soa - (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 4);
// 		glVertex2d(soa + (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 4);
// 		glVertex2d(soa + (parameter_height / 6), parameter_y_max - (parameter_height / 4) * 3);
// 	glEnd();
//
// 	i = 0;
// 	glPushMatrix();
// 		glColor3d(0.0, 0.0, 0.0);
// 		glTranslated(-3000, parameter_y_min + parameter_height*(0.0/4+1.0/8), 0.0);
// 		glScaled(0.7, 0.7, 0.7);
// 		strcpy(display, "");
// 		sprintf(display, "sensitivity of anxiety");
// 		while(display[i] != '\0'){
// 			glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);
// 			i++;
// 		}
// 	glPopMatrix();
//
// 	// 	i = 0;
// 	// glPushMatrix();
// 	// 	glColor3d(0.0, 0.0, 0.0);
// 	// 	glTranslated(3500, -900, 0.0);
// 	// 	glScaled(1, 1, 1);
// 	// 	strcpy(display, "");
// 	// 	sprintf(display, "Strength to throw");
// 	// 	while(display[i] != '\0'){
// 	// 		glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);
// 	// 		i++;
// 	// 	}
// 	// glPopMatrix();
// }

void Room::drawParameter(double Ro , double Rs , double value3 , double value4)
{
	Ro = (Ro-50)/50;
	Rs = (Rs-50)/50;

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


	char display[32] = "";

	// 全体四角（描画領域全体）
	glBegin(GL_QUAD_STRIP);
		glColor3d(0.75, 0.75, 0.75);
		glVertex2d(drawing_region_x_min, drawing_region_y_min);
		glVertex2d(drawing_region_x_min, drawing_region_y_max);
		glColor3d(0.95, 0.95, 0.95);
		glVertex2d(movable_region_center, drawing_region_y_min);
		glVertex2d(movable_region_center, drawing_region_y_max);
	glEnd();

	glBegin(GL_QUAD_STRIP);
		glColor3d(0.95, 0.95, 0.95);
		glVertex2d(movable_region_center, drawing_region_y_min);
		glVertex2d(movable_region_center, drawing_region_y_max);
		glColor3d(0.75, 0.75, 0.75);
		glVertex2d(drawing_region_x_max, drawing_region_y_min);
		glVertex2d(drawing_region_x_max, drawing_region_y_max);
	glEnd();

	// // 左四角（描画領域の内側の端に1セグメント分）
	// for(int j = 0; j < segment_n; j++)
	// {
	// 	// 塗りつぶし
	// 	glColor3d(0.75, 0.75, 0.75);
	// 	glBegin(GL_QUAD_STRIP);
	// 		glVertex2d(drawing_region_x_min, drawing_region_y_max - segment_height*j);
	// 		glVertex2d(drawing_region_x_min + segment_width, drawing_region_y_max - segment_height*j);
	// 		glVertex2d(drawing_region_x_min, drawing_region_y_max - segment_height*(j+1));
	// 		glVertex2d(drawing_region_x_min + segment_width, drawing_region_y_max - segment_height*(j+1));
	// 	glEnd();
	// }
	//
	// // 右四角（描画領域の内側の端に1セグメント分）
	// for(int j = 0; j < segment_n; j++)
	// {
	// 	// 塗りつぶし
	// 	glColor3d(0.75, 0.75, 0.75);
	// 	glBegin(GL_QUAD_STRIP);
	// 		glVertex2d(drawing_region_x_max - segment_width, drawing_region_y_max - segment_height*j);
	// 		glVertex2d(drawing_region_x_max, drawing_region_y_max - segment_height*j);
	// 		glVertex2d(drawing_region_x_max - segment_width, drawing_region_y_max - segment_height*(j+1));
	// 		glVertex2d(drawing_region_x_max, drawing_region_y_max - segment_height*(j+1));
	// 	glEnd();
	// }

	// 横-仕切り線（描画領域全体）
	glColor3d(0.5, 0.5, 0.5);
	glLineWidth(2.0);
	for(int i = 1 ; i < segment_n ; i++)
	{
		glBegin(GL_LINES);
			glVertex2d(drawing_region_x_min, drawing_region_y_max - segment_height*i);
			glVertex2d(drawing_region_x_max, drawing_region_y_max - segment_height*i);
		glEnd();
	}

	// // 縦-仕切り線（描画領域中央）
	// glBegin(GL_LINES);
	// 	glVertex2d(movable_region_center, drawing_region_y_max);
	// 	glVertex2d(movable_region_center, drawing_region_y_min);
	// glEnd();


	// オーナーへの信頼度
	{
		// 設定
		double R = Ro;
		int human_n = 0;

		// 計算
		int segment_center = R * movable_region_width/2 + movable_region_center;
		double color_abs = abs(R)*0.7 + 0.3;
		if(R > 0.01) glColor4d(1.0, 0.0, 0.0, color_abs);
		else if(R < -0.01) glColor4d(0.0, 0.0, 1.0, color_abs);
		else glColor4d(0.0, 1.0, 0.0, 1.0);

		// 四角
		glBegin(GL_QUAD_STRIP);
			glVertex2d(segment_center - segment_width/2, drawing_region_y_max - segment_height*human_n);
			glVertex2d(segment_center + segment_width/2, drawing_region_y_max - segment_height*human_n);
			glVertex2d(segment_center - segment_width/2, drawing_region_y_max - segment_height*(human_n+1));
			glVertex2d(segment_center + segment_width/2, drawing_region_y_max - segment_height*(human_n+1));
		glEnd();

		// 文字レンダリング
		glPushMatrix();
			glColor3d(1.0, 1.0, 1.0);
			glTranslated(segment_center - segment_width/2.35, drawing_region_y_max - segment_height*human_n - segment_height/1.7, 0.0);
			sprintf(display, "H%d %+4.2f", human_n, R);
			int i = 0;
			while(display[i] != '\0') {glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);	 i++;}
		glPopMatrix();
	}

	// 他者への信頼度
	{
		// 設定
		double R = Rs;
		int human_n = 1;

		// 計算
		int segment_center = R * movable_region_width/2 + movable_region_center;
		double color_abs = abs(R)*0.7 + 0.3;
		if(R > 0.01) glColor4d(1.0, 0.0, 0.0, color_abs);
		else if(R < -0.01) glColor4d(0.0, 0.0, 1.0, color_abs);
		else glColor4d(0.0, 1.0, 0.0, 1.0);

		// 四角
		glBegin(GL_QUAD_STRIP);
			glVertex2d(segment_center - segment_width/2, drawing_region_y_max - segment_height*human_n);
			glVertex2d(segment_center + segment_width/2, drawing_region_y_max - segment_height*human_n);
			glVertex2d(segment_center - segment_width/2, drawing_region_y_max - segment_height*(human_n+1));
			glVertex2d(segment_center + segment_width/2, drawing_region_y_max - segment_height*(human_n+1));
		glEnd();

		// 文字レンダリング
		glPushMatrix();
			glColor3d(1.0, 1.0, 1.0);
			glTranslated(segment_center - segment_width/2.35, drawing_region_y_max - segment_height*human_n - segment_height/1.7, 0.0);
			sprintf(display, "H%d %+4.2f", human_n, R);
			int i = 0;
			while(display[i] != '\0') {glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);	 i++;}
		glPopMatrix();
	}

}

void Room::drawGrid(int num1 , int num2)
{
	double size = (double)(Width/num1);
	glColor3d(0.0 , 0.0 , 0.0);
	glLineWidth(0.5);
	for(int i = 0 ; i <= num1*2; i++){
		glBegin(GL_LINES);
			glVertex3d(RoomPoint[0] + (double)(i * size/2) ,RoomPoint[2] , 0.1);
			glVertex3d(RoomPoint[0] + (double)(i * size/2) ,RoomPoint[3] , 0.1);
		glEnd();
	}
	for(int i = 0 ; i <= num2*2; i++){
		glBegin(GL_LINES);
			glVertex3d(RoomPoint[0] , RoomPoint[2] - (double)(i * size/2) ,0.1);
			glVertex3d(RoomPoint[1] , RoomPoint[2] - (double)(i * size/2) ,0.1);
		glEnd();
	}

	// ロボットが人を認識できる領域
	glColor3d(0.0 , 0.0 , 0.0);
	// 左・垂直線
	glBegin(GL_LINES);
		glVertex3d(RoomPoint[0], RoomPoint[2], 1);
		glVertex3d(RoomPoint[0], RoomPoint[2]+1500, 1);
	glEnd();
	// 上・水平線
	glBegin(GL_LINES);
		glVertex3d(RoomPoint[0], RoomPoint[2]+1500, 1);
		glVertex3d(RoomPoint[1]+1500, RoomPoint[2]+1500, 1);
	glEnd();
	// 右・垂直線
	glBegin(GL_LINES);
		glVertex3d(RoomPoint[1]+1500, RoomPoint[2]+1500, 1);
		glVertex3d(RoomPoint[1]+1500, RoomPoint[3], 1);
	glEnd();
	// 下・水平線
	glBegin(GL_LINES);
		glVertex3d(RoomPoint[1]+1500, RoomPoint[3], 1);
		glVertex3d(RoomPoint[1], RoomPoint[3], 1);
	glEnd();
}

Room::~Room()
{
}
