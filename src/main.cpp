/*
 * Example two meshes and two shaders (could also be used for Program 2)
 * includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */
#include <iostream>
#include <glad/glad.h>
#include <cfloat>
#include <cmath>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

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

	// Our shader program
	// std::shared_ptr<Program> solidColorProg;

	//another color prog

	// Shape to be used (from  file) - modify to support multiple
	vector<shared_ptr<Shape>> background;

	vector<shared_ptr<Shape>> dino;
	vector<shared_ptr<Shape>> cube;
	vector<shared_ptr<Shape>> sky_box;

	std::shared_ptr<Program> texProg;

	//a different mesh
	vector<shared_ptr<Shape>> jeep;

	shared_ptr<Texture> texture0;

	shared_ptr<Texture> texture1;

	shared_ptr<Texture> texture2;

//global data for ground plane - direct load constant defined CPU data to GPU (not obj)
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int g_GiboLen;
	//ground VAO
	GLuint GroundVertexArrayID;


	//for every shape make a vector for the shared pointers

	//example data that might be useful when trying to compute bounds on multi-shape
	glm::vec3 dinoMin = (glm::vec3)FLT_MAX;
	glm::vec3 dinoMax = (glm::vec3)FLT_MAX;

	//animation data
	float sTheta = 0;
	float gTrans = 0;
	float zTrans = 0;
	float zTheta = 0;
	float yTrans = 0;
	float yTheta = 0;
	float xTrans = 0;
	int flag = 0;

	float cameraX = 0.0f;
	float cameraY = 0.0f;
	float cameraZ = 0.0f;

	float moveSpeed = 0.1f;
	// int flip_var = 1;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        // Get the camera's direction vector
        glm::vec3 forward = glm::normalize(glm::vec3(cameraX, cameraY, cameraZ));
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0))); // Cross product with up vector to get right vector

        switch (key)
        {
        case GLFW_KEY_A:
            cameraX -= moveSpeed * right.x; // Move left along the camera's right vector
            cameraY -= moveSpeed * right.y; // Move left along the camera's right vector
            cameraZ -= moveSpeed * right.z; // Move left along the camera's right vector
            break;
        case GLFW_KEY_D:
            cameraX += moveSpeed * right.x; // Move right along the camera's right vector
            cameraY += moveSpeed * right.y; // Move right along the camera's right vector
            cameraZ += moveSpeed * right.z; // Move right along the camera's right vector
            break;
        case GLFW_KEY_W:
            cameraX += moveSpeed * forward.x; // Move forward along the camera's forward vector
            cameraY += moveSpeed * forward.y; // Move forward along the camera's forward vector
            cameraZ += moveSpeed * forward.z; // Move forward along the camera's forward vector
            break;
        case GLFW_KEY_S:
            cameraX -= moveSpeed * forward.x; // Move backward along the camera's forward vector
            cameraY -= moveSpeed * forward.y; // Move backward along the camera's forward vector
            cameraZ -= moveSpeed * forward.z; // Move backward along the camera's forward vector
            break;
        }
    }
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			// while(action != GLFW_RELEASE)
			// {
				yTrans -= 1;
			//}
		}
		if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			// while(action != GLFW_RELEASE)
			// {
				yTrans += 1;
			//}
		}
		if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			// while(action != GLFW_RELEASE)
			// {
				xTrans -= 1;
			//}
		}
		if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			// while(action != GLFW_RELEASE)
			// {
				xTrans += 1;
			//}
		}
		if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			// while(action != GLFW_RELEASE)
			// {
				yTheta -= 1;
			//}
		}
		if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			// while(action != GLFW_RELEASE)
			// {
				yTheta += 1;
			// }
		}
		if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			// while(action != GLFW_RELEASE)
			// {
				zTrans += 1;
			// }
		}
		if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			// while(action != GLFW_RELEASE)
			// {
				zTrans -= 1;
			// }
		}
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

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program that we will use for local shading
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
		texProg->addUniform("Texture1");
		texProg->addUniform("Texture2");
		// texProg->addUniform("MatShine");
		// texProg->addUniform("MatAmb");
		// texProg->addUniform("MatDif");
		// texProg->addUniform("MatSpec");
		// texProg->addUniform("MatShine");
		texProg->addUniform("lightPos");
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
  		texture1->setUnit(0);
  		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture2 = make_shared<Texture>();
  		texture2->setFilename(resourceDirectory + "/dark_background.jpeg");
  		texture2->init();
  		texture2->setUnit(0);
  		texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
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
				auto shape_obj = make_shared<Shape>(false);
				shape_obj->createShape(TOshapes[0]);
				shape_obj->measure();
				shape_obj->normalize_v();
				shape_obj->init();
				dino.push_back(shape_obj);

				dinoMin = glm::min(dinoMin, shape_obj->min);
    			dinoMax = glm::max(dinoMax, shape_obj->max);
			}
		}
		cout << "Dino has " << TOshapes.size() << " shapes" << endl;

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
		cout << "Background has " << TOshapes2.size() << " shapes" << endl;


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
				auto shape_obj = make_shared<Shape>(false);
				shape_obj->createShape(TOshapes3[0]);
				shape_obj->measure();
				shape_obj->normalize_v();
				shape_obj->init();
				jeep.push_back(shape_obj);

				// dinoMin = glm::min(dinoMin, shape_obj->min);
    			// dinoMax = glm::max(dinoMax, shape_obj->max);
			}
		}
		cout << "jeep has " << TOshapes3.size() << " shapes" << endl;

	
	vector<tinyobj::shape_t> TOshapes4;
	rc = tinyobj::LoadObj(TOshapes4, objMaterials, errStr, (resourceDirectory + "/Cube_obj.obj").c_str());
	if (!rc) {
		cerr << errStr << endl;
	} 
	else {
		for (auto shape:TOshapes4)
		{
			auto shape_obj = make_shared<Shape>(true); //TRUE MEANS IT IS A TEXTURE. THIS WAS NOT CLEARLY UNDERSTOOD AT ALL
			shape_obj->createShape(TOshapes4[0]);
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
				shape_obj->createShape(TOshapes5[0]);
				shape_obj->measure();
				shape_obj->normalize_v();
				shape_obj->init();
				sky_box.push_back(shape_obj);
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

	void render() {
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
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 150.0f);

		// View is global translation along negative z for now
		View->pushMatrix();
			View->loadIdentity();
			View->translate(vec3(-15 +  xTrans, -20+ yTrans, -50 + zTrans));
			View->rotate(yTheta, vec3(0, 1, 0));



		// Draw base Hierarchical person
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(prog->getUniform("lightPos"), 5.0, 13.0, 4.0);

		setModel(prog, vec3(5, -2.0, 0), 0, 0, 0.4);
		SetMaterial(prog,1);
		for (auto shapes:dino)
		{
			shapes->draw(prog);
		}

		//use helper function that uses glm to create some transform matrices
		setModel(prog, vec3(-1.7, -1.7, 0), 0, 0, 0.5);
		SetMaterial(prog,0);
		for (auto shapes:background)
		{
			shapes->draw(prog);
		}

		Model->pushMatrix();
			Model->translate(vec3(sTheta,0,0));
			Model->translate(vec3(50.0, 3.0, 10.0));
			Model->rotate(M_PI/180 * 230, vec3(0.0,1.0,0.0));
			Model->scale(.1);
			SetMaterial(prog,2);
			setModel(prog, Model);
				for (auto shapes:jeep)
				{

					shapes->draw(prog);
				}
		Model->popMatrix();

		prog->unbind();

		texProg->bind();

		//switch shaders to the texture mapping shader and draw the ground
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		setModel(texProg, vec3(0, -5.0, 50), 0, 0, 0.1);
		// glUniform1i(texProg->getUniform("flip"), flip_var);
		if(flag == 0)
		{
			texture0->bind(texProg->getUniform("Texture0"));
		}
		if(flag == 1) //COME BAKC
		{
			texture1->bind(texProg->getUniform("Texture1"));
		}
		for (auto shapes:cube)
		{
			shapes->draw(texProg);
		}

		setModel(texProg, vec3(1,0,0),0,0,80);
		// glUniform1i(texProg->getUniform("flip"), flip_var);
		texture2->bind(texProg->getUniform("Texture2"));	
		for (auto shapes:sky_box)
		{
			shapes->draw(texProg);
		}


		texProg->unbind();
		

		//animation update example
		sTheta = 30.0f*sin(2.0*M_PI * glfwGetTime());

		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();

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
    windowManager->init(640, 480);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;

    // This is the code that will likely change program to program as you
    // may need to initialize or set up different data and state

    application->init(resourceDir);
    application->initGeom(resourceDir);

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(windowManager->getHandle()))
    {
        // Handle keyboard input for camera movement
        application->keyCallback(windowManager->getHandle(), GLFW_KEY_A, 0, glfwGetKey(windowManager->getHandle(), GLFW_KEY_A), 0);
        application->keyCallback(windowManager->getHandle(), GLFW_KEY_D, 0, glfwGetKey(windowManager->getHandle(), GLFW_KEY_D), 0);
        application->keyCallback(windowManager->getHandle(), GLFW_KEY_W, 0, glfwGetKey(windowManager->getHandle(), GLFW_KEY_W), 0);
        application->keyCallback(windowManager->getHandle(), GLFW_KEY_S, 0, glfwGetKey(windowManager->getHandle(), GLFW_KEY_S), 0);

        // Render scene.
        application->render();

        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }

    // Quit program.
    windowManager->shutdown();
    return 0;
}

