#define _CRT_SECURE_NO_DEPRECATE

//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>


// Assimp includes

#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include <stdlib.h>
#include "assert.h"

// Header includes

#include "Mesh.h"
#include "Camera.h"
#include "RigidBody.h"
#include "Shaders.h"
#include "ShaderManager.h"

#include "AntTweakBar.h"

using namespace std;

int width = 1200;
int height = 900;

#define V_SHADER_PHONG "../Shaders/phongVertexShader.txt"
#define F_SHADER_PHONG "../Shaders/phongFragmentShader.txt"
#define V_SHADER_BLINNPHONG "../Shaders/blinnPhongVertexShader.txt"
#define F_SHADER_BLINNPHONG "../Shaders/blinnPhongFragmentShader.txt"
#define V_SHADER_GOURAD "../Shaders/gouradVertexShader.txt"
#define F_SHADER_GOURAD "../Shaders/gouradFragmentShader.txt"
#define V_SHADER_TOON "../Shaders/toonVertexShader.txt"
#define F_SHADER_TOON "../Shaders/toonFragmentShader.txt"
#define V_SHADER_GOOCH "../Shaders/goochVertexShader.txt"
#define F_SHADER_GOOCH "../Shaders/goochFragmentShader.txt"
#define MESH_MONKEY "Meshes/suzanne.dae"
#define MESH_SPHERE "Meshes/sphere.dae"
#define MESH_TORUS "Meshes/torus.dae"

int oldTime = 0;

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));

glm::vec4 light_pos(-5.0f, 0.0f, 5.0f, 1.0f); 
glm::vec3 la(1.0f, 1.0f, 1.0f); 
glm::vec3 ld(1.0f, 1.0f, 1.0f); 
glm::vec3 ls(1.0f, 1.0f, 1.0f); 

Mesh monkey_mesh(MESH_MONKEY);
RigidBody monkey(monkey_mesh);

Mesh sphere_mesh(MESH_SPHERE);
RigidBody sphere(sphere_mesh);

Mesh torus_mesh(MESH_TORUS);
RigidBody torus(torus_mesh);

glm::mat4 proj_mat, view_mat;

glm::vec3 monkey_turn_speed (0.0f, 2.0f, 0.0f);
glm::vec3 sphere_turn_speed (0.0f, 2.0f, 0.0f);
glm::vec3 torus_turn_speed (1.0f, 1.0f, 1.0f);

ShaderManager shaders;
Shaders *current_shader;

void addShaders()
{
	shaders.addShader("Gourad", V_SHADER_GOURAD, F_SHADER_GOURAD);
	shaders.addShader("Phong", V_SHADER_PHONG, F_SHADER_PHONG);
	shaders.addShader("Blinn-Phong", V_SHADER_BLINNPHONG, F_SHADER_BLINNPHONG);
	shaders.addShader("Toon", V_SHADER_TOON, F_SHADER_TOON);
	shaders.addShader("Gooch", V_SHADER_GOOCH, F_SHADER_GOOCH);
}


#pragma region TWEAK BAR STUFF


void init_tweak()
{
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(width, height);

	TwBar *bar;
	bar = TwNewBar("Rendering");

	shaders.addTBar(bar);
	camera.addTBar(bar);
	TwAddVarRW(bar, "Monkey Rotate Speed", TW_TYPE_DIR3F, &monkey_turn_speed, "");
	TwAddVarRW(bar, "Sphere Rotate Speed", TW_TYPE_DIR3F, &sphere_turn_speed, "");
	TwAddVarRW(bar, "Torus Rotate Speed", TW_TYPE_DIR3F, &torus_turn_speed, "");
	TwAddVarRW(bar, "Light Position", TW_TYPE_DIR3F, &light_pos, "");
	TwAddVarRW(bar, "Light Ambient", TW_TYPE_COLOR3F, &la, "");
	TwAddVarRW(bar, "Light Diffuse", TW_TYPE_COLOR3F, &ld, "");
	TwAddVarRW(bar, "Light Specular", TW_TYPE_COLOR3F, &ls, "");
	monkey.mesh.addTBar(bar);
	sphere.mesh.addTBar(bar);
	torus.mesh.addTBar(bar);
}

void draw_tweak()
{
	TwDraw();
}



#pragma endregion 


void init()
{
	addShaders();
	shaders.compileShaders();
	

	current_shader = &shaders.current_shader;

	monkey.load_mesh();
	monkey.ang_velocity = monkey_turn_speed;

	sphere.load_mesh();
	sphere.ang_velocity = sphere_turn_speed;
	sphere.position = glm::vec3(-3.0f, 0.0f, 0.0f);

	torus.load_mesh();
	torus.ang_velocity = torus_turn_speed;
	torus.position = glm::vec3(3.0f, 0.0f, 0.0f);

	init_tweak();

	camera.move_speed = 0.1f;
}

void display()
{

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glUseProgram(current_shader->id);

	int light_pos_location = glGetUniformLocation (current_shader->id, "LightPosition");
	glUniform4fv(light_pos_location, 1, glm::value_ptr(light_pos));

	int la_location = glGetUniformLocation (current_shader->id, "La");
	int ld_location = glGetUniformLocation (current_shader->id, "Ld");
	int ls_location = glGetUniformLocation (current_shader->id, "Ls");

	glUniform3fv(la_location, 1, glm::value_ptr(la));
	glUniform3fv(ld_location, 1, glm::value_ptr(ld));
	glUniform3fv(ls_location, 1, glm::value_ptr(ls));

	int model_mat_location = glGetUniformLocation (current_shader->id, "model");
	int view_mat_location = glGetUniformLocation (current_shader->id, "view");
	int proj_mat_location = glGetUniformLocation (current_shader->id, "proj");

	proj_mat = glm::perspective(45.0f, (float)width/(float)height, 0.1f, 200.0f);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, glm::value_ptr(proj_mat));

	
	view_mat = camera.getRotationMat();
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, glm::value_ptr(view_mat));

	

	monkey.draw(current_shader->id);
	sphere.draw(current_shader->id);
	torus.draw(current_shader->id);

	draw_tweak();

	glutSwapBuffers();
}

void updateScene()
{
	int time = glutGet(GLUT_ELAPSED_TIME);
	int delta_time = time-oldTime;
	oldTime = time;

	//time since last frame in seconds
	double elapsed_seconds = (double)delta_time/1000;

	monkey.ang_velocity = monkey_turn_speed;
	monkey.update(elapsed_seconds);

	sphere.ang_velocity = sphere_turn_speed;
	sphere.update(elapsed_seconds);

	torus.ang_velocity = torus_turn_speed;
	torus.update(elapsed_seconds);


	glutPostRedisplay();
}





int main(int argc, char** argv)
{

	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Rendering");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);


	 glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	 glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	 glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
	 glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	 glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
 
	  // send the ''glutGetModifers'' function pointer to AntTweakBar
	 TwGLUTModifiersFunc(glutGetModifiers);
 


	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
    return 0;
}