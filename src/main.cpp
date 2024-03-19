/*
 * Example two meshes and two shaders (could also be used for Program 2)
 * includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */
#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "stb_image.h"
#include "Bezier.h"
#include "Spline.h"
#include <chrono>
#include <stdio.h>
#include "particleSys.h"
#include "Particle.h"

#include <cstdlib> // For random number generation
#include <ctime>   // For seeding random number generator

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>
#define PI 3.1415927

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks 
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> partProg;
	std::shared_ptr<Program> texProg;



	// Our shader program
	// std::shared_ptr<Program> solidColorProg;

	particleSys *thePartSystem;
	particleSys *thePartSystem2;
	particleSys *thePartSystem3;
	particleSys *thePartSystem4;
	particleSys *thePartSystem5;
	particleSys *thePartSystem6;

	//another color prog

	// Shape to be used (from  file) - modify to support multiple
	vector<shared_ptr<Shape>> background;

	vector<shared_ptr<Shape>> dino;
	vector<shared_ptr<Shape>> cube;
	vector<shared_ptr<Shape>> sky_box;
	vector<shared_ptr<Shape>> cloud;
	vector<shared_ptr<Shape>> road;
	vector<shared_ptr<Shape>> rock;
	vector<shared_ptr<Shape>> wall;

	//a different mesh
	vector<shared_ptr<Shape>> jeep;

	shared_ptr<Texture> texture0;

	shared_ptr<Texture> texture1;

	shared_ptr<Texture> texture2;

	shared_ptr<Texture> texture3;

	shared_ptr<Texture> texture4;

	shared_ptr<Texture> texture5;

	shared_ptr<Texture> texture6;


