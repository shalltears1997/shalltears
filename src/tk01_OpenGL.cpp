#include "opengl.hpp"
#include "02_gl.hpp"
#include "tk01_OpenGL.hpp"
#include "gv_extern.hpp"

void handleOpenGLClass::begin(int c, char** v)
{
	// glutInit(&argc, argv);
	glutInit(&c, v);
	myInit();
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouseFunc);
	glutDisplayFunc(myDisplay);
	//glutReshapeFunc(myReshape);
	glutMotionFunc(myMouseMotion);
	glutTimerFunc(samplingTime, myTimer, samplingTime);  // おそらく第3引数は第2引数の関数へ渡す引数
	// 一度上記glutMainLoop()を実行すると、ここMain()には戻ってこないので、以下は実行されない
	glutMainLoop();
}
