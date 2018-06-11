#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <resource_manager.h>
#include <particle_generator.h>
#include <planet_system.h>
#include <text_renderer.h>
#include <texture.h>
#include <learnopengl\camera.h>

#include <iostream>
#include <vector>
#include <string>

// GLFW function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mask);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, float dt);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
bool firstMouse = true;
bool LeftMouseButton = false;

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// OpenGL configuration
	glEnable(GL_DEPTH_TEST);

	ResourceManager::LoadShader("shaders/particle.vs", "shaders/particle.frag", nullptr, "particle");
	ResourceManager::LoadShader("shaders/planet.vert", "shaders/planet.frag", nullptr, "planet");
	ResourceManager::LoadShader("shaders/skybox.vs", "shaders/skybox.frag", nullptr, "skybox");
	ResourceManager::LoadTexture("resources/textures/container.jpg", false, "texture1");
	ResourceManager::LoadTexture("resources/textures/awesomeface.png", false, "texture2");
	std::vector<std::string> faces1
	{
		"resources/textures/ame_nebula/purplenebula_rt.tga",
		"resources/textures/ame_nebula/purplenebula_lf.tga",
		"resources/textures/ame_nebula/purplenebula_up.tga",
		"resources/textures/ame_nebula/purplenebula_dn.tga",
		"resources/textures/ame_nebula/purplenebula_ft.tga",
		"resources/textures/ame_nebula/purplenebula_bk.tga"
	};
	std::vector<std::string> faces2
	{
		"resources/textures/ame_starfield/starfield_rt.tga",
		"resources/textures/ame_starfield/starfield_lf.tga",
		"resources/textures/ame_starfield/starfield_up.tga",
		"resources/textures/ame_starfield/starfield_dn.tga",
		"resources/textures/ame_starfield/starfield_ft.tga",
		"resources/textures/ame_starfield/starfield_bk.tga"
	};
	std::vector<std::string> faces3
	{
		"resources/textures/sor_cwd/cwd_rt.JPG",
		"resources/textures/sor_cwd/cwd_lf.JPG",
		"resources/textures/sor_cwd/cwd_up.JPG",
		"resources/textures/sor_cwd/cwd_dn.JPG",
		"resources/textures/sor_cwd/cwd_ft.JPG",
		"resources/textures/sor_cwd/cwd_bk.JPG"
	};
	ResourceManager::LoadTexture3D(faces1, false, "skybox");
	ParticleGenerator *particleGenerator = new ParticleGenerator(ResourceManager::GetShader("particle"), Texture2D(), 1000);
	PlanetSystem *planetSystem = new PlanetSystem(ResourceManager::GetShader("planet"));
	TextRenderer *text = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
	text->Load("OCRAEXT.TTF", 24);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	ResourceManager::GetShader("skybox").Use().SetInteger("skybox", 0);
	ResourceManager::GetShader("planet").Use().SetVector3f("albedo", glm::vec3(0.5f, 0.5f, 0.5f));
	ResourceManager::GetShader("planet").SetFloat("ao", 1.0f);
	// DeltaTime variables
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	// lights
	// ------
	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f)
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f)
	};


	glm::vec3 centerPos = glm::vec3(0.0f, 0.0f, 0.0f);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		float fps = 1.0f / deltaTime;
		
		// input
		processInput(window, deltaTime);

		// Render
		glClearColor(0.3f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model;

		ResourceManager::GetShader("planet").Use();
		// pass projection matrix to shader (note that in this case it could change every frame)
		ResourceManager::GetShader("planet").SetMatrix4("projection", projection);
		// camera/view transformation
		ResourceManager::GetShader("planet").SetMatrix4("view", view);
		ResourceManager::GetShader("planet").SetVector3f("camPos", camera.Position);
		ResourceManager::GetShader("planet").SetFloat("metallic", 0.5f);
		ResourceManager::GetShader("planet").SetFloat("roughness", 0.5f);
		
		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			newPos = lightPositions[i];
			ResourceManager::GetShader("planet").SetVector3f(std::string("lightPositions[" + std::to_string(i) + "]").c_str(), newPos);
			ResourceManager::GetShader("planet").SetVector3f(std::string("lightColors[" + std::to_string(i) + "]").c_str(), lightColors[i]);
		}

		ResourceManager::GetShader("particle").Use().SetMatrix4("projection",projection);
		ResourceManager::GetShader("particle").SetMatrix4("view", view);
		particleGenerator->Update(deltaTime, 2, centerPos);
		particleGenerator->Draw();

		planetSystem->Update(deltaTime);
		planetSystem->Draw();



		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		ResourceManager::GetShader("skybox").Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		ResourceManager::GetShader("skybox").SetMatrix4("view", view);
		ResourceManager::GetShader("skybox").SetMatrix4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ResourceManager::GetTexture3D("skybox").ID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		text->RenderText("FPS:"+std::to_string(fps), 5.0f, 5.0f, 2.0f);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	ResourceManager::Clear();
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, float dt)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * dt;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, dt);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, dt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, dt);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if ((key == GLFW_KEY_W) && (action == GLFW_PRESS))
		{
			camera.ProcessKeyboard(FORWARD, 0.01f);
		}
		if ((key == GLFW_KEY_S) && (action == GLFW_PRESS))
		{
			camera.ProcessKeyboard(BACKWARD, 0.01f);
		}
		if ((key == GLFW_KEY_A) && (action == GLFW_PRESS))
		{
			camera.ProcessKeyboard(LEFT, 0.01f);
		}
		if ((key == GLFW_KEY_D) && (action == GLFW_PRESS))
		{
			camera.ProcessKeyboard(RIGHT, 0.01f);
		}
	}
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mask)
{
	if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS))
	{
		LeftMouseButton = true;
	}
	if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_RELEASE))
	{
		LeftMouseButton = false;
		firstMouse = true;
	}
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!LeftMouseButton)
	{
		return;
	}
	if (firstMouse && LeftMouseButton)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}