//global data for ground plane - direct load constant defined CPU data to GPU (not obj)
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int g_GiboLen;
	//ground VAO
	GLuint GroundVertexArrayID;


	//for every shape make a vector for the shared pointers

	//example data that might be useful when trying to compute bounds on multi-shape
	glm::vec3 dinoMin = (glm::vec3)FLT_MAX;
	glm::vec3 dinoMax = (glm::vec3)FLT_MAX;

	//double g_phi, g_theta;
	vec3 view = vec3(10, 20, 5);
	vec3 strafe = vec3(1, 0, 0);
	vec3 g_eye = vec3(20, 20, 40);
	vec3 g_lookAt = vec3(0, 1, -10);
	vec3 up = vec3(0,1,0);

	Spline splinepath[2];
	bool goCamera = false;

	//animation data
	float sTheta = 0;
	float gTrans = 0;
	float zTrans = 0;
	float zTheta = 0;
	float yTrans = 0;
	float yTheta = 0;
	float xTrans = 0;
	float x_move = 0;

	double saved_x = 0;
	double saved_y = 0;
	double count = glfwGetTime();
	double lastUpdateTime = 0.0;

	double pitch = 0;
	double yaw = 0;

	int flag = 0;

	int super_flag = 0;
	int uber_flag = 0;

	// float cameraX = 0.0f;
	// float cameraY = 0.0f;
	// float cameraZ = 0.0f;

	float moveSpeed = 0.1f;

	//particle stuff from lab
	float t = 0.0f; // reset in init
	float h = 0.01f;
	// int flip_var = 1;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        // Get the camera's direction vector
        vec3 forward = normalize(g_lookAt - g_eye);
        vec3 right = normalize(cross(forward, up)); // Cross product with up vector to get right vector

        switch (key)
        {
        case GLFW_KEY_A:
			g_eye -= right;
			break;
        case GLFW_KEY_D:
            g_eye += right;
			break;
        case GLFW_KEY_W:
            g_eye += forward;
			break;
        case GLFW_KEY_S:
            g_eye -= forward;
			break;
        }
		g_lookAt = g_eye + forward;

    }
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		// if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		// 	// while(action != GLFW_RELEASE)
		// 	// {
		// 		xTrans += 1;
		// 	//}
		// }
		// if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		// 	// while(action != GLFW_RELEASE)
		// 	// {
		// 		xTrans -= 1;
		// 	// }
		// }
		// if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		// 	// while(action != GLFW_RELEASE)
		// 	// {
		// 		zTrans += 1;
		// 	// }
		// }
		// if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		// 	// while(action != GLFW_RELEASE)
		// 	// {
		// 		zTrans -= 1;
		// 	// }
		// }
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		if (key == GLFW_KEY_M && action == GLFW_PRESS) {
			flag = 1;
		}
		if (key == GLFW_KEY_M && action == GLFW_RELEASE) {
			flag = 0;
		}
		if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
			goCamera = !goCamera;
		}
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			thePartSystem->reSet();
		}
		// if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		// 	flip_var = 1;
		// }
		// if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
		// 	flip_var = 0;
		// }
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	// void mouse_pos_Callback(GLFWwindow *window, double x_pos, double y_pos)
	// {
		//cout << "X pos: " << x_pos << " Y pos: " << y_pos << endl;
		// double off_x = x_pos - saved_x;
		// double off_y = saved_y - y_pos;
		// saved_x = x_pos;
		// saved_y = y_pos;

		// float sensitivity = 0.1f;
		// off_x *= sensitivity;
		// off_y *= sensitivity;

		// yaw += glm::radians(off_x);
		// pitch += glm::radians(off_y);

		// pitch = glm::clamp(pitch, glm::radians(-80.0f), glm::radians(80.0f));

		// glm::vec3 look_at_pos;
		// look_at_pos.x = glm::cos(yaw) * glm::cos(pitch);
		// look_at_pos.y = glm::sin(pitch);
		// look_at_pos.z = glm::sin(yaw) * glm::cos(pitch);

		// glm::vec3 w = glm::normalize(look_at_pos);
		// glm::vec3 u = glm::normalize(glm::cross(vec3(0,1,0), w));
		// glm::vec3 v = glm::cross(w,u);

		// look_here = w + eye;
		// cout << "x: " << look_at_pos.x << " y: " << look_at_pos.x << " z: " << look_at_pos.x << endl; 
	// }
	void scrollCallback(GLFWwindow* window, double x_pos, double y_pos) {
   		//cout << "xDel + yDel " << deltaX << " " << deltaY << endl;
   		// double off_x = x_pos - saved_x;
		// double off_y = saved_y - y_pos;
		// saved_x = x_pos;
		// saved_y = y_pos;

		//float sensitivity = 0.1f;
		// off_x *= sensitivity;
		// off_y *= sensitivity;

		yaw += x_pos;
		pitch += y_pos;

		pitch = glm::clamp(pitch,-89.0, 89.0);

		float yaw_rad = radians(yaw);
		float pitch_rad = radians(pitch);

		vec3 look_at_pos;
		look_at_pos.x = cos(pitch_rad) * cos(yaw_rad);
		look_at_pos.y = -sin(pitch_rad);
		look_at_pos.z = sin(yaw_rad) * cos(pitch_rad);

		vec3 w = normalize(look_at_pos);
		vec3 u = normalize(cross(up, w));
		vec3 v = normalize(cross(w,u));

		g_lookAt = look_at_pos + g_eye;
		//cout << "x: " << look_at_pos.x << " y: " << look_at_pos.x << " z: " << look_at_pos.x << endl;
		saved_x = x_pos;
		saved_y = y_pos;
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		
		// Initialize the GLSL program that we will use for local shading
		partProg = make_shared<Program>();
		partProg->setVerbose(true);
		partProg->setShaderNames(
			resourceDirectory + "/lab10_vert.glsl",
			resourceDirectory + "/lab10_frag.glsl");
		if (! partProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		partProg->addUniform("P");
		partProg->addUniform("M");
		partProg->addUniform("V");
		partProg->addUniform("pColor");
		partProg->addUniform("alphaTexture");
		partProg->addAttribute("vertPos");

		
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("MatShine");
		prog->addUniform("lightPos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");

		// // Initialize the GLSL program.

		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		texProg->init();
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		// texProg->addUniform("flip");
		texProg->addUniform("Texture0");
		// texProg->addUniform("MatShine");
		// texProg->addUniform("MatAmb");
		// texProg->addUniform("MatDif");
		// texProg->addUniform("MatSpec");
		// texProg->addUniform("MatShine");
		//texProg->addUniform("lightPos");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");


		//read in a load the texture
		texture0 = make_shared<Texture>();
  		texture0->setFilename(resourceDirectory + "/Cube_diffuse.jpg");
  		texture0->init();
  		texture0->setUnit(0);
  		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		//extra texture
		texture1 = make_shared<Texture>();
  		texture1->setFilename(resourceDirectory + "/Cube_gloss.jpg");
  		texture1->init();
  		texture1->setUnit(1);
  		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture2 = make_shared<Texture>();
  		texture2->setFilename(resourceDirectory + "/dark_background.jpeg");
  		texture2->init();
  		texture2->setUnit(2);
  		texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture3 = make_shared<Texture>();
  		texture3->setFilename(resourceDirectory + "/cloud_sky.jpg");
  		texture3->init();
  		texture3->setUnit(3);
  		texture3->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture4 = make_shared<Texture>();
		texture4->setFilename(resourceDirectory + "/line.png");
		texture4->init();
		texture4->setUnit(4);
		texture4->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		
		texture5 = make_shared<Texture>();
		texture5->setFilename(resourceDirectory + "/diff_dino.jpg");
		texture5->init();
		texture5->setUnit(4);
		texture5->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture6 = make_shared<Texture>();
		texture6->setFilename(resourceDirectory + "/camo.jpg");
		texture6->init();
		texture6->setUnit(5);
		texture6->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		

		splinepath[0] = Spline(glm::vec3(-20,20,5), glm::vec3(30, 20,-10), glm::vec3(30, 20, 5), glm::vec3(35,20,5), 5);
		splinepath[1] = Spline(glm::vec3(35,20,5), glm::vec3(25,0,0), glm::vec3(20, 0, 0), glm::vec3(15,20,25), 3); 

		thePartSystem = new particleSys(vec3(60, 70, -50));
		thePartSystem2 = new particleSys(vec3(30, 70, -20));
		thePartSystem3 = new particleSys(vec3(0, 70, 10));
		thePartSystem4 = new particleSys(vec3(-30, 70, 40));
		thePartSystem5 = new particleSys(vec3(30, 70, 30));
		thePartSystem6 = new particleSys(vec3(-20, 70, -30));
		thePartSystem3->gpuSetup();
		thePartSystem->gpuSetup();
		thePartSystem2->gpuSetup();
		thePartSystem4->gpuSetup();
		thePartSystem5->gpuSetup();
		thePartSystem6->gpuSetup();
	}

	void initGeom(const std::string& resourceDirectory)
	{

		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
 		// Some obj files contain material information.We'll ignore them for this assignment.
 		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;
		//load in the mesh and make the shape(s)
 		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/Dino.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			for (auto shape:TOshapes)
			{
				auto shape_obj = make_shared<Shape>(true);
				shape_obj->createShape(shape);
				shape_obj->measure();
				shape_obj->normalize_v();
				shape_obj->init();
				dino.push_back(shape_obj);

				dinoMin = glm::min(dinoMin, shape_obj->min);
    			dinoMax = glm::max(dinoMax, shape_obj->max);
			}
		}
		//cout << "Dino has " << TOshapes.size() << " shapes" << endl;

		//load in another mesh and make the shape(s)
		vector<tinyobj::shape_t> TOshapes2;
 		rc = tinyobj::LoadObj(TOshapes2, objMaterials, errStr, (resourceDirectory + "/backgroundtest.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			for (auto shape:TOshapes2)
			{
				auto shape_obj = make_shared<Shape>(false);
				shape_obj->createShape(shape);
				shape_obj->measure();
				shape_obj->init();
				background.push_back(shape_obj); //saving shape_obj into the vector of backgrounds
			}
		}

		vector<tinyobj::shape_t> TOshapes7;
 		rc = tinyobj::LoadObj(TOshapes7, objMaterials, errStr, (resourceDirectory + "/Rock1.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			for (auto shape:TOshapes7)
			{
				auto shape_obj = make_shared<Shape>(false);
				shape_obj->createShape(shape);
				shape_obj->measure();
				shape_obj->init();
				rock.push_back(shape_obj); //saving shape_obj into the vector of backgrounds
			}
		}
		
		vector<tinyobj::shape_t> TOshapes8;
 		rc = tinyobj::LoadObj(TOshapes8, objMaterials, errStr, (resourceDirectory + "/wall.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			for (auto shape:TOshapes8)
			{
				auto shape_obj = make_shared<Shape>(false);
				shape_obj->createShape(shape);
				shape_obj->measure();
				shape_obj->init();
				wall.push_back(shape_obj); //saving shape_obj into the vector of backgrounds
			}
		}


		//read out information stored in the shape about its size - something like this...
		//then do something with that information.....
		// gMin.x = background->min.x;
		// gMin.y = backround->min.y;

	vector<tinyobj::shape_t> TOshapes3;
	rc = tinyobj::LoadObj(TOshapes3, objMaterials, errStr, (resourceDirectory + "/jeep.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			for (auto shape:TOshapes3)
			{
				auto shape_obj = make_shared<Shape>(true);
				shape_obj->createShape(shape);
				shape_obj->measure();
				shape_obj->normalize_v();
				shape_obj->init();
				jeep.push_back(shape_obj);

				// dinoMin = glm::min(dinoMin, shape_obj->min);
    			// dinoMax = glm::max(dinoMax, shape_obj->max);
			}
		}
		//cout << "jeep has " << TOshapes3.size() << " shapes" << endl;

	
	vector<tinyobj::shape_t> TOshapes4;
	rc = tinyobj::LoadObj(TOshapes4, objMaterials, errStr, (resourceDirectory + "/Cube_obj.obj").c_str());
	if (!rc) {
		cerr << errStr << endl;
	} 
	else {
		for (auto shape:TOshapes4)
		{
			auto shape_obj = make_shared<Shape>(true); //TRUE MEANS IT IS A TEXTURE. THIS WAS NOT CLEARLY UNDERSTOOD AT ALL
			shape_obj->createShape(shape);
			shape_obj->measure();
			shape_obj->normalize_v();
			shape_obj->init();
			cube.push_back(shape_obj);
		}
	}
	
	vector<tinyobj::shape_t> TOshapes5;
 		rc = tinyobj::LoadObj(TOshapes5, objMaterials, errStr, (resourceDirectory + "/sphereWTex.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			for (auto shape:TOshapes5)
			{
				auto shape_obj = make_shared<Shape>(true); //TRUE MEANS IT IS A TEXTURE. THIS WAS NOT CLEARLY UNDERSTOOD AT ALL
				shape_obj->createShape(shape);
				shape_obj->measure();
				shape_obj->normalize_v();
				shape_obj->init();
				sky_box.push_back(shape_obj);
			}
		}
	vector<tinyobj::shape_t> TOshapes6;
 		rc = tinyobj::LoadObj(TOshapes6, objMaterials, errStr, (resourceDirectory + "/road.obj").c_str());
		
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			for (auto shape:TOshapes6)
			{
				auto shape_obj = make_shared<Shape>(false);
				shape_obj->createShape(shape);
				shape_obj->measure();
				shape_obj->init();
				road.push_back(shape_obj); //saving shape_obj into the vector of backgrounds
			}
		}

	}

	
	

	void SetMaterial(shared_ptr<Program> curS, int i) 
	{
		switch (i) {
			case 0: //
				glUniform3f(curS->getUniform("MatAmb"), 0.039, 0.196, 0.039);
				glUniform3f(curS->getUniform("MatDif"), 0.0, 0.314, 0.0);
				glUniform3f(curS->getUniform("MatSpec"), 0.118, 0.118, 0.118);
				glUniform1f(curS->getUniform("MatShine"), 20.0);
			break;
			case 1: // 
				glUniform3f(curS->getUniform("MatAmb"), 0.2, 0.1, 0.0);
				glUniform3f(curS->getUniform("MatDif"), 0.1, 0.3, 0.1);
				glUniform3f(curS->getUniform("MatSpec"), 0.1, 0.1, 0.1);
				glUniform1f(curS->getUniform("MatShine"), 4.0);
			break;
			case 2: //
				glUniform3f(curS->getUniform("MatAmb"), 0.5, 0.5, 0.5);
				glUniform3f(curS->getUniform("MatDif"), 0.2, 0.6, 0.2);
				glUniform3f(curS->getUniform("MatSpec"), 0.3, 0.3, 0.3);
				glUniform1f(curS->getUniform("MatShine"), 70.0);
			case 3: //
				glUniform3f(curS->getUniform("MatAmb"), 0.5, 0.5, 0.5);
				glUniform3f(curS->getUniform("MatDif"), 0.2, 0.6, 0.2);
				glUniform3f(curS->getUniform("MatSpec"), 0.3, 0.3, 0.3);
				glUniform1f(curS->getUniform("MatShine"), 70.0);	
			break;
			case 4: //
				glUniform3f(curS->getUniform("MatAmb"), 1.0, 1.0, 1.0);
				glUniform3f(curS->getUniform("MatDif"), 1.0, 1.0, 1.0);
				glUniform3f(curS->getUniform("MatSpec"), 1.0, 1.0, 1.0);
				glUniform1f(curS->getUniform("MatShine"), 70.0);	
			break;
		}
	}


	/* helper for sending top of the matrix strack to GPU */
	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   }

	/* helper function to set model trasnforms */
  	void setModel(shared_ptr<Program> curS, vec3 trans, float rotY, float rotX, float sc) {
  		mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
  		mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
  		mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
  		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
  		mat4 ctm = Trans*RotX*RotY*ScaleS;
  		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
  	}

		/* camera controls - do not change */
	void SetView(shared_ptr<Program>  shader) {
  		glm::mat4 Cam = glm::lookAt(g_eye, g_lookAt, up);
  		glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
	}

	
	void perturbPoint(float& x, float& y, float& z, float magnitude) 
	{
		// Perturb the point by a random amount within the given magnitude
		x += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * magnitude;
		y += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * magnitude;
		z += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * magnitude;
	}


	void drawLightning(float x1, float y1, float z1, float x2, float y2, float z2, int iterations) 
	{
		if (iterations <= 0) {
			// Base case: draw a line segment
			glBegin(GL_LINES);
			glVertex3f(x1, y1, z1);
			glVertex3f(x2, y2, z2);
			glEnd();
		} else {
			float mx = (x1 + x2) / 2.0f;
			float my = (y1 + y2) / 2.0f;
			float mz = (z1 + z2) / 2.0f;

			// Perturb the midpoint
			float perturbationMagnitude = (x2 - x1 + y2 - y1 + z2 - z1) / 3.0f;
			perturbPoint(mx, my, mz, perturbationMagnitude);

			drawLightning(x1, y1, z1, mx, my, mz, iterations - 1);
			drawLightning(mx, my, mz, x2, y2, z2, iterations - 1);
		}
	}

	void updateUsingCameraPath(float frametime)  {

   	  if (goCamera) {
       if(!splinepath[0].isDone()){
       		splinepath[0].update(frametime);
            g_eye = splinepath[0].getPosition();
        } else {
            splinepath[1].update(frametime);
            g_eye = splinepath[1].getPosition();
        }
      }
   	}

	void render(float frametime) {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto V = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		updateUsingCameraPath(frametime);

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 150.0f);

		// // View is global view for the particles
		V->pushMatrix();
		V->loadIdentity();
		V->translate(vec3(-35, 20, -45));
		V->lookAt(g_eye, g_lookAt, up);
	

		// Draw base Hierarchical person
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		//glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		SetView(prog);
		glUniform3f(prog->getUniform("lightPos"), 5.0, 13.0, 4.0);

		// glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		//SetView(prog);
		//use helper function that uses glm to create some transform matrices
		setModel(prog, vec3(-1.7, -1.7, 0), 0, 0, 0.5);
		SetMaterial(prog,0);
		for (auto shapes:background)
		{
			shapes->draw(prog);
		}


		setModel(prog, vec3(10, -6, 13), 0, 0, 4.0f);
		SetMaterial(prog,1);
		for (auto shapes:road)
		{
			shapes->draw(prog);
		}
		
		setModel(prog, vec3(-60, 0, -10), 0, 0, 2.0f);
		SetMaterial(prog,1);
		for (auto shapes:wall)
		{
			shapes->draw(prog);
		}

		setModel(prog, vec3(60, 0, -10), 0, 0, 2.0f);
		SetMaterial(prog,1);
		for (auto shapes:wall)
		{
			shapes->draw(prog);
		}

		setModel(prog, vec3(0, -5, 0), 0, 0, 1.0f);
		SetMaterial(prog, 4);
		for (auto shapes:rock)
		{
			shapes->draw(prog);
		}

		//LIGHTNING STUFF HERE
		Model->pushMatrix();  // Save the current model matrix
		Model->loadIdentity();  // Reset model matrix to identity
		//setModel(prog, 2);
		glLineWidth(7.0f);
		float startX1 = -30.0f;
		float startY1 = 0.0f;
		float startZ1 = 20.0f;
		float endX1 = -10.0f;
		float endY1 = 70.0f;
		float endZ1 = -70.0f;

		float startX2 = 0.0f;
		float startY2 = 0.0f;
		float startZ2 = 0.0f;
		float endX2 = 10.0f;
		float endY2 = 70.0f;
		float endZ2 = -40.0f;

		float startX3 = -5.0f;
		float startY3 = 0.0f;
		float startZ3 = 50.0f;
		float endX3 = 40.0f;
		float endY3 = 70.0f;
		float endZ3 = 10.0f;

		float startX4 = -25.0f;
		float startY4 = 0.0f;
		float startZ4 = -50.0f;
		float endX4 = 10.0f;
		float endY4 = 70.0f;
		float endZ4 = -60.0f;

		//drawLightning(startX1, startY1, startZ1, endX1, endY1, endZ1, 4);
		double currentTime = glfwGetTime();
		if (currentTime - lastUpdateTime >= 2.0) {
			lastUpdateTime = currentTime;
			uber_flag++;
		}
		if (uber_flag == 0) {
			drawLightning(startX2, startY2, startZ2, endX2, endY2, endZ2, 3);
		} else if (uber_flag == 1) {
			drawLightning(startX3, startY3, startZ3, endX3, endY3, endZ3, 4);
		} else if (uber_flag == 2) {
			drawLightning(startX4, startY4, startZ4, endX4, endY4, endZ4, 5);
		}
		if(uber_flag == 3)
		{
			uber_flag = 0;
		}
		Model->popMatrix();  // Restore the previous model matrix

		prog->unbind();

		texProg->bind();

		//switch shaders to the texture mapping shader and draw the ground
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		//glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		SetView(texProg);
		glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));

				
		setModel(texProg, vec3(-10, -2.0, -40), 0, 0, 1.0);
		//SetMaterial(texProg,1);
		texture5->bind(texProg->getUniform("Texture0"));
		for (auto shapes:dino)
		{
			shapes->draw(texProg);
		}

		setModel(texProg, vec3(0, -5.0, 50), 0, 0, 0.1);
		// glUniform1i(texProg->getUniform("flip"), flip_var);
		if(flag == 0)
		{
			texture0->bind(texProg->getUniform("Texture0"));
		}
		if(flag == 1) //COME BAKC
		{
			texture1->bind(texProg->getUniform("Texture0"));
		}
		for (auto shapes:cube)
		{
			shapes->draw(texProg);
		}

		setModel(texProg, vec3(1,0,0),0,0,80);
		// glUniform1i(texProg->getUniform("flip"), flip_var);
		texture2->bind(texProg->getUniform("Texture0"));	
		for (auto shapes:sky_box)
		{
			shapes->draw(texProg);
		}


		Model->pushMatrix();
			Model->translate(vec3(x_move,0,0));
			Model->translate(vec3(50.0, 3.0, 10.0));
			Model->rotate(M_PI/180 * 230, vec3(0.0,1.0,0.0));
			Model->scale(0.1f);
			//SetMaterial(prog,2);
			texture6->bind(texProg->getUniform("Texture0"));
			setModel(texProg, Model);
			for (auto shapes:jeep)
			{
				shapes->draw(texProg);
			}
		Model->popMatrix();
		


		texProg->unbind();

		partProg->bind();
		texture4->bind(partProg->getUniform("alphaTexture"));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix())));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix())));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix())));
		
		CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.5, 0.5, 1.0));
		
		//thePartSystem->drawMe(partProg);
		CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
		CHECKED_GL_CALL(glEnable(GL_BLEND));
		//CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		CHECKED_GL_CALL(glPointSize(8.0f));
		
		thePartSystem2->drawMe(partProg);	
		thePartSystem3->drawMe(partProg);
		thePartSystem->drawMe(partProg);
		thePartSystem4->drawMe(partProg);
		thePartSystem5->drawMe(partProg);
		thePartSystem6->drawMe(partProg);

		thePartSystem->update();
		thePartSystem2->update();
		thePartSystem3->update();
		thePartSystem4->update();
		thePartSystem5->update();
		thePartSystem6->update();

		V->popMatrix();
		partProg->unbind();

		

		//animation update example
		sTheta = 125.0f*sin(2.0*M_PI * glfwGetTime());
		x_move -= 1.0f;
		if(x_move <= -160)
		{
			x_move = 0;
		}

		// Pop matrix stacks.
		
		Projection->popMatrix();

	}
};

int main(int argc, char *argv[])
{
    // Where the resources are loaded from
    std::string resourceDir = "../resources";

    if (argc >= 2)
    {
        resourceDir = argv[1];
    }

    Application *application = new Application();

    // Your main will always include a similar set up to establish your window
    // and GL context, etc.

    WindowManager *windowManager = new WindowManager();
    windowManager->init(1080, 720);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;

    // This is the code that will likely change program to program as you
    // may need to initialize or set up different data and state

    application->init(resourceDir);
    application->initGeom(resourceDir);

	//added from project 4 base code
	auto lastTime = chrono::high_resolution_clock::now();
	// GLfloat line_vertical[] =
	// 	{
	// 		200, 100, 0,
	// 		100, 300, 0
	// 	};

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(windowManager->getHandle()))
    {

		// glClear(GL_COLOR_BUFFER_BIT);
		// glEnableClientState(GL_VERTEX_ARRAY);
		// glVertexPointer(3, GL_FLOAT, 0, line_vertical);
		// glDrawArrays(GL_LINES, 0 , 2);
		// glDisableClientState(GL_VERTEX_ARRAY);
        auto nextLastTime = chrono::high_resolution_clock::now();

		// get time since last frame
		float deltaTime =
			chrono::duration_cast<std::chrono::microseconds>(
				chrono::high_resolution_clock::now() - lastTime)
				.count();
		// convert microseconds (weird) to seconds (less weird)
		deltaTime *= 0.000001;

		// reset lastTime so that we can calculate the deltaTime
		// on the next frame
		lastTime = nextLastTime;

        // Render scene.
        application->render(deltaTime);

        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }

    // Quit program.
    windowManager->shutdown();
    return 0;
}

