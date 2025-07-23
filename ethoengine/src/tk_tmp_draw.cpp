#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include "opengl.hpp"

#include "gv_extern.hpp"

#include "c_struct_position.hpp"
#include "l_tk_pote.hpp"
#include "c_human.hpp"


void Human::drawInnerState( int i )
{
	int graph_x_min1 = 3700;
	int graph_x_max1 = 4100;
	int graph_y_min1 = -690;
	int graph_y_min2 = 435;
	int graph_y_max1 = 410;

	int graph_width = graph_x_max1 - graph_x_min1;  //グラフの幅
	int graph_height = graph_y_max1 - graph_y_min1; //グラフの高さ
	double anx;
	double mis;

	//anx = anxiety / 50 - 2.0;
	anx = (graph_height / 100) * anxiety.value;
	glColor3d(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
		glVertex2d(graph_x_min1 + (405 * i), graph_y_min2);
		glVertex2d(graph_x_min1 + (405 * i) + (graph_width / 2), graph_y_min2);
		glVertex2d(graph_x_min1 + (405 * i) + (graph_width / 2), anx + graph_y_min2);
		glVertex2d(graph_x_min1 + (405 * i), anx + graph_y_min2);
	glEnd();

	//mis = miss / 50 - 2.0;
	mis = (graph_height / 100) * miss.value;
	glColor3d(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2d(graph_x_min1 + (405 * i) + (graph_width / 2), graph_y_min2);
		glVertex2d(graph_x_min1 + (405 * i) + (graph_width / 2) * 2, graph_y_min2);
		glVertex2d(graph_x_min1 + (405 * i) + (graph_width / 2) * 2, mis + graph_y_min2);
		glVertex2d(graph_x_min1 + (405 * i) + (graph_width / 2), mis + graph_y_min2);
	glEnd();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <ctime>
#include <cmath>
#include "opengl.hpp"

#include "gv_extern_id.hpp"

#include "c_struct_position.hpp"
#include "c_dog.hpp"


bool Dog::decreaseKnownLevel()
{
	int i,j;
	i = (int)(current.posex + WIDTH_ETHO/2)/(WIDTH_ETHO/area1);
	j = (int)(HEIGHT_ETHO - current.posey)/(HEIGHT_ETHO/area2);
	if(i < 0 || area1 < i || j < 0 || area2 < j) return false;
	else{
		known_level[i][j] = 0.0;
		if(0 <= (i - 1) && 0 <= (j - 1))		known_level[i-1][j-1];
		if(0 <= (i - 1) && (j + 1) < area2)		known_level[i-1][j+1];
		if((i + 1) < area1 && 0 <= (j -1))		known_level[i+1][j-1];
		if((i + 1) < area1 && (j + 1) < area2)	known_level[i+1][j+1];
	}
	return true;
}

int Dog::getNumberOfKnownPlace()
{
	int cnt = 0;
	for(int i = 0 ; i < area1 ; i++){
		for(int j = 0 ; j < area2 ; j++){
			if(known_level[i][j] > 0.50) cnt++;
		}
	}
	return cnt;
}
void Dog::drawKnownArea()
{
	for(int i = 0 ; i < area1 ; i++){
		for(int j = 0 ; j < area2 ; j++){
			glColor4d(0.75, 0.75, 1.0 , (double)known_level[i][j]);
				glBegin(GL_QUAD_STRIP);
					glVertex2d(RoomPoint[0] + (WIDTH_ETHO/area1) * i, RoomPoint[2] - (HEIGHT_ETHO/area2) * j);
					glVertex2d(RoomPoint[0] + (WIDTH_ETHO/area1) * (i + 1), RoomPoint[2] - (HEIGHT_ETHO/area2) * j);
					glVertex2d(RoomPoint[0] + (WIDTH_ETHO/area1) * (i + 1), RoomPoint[2] - (HEIGHT_ETHO/area2) * (j+1));
					glVertex2d(RoomPoint[0] + (WIDTH_ETHO/area1) * i, RoomPoint[2] - (HEIGHT_ETHO/area2) * (j+1));
				glEnd();
		}
	}
}
void Dog::drawInnerState()
{
	int graph_x_min1 = 3700;
	int graph_x_min2 = 4105;
	int graph_x_min3 = 4515;
	int graph_x_max1 = 4100;
	int graph_x_max2 = 4505;
	int graph_x_max3 = 4915;
	int graph_y_min1 = -690;
	int graph_y_min2 = 435;
	int graph_y_max1 = 410;
	int graph_y_max2 = 1535;

	int graph_width = graph_x_max1 - graph_x_min1;  //グラフの幅
	int graph_height = graph_y_max1 - graph_y_min1; //グラフの高さ
	double explo;
	double anx;
	double mis;
	double explo1;
	double anx1;
	double mis1;
	double explo2;
	double anx2;
	double mis2;
	double explo3;
	double anx3;
	double mis3;
	double explo4;
	double anx4;
	double mis4;
	double explo5;


	//anx = anxiety / 50 - 2.0;
	anx3 = (graph_height / 100) * b_anxiety.value;
	glColor3d(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
		glVertex2d(graph_x_min1 , graph_y_min1);
		glVertex2d(graph_x_min1 + (graph_width / 2), graph_y_min1);
		glVertex2d(graph_x_min1 + (graph_width / 2), anx3 + graph_y_min1);
		glVertex2d(graph_x_min1 , anx3 + graph_y_min1);
	glEnd();

	//mis = miss / 50 - 2.0;
	mis3 = (graph_height / 100) * b_miss.value;
	glColor3d(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2d(graph_x_min1 + (graph_width / 2), graph_y_min1);
		glVertex2d(graph_x_min1 + (graph_width / 2) * 2, graph_y_min1);
		glVertex2d(graph_x_min1 + (graph_width / 2) * 2, mis3 + graph_y_min1);
		glVertex2d(graph_x_min1 + (graph_width / 2), mis3 + graph_y_min1);
	glEnd();
//
	//explo4 = (graph_height / 100) * oy_explore.value;
	//glColor3d(1.0, 0.0, 0.0);
	//glBegin(GL_QUADS);
	//	glVertex2d(graph_x_min2, graph_y_min1);
	//	glVertex2d(graph_x_min2 + (graph_width / 3), graph_y_min1);
	//	glVertex2d(graph_x_min2 + (graph_width / 3), explo4 + graph_y_min1);
	//	glVertex2d(graph_x_min2, explo4 + graph_y_min1);
	//glEnd();

	//anx = anxiety / 50 - 2.0;
	anx4 = (graph_height / 100) * oy_anxiety.value;
	glColor3d(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
		glVertex2d(graph_x_min2, graph_y_min1);
		glVertex2d(graph_x_min2 + (graph_width / 2), graph_y_min1);
		glVertex2d(graph_x_min2 + (graph_width / 2), anx4 + graph_y_min1);
		glVertex2d(graph_x_min2, anx4 + graph_y_min1);
	glEnd();

	//mis = miss / 50 - 2.0;
	mis4 = (graph_height / 100) * oy_miss.value;
	glColor3d(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2d(graph_x_min2 + (graph_width / 2) , graph_y_min1);
		glVertex2d(graph_x_min2 + (graph_width / 2) * 2, graph_y_min1);
		glVertex2d(graph_x_min2 + (graph_width / 2) * 2, mis4 + graph_y_min1);
		glVertex2d(graph_x_min2 + (graph_width / 2) , mis4 + graph_y_min1);
	glEnd();
//
	explo5 = (graph_height / 100) * explore.value;
	glColor3d(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
		glVertex2d(graph_x_min3, graph_y_min1);
		glVertex2d(graph_x_min3 + (graph_width), graph_y_min1);
		glVertex2d(graph_x_min3 + (graph_width), explo5 + graph_y_min1);
		glVertex2d(graph_x_min3, explo5 + graph_y_min1);
	glEnd();

	/*//anx = anxiety / 50 - 2.0;
	anx1 = (graph_height / 100) * anxiety.value;
	glColor3d(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
		glVertex2d(graph_x_min2 + (graph_width / 3) , graph_y_min2);
		glVertex2d(graph_x_min2 + (graph_width / 3) * 2, graph_y_min2);
		glVertex2d(graph_x_min2 + (graph_width / 3) * 2, anx1 + graph_y_min2);
		glVertex2d(graph_x_min2 + (graph_width / 3) , anx1 + graph_y_min2);
	glEnd();

	//mis = miss / 50 - 2.0;
	mis1 = (graph_height / 100) * s_miss.value;
	glColor3d(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2d(graph_x_min2 + (graph_width / 3) * 2, graph_y_min2);
		glVertex2d(graph_x_min2 + (graph_width / 3) * 3, graph_y_min2);
		glVertex2d(graph_x_min2 + (graph_width / 3) * 3, mis1 + graph_y_min2);
		glVertex2d(graph_x_min2 + (graph_width / 3) * 2, mis1 + graph_y_min2);
	glEnd();*/

}
void Dog::drawMode()
{
	// 選択されているモードの表示
	int i = 0;
	char display[32];
	glPushMatrix();
	glColor3d(0.0, 0.0, 0.0);
	glTranslated(0.0, 3400.0, 0.0);
	glScaled(2.0, 2.0, 2.0);

	// 行動名
	if     (data.motion_id == GO_TO_SITTING_OWNER) sprintf(display, "GO_TO_SITTING_OWNER"); // 01
	else if(data.motion_id == PLAY_USING_BALL)     sprintf(display, "PLAY_USING_BALL");     // 02
	else if(data.motion_id == EXPLORE)             sprintf(display, "EXPLORE");             // 21
	else if(data.motion_id == MISSING)             sprintf(display, "MISSING");             // 22
	else if(data.motion_id == GO_TO_DOOR)          sprintf(display, "GO_TO_DOOR");          // 23
	else if(data.motion_id == GO_TO_OWNER)         sprintf(display, "GO_TO_OWNER");         // 24
	else if(data.motion_id == EXPLORE_STRANGER)    sprintf(display, "EXPLORE_STRANGER");    // 25
	else                                           sprintf(display, "Unknown; ID=%d.", data.motion_id);



	while(display[i] != '\0')
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);
		i++;
	}

	glPopMatrix();
}

void Dog::drawMode2()//kaita
{
	// 選択されているモードの表示
	int i = 0;
	char display[32] ={};
	glPushMatrix();
	glColor3d(0.0, 0.0, 0.0);
		glTranslated(0.0, 3400.0, 0.0);
		glScaled(2.0, 2.0, 2.0);
		strcpy(display, "");

		sprintf(display, "Push.");

		while(display[i] != '\0'){
			glutStrokeCharacter(GLUT_STROKE_ROMAN, display[i]);
			i++;
		}
	glPopMatrix();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
