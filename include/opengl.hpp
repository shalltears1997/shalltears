
// OpenGLはDLLで提供されるため、下記3つのファイルが必要
	// ～.h		関数の定義
	// ～.lib	コンパイルに必要
	// ～.dll	実行に必要

#if defined(_WIN32) || defined(_WIN64)
	#pragma comment(lib,"C:/etho/OneDrive/etho/ee_win/freeglut/lib/freeglut.lib")
	#include <C:/etho/OneDrive/etho/ee_win/freeglut/include/GL/freeglut.h>
#elif defined(__APPLE__)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif