#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include <../stb_image.h>      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <../camera.h> // Camera class

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
	const char* const WINDOW_TITLE = "Jon Frodin - Final Project"; // Macro for window title

	// Variables for window width and height
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	// Stores the GL data relative to a given mesh
	struct GLMesh
	{
		GLuint vao;         // Handle for the vertex array object
		GLuint vbo;         // Handle for the vertex buffer object
		GLuint nVertices;    // Number of indices of the mesh
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;
	// Triangle mesh data
	GLMesh gMesh;
	// Texture
	GLuint gTextureId, gTextureId2, gTextureId3, gTextureId4, gTextureId5, gTextureId6, gTextureId7, gTextureId8, gTextureId9;
	glm::vec2 gUVScale(5.0f, 5.0f);
	GLint gTexWrapMode = GL_REPEAT;

	// Shader programs
	GLuint gCubeProgramId;
	GLuint gLampProgramId;
	GLuint gLampProgramId2;
	GLuint gLampProgramId3;

	// camera
	Camera gCamera(glm::vec3(0.0f, 0.0f, 7.0f));
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	// timing
	float gDeltaTime = 0.0f; // time between current frame and last frame
	float gLastFrame = 0.0f;

	// Subject position and scale
	glm::vec3 gCubePosition(0.0f, 0.0f, 0.0f);	//change this most likely
	glm::vec3 gCubeScale(2.0f);

	// Cube and light color
	//m::vec3 gObjectColor(0.6f, 0.5f, 0.75f);
	glm::vec3 gObjectColor(1.f, 0.2f, 0.0f);
	glm::vec3 gLightColor(1.0f, 1.0f, 1.0f);
	glm::vec3 gLightColor2(1.0f, 1.0f, 1.0f);
	glm::vec3 gLightColor3(1.0f, 1.0f, 1.0f);

	// Light position and scale
	glm::vec3 gLightPosition(0.0f, 3.0f, 3.0f);
	glm::vec3 gLightScale(0.3f);
	glm::vec3 gLightPosition2(0.5f, 0.9f, -0.3f);
	glm::vec3 gLightScale2(0.3f);
	glm::vec3 gLightPosition3(-0.5f, 0.9f, -0.3f);
	glm::vec3 gLightScale3(0.3f);

	// Lamp animation
	//bool gIsLampOrbiting = true;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char*[], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh &mesh);
void UDestroyMesh(GLMesh &mesh);
bool UCreateTexture(const char* filename, GLuint &textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint &programId);
void UDestroyShaderProgram(GLuint programId);
glm::mat4 getProjection(int projectionValueLocal);


/* Cube Vertex Shader Source Code*/
const GLchar * cubeVertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

	vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

	vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
	vertexTextureCoordinate = textureCoordinate;
}
);


/* Cube Fragment Shader Source Code*/
const GLchar * cubeFragmentShaderSource = GLSL(440,

	in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightColor2;	// second light
uniform vec3 lightColor3;	// Third light
uniform vec3 lightPos;	//first light
uniform vec3 lightPos2;	// second light
uniform vec3 lightPos3;	// third light
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
	/*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

	//Calculate Ambient lighting*/
	float ambientStrength = 0.004f; // Set ambient or global lighting strength (0.4%)
	float ambientStrength2 = 0.08f;	// ambient strength for white light (1%)
	float ambientStrength3 = 0.08f;	// ambient strength for white light (1%)
	vec3 ambient = ambientStrength * lightColor; // Generate ambient light color
	vec3 ambient2 = ambientStrength2 * lightColor2;
	vec3 ambient3 = ambientStrength3 * lightColor3;

	//Calculate Diffuse lighting*/
	vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
	vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
	vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos);
	vec3 lightDirection3 = normalize(lightPos3- vertexFragmentPos);
	float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
	float impact2 = max(dot(norm, lightDirection2), 0.0);
	float impact3 = max(dot(norm, lightDirection3), 0.0);
	vec3 diffuse = impact * lightColor; // Generate diffuse light color
	vec3 diffuse2 = impact2 * lightColor2;
	vec3 diffuse3 = impact3 * lightColor3;

	//Calculate Specular lighting*/
	float specularIntensity = 0.02f; // Set specular light strength 3% strength
	float specularIntensity2 = 0.05f;	// 5% strength
	float specularIntensity3 = 0.05f;	// 5% strength
	float highlightSize = 16.0f; // Set specular highlight size
	vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
	vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
	vec3 reflectDir2 = reflect(-lightDirection2, norm);
	vec3 reflectDir3 = reflect(-lightDirection3, norm);
	//Calculate specular component
	float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
	float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize);
	float specularComponent3 = pow(max(dot(viewDir, reflectDir3), 0.0), highlightSize);
	vec3 specular = specularIntensity * specularComponent * lightColor;
	vec3 specular2 = specularIntensity2 * specularComponent2 * lightColor2;
	vec3 specular3 = specularIntensity3 * specularComponent3 * lightColor3;

	// Texture holds the color to be used for all three components
	vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

	// Calculate phong result
	vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;
	vec3 phong2 = (ambient2 + diffuse2 + specular2) * textureColor.xyz;
	vec3 phong3 = (ambient3 + diffuse3 + specular3) * textureColor.xyz;


	fragmentColor = vec4(phong, 1.0) + vec4(phong2, 1.0) + vec4(phong3, 1.0); // Send lighting results to GPU
}
);


/* Lamp Shader Source Code*/
const GLchar * lampVertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

		//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Fragment Shader Source Code*/
const GLchar * lampFragmentShaderSource = GLSL(440,

	out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
	fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char *image, int width, int height, int channels)
{
	for (int j = 0; j < height / 2; ++j)
	{
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;

		for (int i = width * channels; i > 0; --i)
		{
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}
// global variable for toggling projection - default to perspective rather than ortho
int projectionValueGlobal = 0;

int main(int argc, char* argv[])
{
	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE;

	// Create the mesh
	UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object

	// Create the shader programs
	if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, gCubeProgramId))
		return EXIT_FAILURE;

	if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId))
		return EXIT_FAILURE;
	// second light
	if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId2))
		return EXIT_FAILURE;
	// third light
	if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId3))
		return EXIT_FAILURE;

	// Load texture
	// texture 1 = knobs
	const char * texFilename = "../resources/textures/knob2.jpg";
	if (!UCreateTexture(texFilename, gTextureId))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/carpet.jpg";
	// texture 2 = carpet
	if (!UCreateTexture(texFilename, gTextureId2)) {
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	// texture 3 = blank - used for black items like the tank hoses and frame and lighting
	// texture 4 = purple wood
	texFilename = "../resources/textures/purple_wood_3.jpg";
	if (!UCreateTexture(texFilename, gTextureId4)) {
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/underwater.png";
	if (!UCreateTexture(texFilename, gTextureId5)) {
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/transparent.png";
	if (!UCreateTexture(texFilename, gTextureId6)) {
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/pebbles.jpg";
	if (!UCreateTexture(texFilename, gTextureId7)) {
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/scales.jpg";
	if (!UCreateTexture(texFilename, gTextureId8)) {
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/driftwood.jpg";
	if (!UCreateTexture(texFilename, gTextureId9)) {
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	glUseProgram(gCubeProgramId);
	// We set the texture as texture unit 0
	glUniform1i(glGetUniformLocation(gCubeProgramId, "uTexture"), 0);

	// Sets the background color of the window to black (it will be implicitely used by glClear)
	glClearColor(0.0f, 0.1f, 0.1f, 1.0f);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(gWindow))
	{
		// per-frame timing
		// --------------------
		float currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;

		// input
		// -----
		UProcessInput(gWindow);

		// Render this frame
		URender();

		glfwPollEvents();
	}

	// Release mesh data
	UDestroyMesh(gMesh);

	// Release texture
	UDestroyTexture(gTextureId);

	// Release shader programs
	UDestroyShaderProgram(gCubeProgramId);
	UDestroyShaderProgram(gLampProgramId);
	UDestroyShaderProgram(gLampProgramId2);
	UDestroyShaderProgram(gLampProgramId3);

	exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	// GLFW: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// GLFW: window creation
	// ---------------------
	*window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
	glfwSetCursorPosCallback(*window, UMousePositionCallback);
	glfwSetScrollCallback(*window, UMouseScrollCallback);
	glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLEW: initialize
	// ----------------
	// Note: if using GLEW version 1.13 or earlier
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	// Displays GPU OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

	return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
	static const float cameraSpeed = 2.5f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		gCamera.ProcessKeyboard(LEFT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
	// new Q and E without using modified header
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.Position += gCamera.WorldUp * (gCamera.MovementSpeed * gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.Position -= gCamera.WorldUp *(gCamera.MovementSpeed * gDeltaTime);
	// old Q and E using a modified header
	/*if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.ProcessKeyboard(UP, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.ProcessKeyboard(DOWN, gDeltaTime);*/
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		if (projectionValueGlobal == 1) {
			getProjection(projectionValueGlobal);
			projectionValueGlobal = 0;
		}
		else {
			getProjection(projectionValueGlobal);
			projectionValueGlobal = 1;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && gTexWrapMode != GL_REPEAT)
	{
		glBindTexture(GL_TEXTURE_2D, gTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);

		gTexWrapMode = GL_REPEAT;

		cout << "Current Texture Wrapping Mode: REPEAT" << endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && gTexWrapMode != GL_MIRRORED_REPEAT)
	{
		glBindTexture(GL_TEXTURE_2D, gTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);

		gTexWrapMode = GL_MIRRORED_REPEAT;

		cout << "Current Texture Wrapping Mode: MIRRORED REPEAT" << endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_EDGE)
	{
		glBindTexture(GL_TEXTURE_2D, gTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		gTexWrapMode = GL_CLAMP_TO_EDGE;

		cout << "Current Texture Wrapping Mode: CLAMP TO EDGE" << endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_BORDER)
	{
		float color[] = { 1.0f, 0.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

		glBindTexture(GL_TEXTURE_2D, gTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glBindTexture(GL_TEXTURE_2D, 0);

		gTexWrapMode = GL_CLAMP_TO_BORDER;

		cout << "Current Texture Wrapping Mode: CLAMP TO BORDER" << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
	{
		gUVScale += 0.1f;
		cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
	{
		gUVScale -= 0.1f;
		cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
	}

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		if (action == GLFW_PRESS)
			cout << "Left mouse button pressed" << endl;
		else
			cout << "Left mouse button released" << endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
	{
		if (action == GLFW_PRESS)
			cout << "Middle mouse button pressed" << endl;
		else
			cout << "Middle mouse button released" << endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_RIGHT:
	{
		if (action == GLFW_PRESS)
			cout << "Right mouse button pressed" << endl;
		else
			cout << "Right mouse button released" << endl;
	}
	break;

	default:
		cout << "Unhandled mouse button event" << endl;
		break;
	}
}
// function to call from the 'P' toggle key to switch between ortho and perspective
glm::mat4 getProjection(int projectionValueLocal) {
	if (projectionValueLocal == 0) {
		glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
		return projection;
	}
	else {
		glm::mat4 projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);
		return projection;
	}
}
void renderLamp(GLuint lampProg, glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos, glm::vec3 lightScale) {
	glUseProgram(lampProg);

	//Transform the smaller cube used as a visual que for the light source
	glm::mat4 model = glm::translate(lightPos) * glm::scale(lightScale);

	// Reference matrix uniforms from the Lamp Shader program
	GLint modelLoc = glGetUniformLocation(lampProg, "model");
	GLint viewLoc = glGetUniformLocation(lampProg, "view");
	GLint projLoc = glGetUniformLocation(lampProg, "projection");

	// Pass matrix data to the Lamp Shader program's matrix uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	//glDrawArrays(GL_TRIANGLES, 0, 12);	draws the lamps but I turned it off for realism
}
// render a cube method
void renderCube(glm::mat4 rotation, glm::mat4 translation, glm::mat4 scale, GLuint texture) {
	// cosntrcut the model
	glm::mat4 model = translation * rotation * scale;

	// Set the shader to be used
	glUseProgram(gCubeProgramId);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gCubeProgramId, "model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gMesh.vao);
	// Reference matrix uniforms from the Cube Shader program for the cube color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(gCubeProgramId, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(gCubeProgramId, "lightColor");
	GLint lightColorLoc2 = glGetUniformLocation(gCubeProgramId, "lightColor2");
	GLint lightColorLoc3 = glGetUniformLocation(gCubeProgramId, "lightColor3");
	GLint lightPositionLoc = glGetUniformLocation(gCubeProgramId, "lightPos");
	GLint lightPositionLoc2 = glGetUniformLocation(gCubeProgramId, "lightPos2");
	GLint lightPositionLoc3 = glGetUniformLocation(gCubeProgramId, "lightPos3");
	GLint viewPositionLoc = glGetUniformLocation(gCubeProgramId, "viewPosition");

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
	glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
	glUniform3f(lightColorLoc3, gLightColor3.r, gLightColor3.g, gLightColor3.b);
	glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
	glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
	glUniform3f(lightPositionLoc3, gLightPosition3.x, gLightPosition3.y, gLightPosition3.z);
	const glm::vec3 cameraPosition = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Draws the triangle
	glDrawArrays(GL_TRIANGLES, 0, 36);		// Draws the cube - edit the numbers based on the new objects like the plane
}
// render a cylinder with data passed in from the render loop
void renderCylinder(glm::mat4 rotation, glm::mat4 translation, glm::mat4 scale, GLuint texture) {
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 model = translation * rotation * scale;

	// Set the shader to be used
	glUseProgram(gCubeProgramId);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gCubeProgramId, "model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gMesh.vao);

	// Reference matrix uniforms from the Cube Shader program for the cube color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(gCubeProgramId, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(gCubeProgramId, "lightColor");
	GLint lightColorLoc2 = glGetUniformLocation(gCubeProgramId, "lightColor2");
	GLint lightColorLoc3 = glGetUniformLocation(gCubeProgramId, "lightColor3");
	GLint lightPositionLoc = glGetUniformLocation(gCubeProgramId, "lightPos");
	GLint lightPositionLoc2 = glGetUniformLocation(gCubeProgramId, "lightPos2");
	GLint lightPositionLoc3 = glGetUniformLocation(gCubeProgramId, "lightPos3");
	GLint viewPositionLoc = glGetUniformLocation(gCubeProgramId, "viewPosition");

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
	glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
	glUniform3f(lightColorLoc3, gLightColor3.r, gLightColor3.g, gLightColor3.b);
	glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
	glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
	glUniform3f(lightPositionLoc3, gLightPosition3.x, gLightPosition3.y, gLightPosition3.z);
	const glm::vec3 cameraPosition = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Draws the triangle
	glDrawArrays(GL_TRIANGLES, 36, 156);		// Draws the cylinder - edit the numbers due to the plane and other objects now
}
// render a the plane at the bottom of the scene
void renderPyramid(glm::mat4 rotation, glm::mat4 translation, glm::mat4 scale, GLuint texture) {
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 model = translation * rotation * scale;

	// Set the shader to be used
	glUseProgram(gCubeProgramId);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gCubeProgramId, "model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gMesh.vao);

	// Reference matrix uniforms from the Cube Shader program for the cube color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(gCubeProgramId, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(gCubeProgramId, "lightColor");
	GLint lightColorLoc2 = glGetUniformLocation(gCubeProgramId, "lightColor2");
	GLint lightColorLoc3 = glGetUniformLocation(gCubeProgramId, "lightColor3");
	GLint lightPositionLoc = glGetUniformLocation(gCubeProgramId, "lightPos");
	GLint lightPositionLoc2 = glGetUniformLocation(gCubeProgramId, "lightPos2");
	GLint lightPositionLoc3 = glGetUniformLocation(gCubeProgramId, "lightPos3");
	GLint viewPositionLoc = glGetUniformLocation(gCubeProgramId, "viewPosition");

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
	glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
	glUniform3f(lightColorLoc3, gLightColor3.r, gLightColor3.g, gLightColor3.b);
	glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
	glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
	glUniform3f(lightPositionLoc3, gLightPosition3.x, gLightPosition3.y, gLightPosition3.z);
	const glm::vec3 cameraPosition = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Draws the triangle
	glDrawArrays(GL_TRIANGLES, 345, 18);		// Draws the pyramid
}
// render a the plane at the bottom of the scene
void renderPlane(glm::mat4 rotation, glm::mat4 translation, glm::mat4 scale, GLuint texture) {
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 model = translation * rotation * scale;

	// Set the shader to be used
	glUseProgram(gCubeProgramId);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gCubeProgramId, "model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gMesh.vao);

	// Reference matrix uniforms from the Cube Shader program for the cube color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(gCubeProgramId, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(gCubeProgramId, "lightColor");
	GLint lightColorLoc2 = glGetUniformLocation(gCubeProgramId, "lightColor2");
	GLint lightColorLoc3 = glGetUniformLocation(gCubeProgramId, "lightColor3");
	GLint lightPositionLoc = glGetUniformLocation(gCubeProgramId, "lightPos");
	GLint lightPositionLoc2 = glGetUniformLocation(gCubeProgramId, "lightPos2");
	GLint lightPositionLoc3 = glGetUniformLocation(gCubeProgramId, "lightPos3");
	GLint viewPositionLoc = glGetUniformLocation(gCubeProgramId, "viewPosition");

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
	glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
	glUniform3f(lightColorLoc3, gLightColor3.r, gLightColor3.g, gLightColor3.b);
	glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
	glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
	glUniform3f(lightPositionLoc3, gLightPosition3.x, gLightPosition3.y, gLightPosition3.z);
	const glm::vec3 cameraPosition = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Draws the triangle
	glDrawArrays(GL_TRIANGLES, 192, 36);		// Draws the cube - edit the numbers due to the plane and other objects now
}
// special methods for the water
void renderWater2(glm::mat4 rotation, glm::mat4 translation, glm::mat4 scale, GLuint texture) {
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 model = translation * rotation * scale;

	// Set the shader to be used
	glUseProgram(gCubeProgramId);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gCubeProgramId, "model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gMesh.vao);

	// Reference matrix uniforms from the Cube Shader program for the cube color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(gCubeProgramId, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(gCubeProgramId, "lightColor");
	GLint lightColorLoc2 = glGetUniformLocation(gCubeProgramId, "lightColor2");
	GLint lightColorLoc3 = glGetUniformLocation(gCubeProgramId, "lightColor3");
	GLint lightPositionLoc = glGetUniformLocation(gCubeProgramId, "lightPos");
	GLint lightPositionLoc2 = glGetUniformLocation(gCubeProgramId, "lightPos2");
	GLint lightPositionLoc3 = glGetUniformLocation(gCubeProgramId, "lightPos3");
	GLint viewPositionLoc = glGetUniformLocation(gCubeProgramId, "viewPosition");

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
	glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
	glUniform3f(lightColorLoc3, gLightColor3.r, gLightColor3.g, gLightColor3.b);
	glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
	glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
	glUniform3f(lightPositionLoc3, gLightPosition3.x, gLightPosition3.y, gLightPosition3.z);
	const glm::vec3 cameraPosition = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Draws the triangle
	glDrawArrays(GL_TRIANGLES, 6, 30);		// Draws front with the transparent texture
}
void renderWater1(glm::mat4 rotation, glm::mat4 translation, glm::mat4 scale, GLuint texture) {
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 model = translation * rotation * scale;

	// Set the shader to be used
	glUseProgram(gCubeProgramId);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gCubeProgramId, "model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gMesh.vao);

	// Reference matrix uniforms from the Cube Shader program for the cube color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(gCubeProgramId, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(gCubeProgramId, "lightColor");
	GLint lightColorLoc2 = glGetUniformLocation(gCubeProgramId, "lightColor2");
	GLint lightColorLoc3 = glGetUniformLocation(gCubeProgramId, "lightColor3");
	GLint lightPositionLoc = glGetUniformLocation(gCubeProgramId, "lightPos");
	GLint lightPositionLoc2 = glGetUniformLocation(gCubeProgramId, "lightPos2");
	GLint lightPositionLoc3 = glGetUniformLocation(gCubeProgramId, "lightPos3");
	GLint viewPositionLoc = glGetUniformLocation(gCubeProgramId, "viewPosition");

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
	glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
	glUniform3f(lightColorLoc3, gLightColor3.r, gLightColor3.g, gLightColor3.b);
	glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
	glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
	glUniform3f(lightPositionLoc3, gLightPosition3.x, gLightPosition3.y, gLightPosition3.z);
	const glm::vec3 cameraPosition = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Draws the triangle
	glDepthMask(false);	// make transparent foreground
	glDrawArrays(GL_TRIANGLES, 0, 6);		// Draws front with the transparent texture
	glDepthMask(true);	// re-activates the depth test after transparent foreground
	renderWater2(rotation, translation, scale, gTextureId5);	// draws the rest of the cube with the proper background
}
// global variables for the fish status
struct Fish{
	int startStatus;
	glm::vec3 startingPos;
	int size;
	int fishNum;
};
Fish Fish1{1, glm::vec3(1.0f, 0.5f, 0.4f), 1, 1};
Fish Fish2{2, glm::vec3(-0.4f, 0.3f, 0.0f), 1, 2};
Fish Fish3{2, glm::vec3(-0.6f, 0.5f, -0.2f), 2, 3};

glm::mat4 getFishRotation(Fish Fish) {
	//status 1 is facing left and status 2 is facing right
	// default rotation (right facing)
	glm::mat4 rotation;
	if (Fish.size == 1) {
		if (Fish.startStatus == 1) {
			rotation = rotate(3.0f, glm::vec3(0.0f, 2.0f, 0.0f));
			if (Fish.startingPos.x <= -1.0f) {
				rotation = rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
				Fish.startStatus = 2;
			}
		}
		else {
			rotation = rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			if (Fish.startingPos.x >= 1.0f) {
				rotation = rotate(3.0f, glm::vec3(0.0f, 2.0f, 0.0f));
				Fish.startStatus = 1;
			}
		}
	}
	// big fish
	else {
		if (Fish.startStatus == 1) {
			rotation = rotate(3.0f, glm::vec3(0.0f, 2.0f, 0.0f));
			if (Fish.startingPos.x <= -0.8f) {
				rotation = rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
				Fish.startStatus = 2;
			}
		}
		else {
			rotation = rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			if (Fish.startingPos.x >= 0.8f) {
				rotation = rotate(3.0f, glm::vec3(0.0f, 2.0f, 0.0f));
				Fish.startStatus = 1;
			}
		}
	}
	//updates the target fish
	if (Fish.fishNum == 1) { Fish1.startStatus = Fish.startStatus; }
	else if (Fish.fishNum == 2) { Fish2.startStatus = Fish.startStatus; }
	else { Fish3.startStatus = Fish.startStatus; }
	return rotation;
	/* I wanted to include the z axis and y axis in these to make sure we had really fluid movement, but I just ran out of time*/
}
glm::mat4 getFishPos(Fish Fish) {
	// pass status to the rotation module when necessary
	// 2 sizes for fish (1 big fish and 2 little fish
	glm::vec3 newPos = Fish.startingPos;
	// size 1 (small fish)
	if (Fish.startingPos.x > -1.0f && Fish.startStatus == 1 && Fish.size == 1) {
		newPos = Fish.startingPos + glm::vec3(-0.001f, 0.0f, 0.0f);
	}
	else if (Fish.startingPos.x <= -1.0f && Fish.startStatus == 1 && Fish.size == 1) {
		newPos = Fish.startingPos + glm::vec3(0.001f, 0.0f, 0.0f);
	}
	else if (Fish.startingPos.x < 1.0f && Fish.startStatus == 2 && Fish.size == 1) {
		newPos = Fish.startingPos + glm::vec3(0.001f, 0.0f, 0.0f);
	}
	else if (Fish.startingPos.x >= 1.0f && Fish.startStatus == 2 && Fish.size == 1) {
		newPos = Fish.startingPos + glm::vec3(-0.001f, 0.0f, 0.0f);
	}
	// size 2 (big fish)
	if (Fish.startingPos.x > -0.8f && Fish.startStatus == 1 && Fish.size == 2) {
		newPos = Fish.startingPos + glm::vec3(-0.001f, 0.0f, 0.0f);
	}
	else if (Fish.startingPos.x <= -0.8f && Fish.startStatus == 1 && Fish.size == 2) {
		newPos = Fish.startingPos + glm::vec3(0.001f, 0.0f, 0.0f);
	}
	else if (Fish.startingPos.x < 0.8f && Fish.startStatus == 2 && Fish.size == 2) {
		newPos = Fish.startingPos + glm::vec3(0.001f, 0.0f, 0.0f);
	}
	else if (Fish.startingPos.x >= 0.8f && Fish.startStatus == 2 && Fish.size == 2) {
		newPos = Fish.startingPos + glm::vec3(-0.001f, 0.0f, 0.0f);
	}
	
	glm::mat4 translation = translate(newPos);
	//update global pos
	if (Fish.fishNum == 1) { Fish1.startingPos = newPos; Fish1.startStatus = Fish.startStatus; }
	else if (Fish.fishNum == 2) { Fish2.startingPos = newPos; Fish2.startStatus = Fish.startStatus; }
	else { Fish3.startingPos = newPos; Fish3.startStatus = Fish.startStatus; }
	return translation;
}
// render a the plane at the bottom of the scene
void renderFish(glm::mat4 scale, GLuint texture, Fish Fish) {
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 translation2 = getFishPos(Fish);
	glm::mat4 rotation2 = getFishRotation(Fish);
	glm::mat4 model = translation2 * rotation2 * scale;

	// Set the shader to be used
	glUseProgram(gCubeProgramId);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gCubeProgramId, "model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gMesh.vao);

	// Reference matrix uniforms from the Cube Shader program for the cube color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(gCubeProgramId, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(gCubeProgramId, "lightColor");
	GLint lightColorLoc2 = glGetUniformLocation(gCubeProgramId, "lightColor2");
	GLint lightColorLoc3 = glGetUniformLocation(gCubeProgramId, "lightColor3");
	GLint lightPositionLoc = glGetUniformLocation(gCubeProgramId, "lightPos");
	GLint lightPositionLoc2 = glGetUniformLocation(gCubeProgramId, "lightPos2");
	GLint lightPositionLoc3 = glGetUniformLocation(gCubeProgramId, "lightPos3");
	GLint viewPositionLoc = glGetUniformLocation(gCubeProgramId, "viewPosition");

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
	glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
	glUniform3f(lightColorLoc3, gLightColor3.r, gLightColor3.g, gLightColor3.b);
	glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
	glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
	glUniform3f(lightPositionLoc3, gLightPosition3.x, gLightPosition3.y, gLightPosition3.z);
	const glm::vec3 cameraPosition = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Draws the triangle
	glDrawArrays(GL_TRIANGLES, 228, 117);		// Draws the fish
}

// Functioned called to render a frame
void URender()
{
	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the frame and z buffers
	glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate the cube VAO (used by cube and lamp)
	glBindVertexArray(gMesh.vao);

	// CUBE: draw cube
	//----------------
	// Set the shader to be used
	glUseProgram(gCubeProgramId);
	//default scale rotation and translation
	// 1. Scales the object by 2
	glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
	// 2. rotates the shape
	glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	// 3. Place object at the origin
	glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 model = glm::translate(gCubePosition) * rotation * glm::scale(gCubeScale);

	// camera/view transformation
	glm::mat4 view = gCamera.GetViewMatrix();

	// Creates a perspective projection
	//glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	// Calls the projection method to determine what status to use
	glm::mat4 projection = getProjection(projectionValueGlobal);

	// Retrieves and passes transform matrices to the Shader program
	//GLint modelLoc = glGetUniformLocation(gCubeProgramId, "model");
	GLint viewLoc = glGetUniformLocation(gCubeProgramId, "view");
	GLint projLoc = glGetUniformLocation(gCubeProgramId, "projection");

	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	GLint UVScaleLoc = glGetUniformLocation(gCubeProgramId, "uvScale");
	glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));


	// LAMP: draw lamp
	//----------------
	renderLamp(gLampProgramId, view, projection, gLightPosition, gLightScale);
	renderLamp(gLampProgramId2, view, projection, gLightPosition2, gLightScale2);
	renderLamp(gLampProgramId3, view, projection, gLightPosition3, gLightScale3);

	//scale data where needed
	glm::mat4 scaleKnobs = glm::scale(glm::vec3(0.1f, 0.1f, 0.0f));
	glm::mat4 scaleDrawers = glm::scale(glm::vec3(0.8f, 0.5f, 0.8f));
	glm::mat4 scaleTop = glm::scale(glm::vec3(3.5f, 0.2f, 1.0f));
	glm::mat4 scaleLeg = glm::scale(glm::vec3(1.6f, 0.1f, 0.05f));
	glm::mat4 scaleBack = glm::scale(glm::vec3(0.8f, 3.0f, 0.01f));
	glm::mat4 scaleSides = glm::scale(glm::vec3(0.88f, 0.6f, 0.01f));
	glm::mat4 scaleMidTray = glm::scale(glm::vec3(2.95f, 0.06f, 0.9f));
	glm::mat4 scaleBottomTray = glm::scale(glm::vec3(2.9f, 0.06f, 0.84f));
	glm::mat4 scaleFilterInlet = glm::scale(glm::vec3(0.1f, 0.1f, 0.4f));	// reuse for outlet
	glm::mat4 scaleFilterInlet2 = glm::scale(glm::vec3(0.1f, 0.1f, 0.2f));	// reuse for outlet
	glm::mat4 scaleFilterInlet3 = glm::scale(glm::vec3(0.1f, 0.1f, 2.6f));	// reuse for outlet
	glm::mat4 scaleTank = glm::scale(glm::vec3(3.0f, 1.0f, 0.9f));
	glm::mat4 scaleVerticalFrameSides = glm::scale(glm::vec3(0.02f, 0.93f, 0.02f));
	glm::mat4 scaleHorizontalFrameSides = glm::scale(glm::vec3(3.03f, 0.1f, 0.05f));
	glm::mat4 scaleLeftRightFrames = glm::scale(glm::vec3(0.05f, 0.1f, 0.95f));
	glm::mat4 scalePebbles = glm::scale(glm::vec3(3.0f, 0.1f, 0.95f));
	glm::mat4 scaleFish1 = glm::scale(glm::vec3(0.3f, 0.3f, 0.3f));	// reused for fish 2
	glm::mat4 scaleFish3 = glm::scale(glm::vec3(0.6f, 0.6f, 0.6f));
	glm::mat4 scaleDriftWood = glm::scale(glm::vec3(0.3f, 0.3f, 0.3f));

	// rotation data where needed
	glm::mat4 verticalCubeRotation = glm::rotate(1.581f, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 horizontalCubeRotation = glm::rotate(1.581f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 verticalCylinderRotation = glm::rotate(1.581f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateFish = glm::rotate(3.0f, glm::vec3(0.0f, 2.0f, 0.0f));
	//glm::mat4 horizontalCylinderRotation = glm::rotate(1.581f, glm::vec3(0.0f, 1.0f, 0.0f));

	// Position data for rendered objects
	glm::mat4 lefDrawerTranslation = glm::translate(glm::vec3(-1.0f, -0.4f, 0.0f));
	glm::mat4 rightDrawerTranslation = glm::translate(glm::vec3(1.0f, -0.4f, 0.0f));
	glm::mat4 topTranslation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 knob1Translation = glm::translate(glm::vec3(-1.0f, -0.4f, 0.45f));
	glm::mat4 knob2Translation = glm::translate(glm::vec3(1.0f, -0.4f, 0.45f));
	glm::mat4 leftFrontLegTranslation = glm::translate(glm::vec3(-1.53f, -0.8f, 0.4f));
	glm::mat4 rightFrontLegTranslation = glm::translate(glm::vec3(1.53f, -0.8f, 0.4f));
	glm::mat4 leftBackLegTranslation = glm::translate(glm::vec3(-1.53f, -0.8f, -0.4f));
	glm::mat4 rightBackLegTranslation = glm::translate(glm::vec3(1.53f, -0.8f, -0.4f));
	glm::mat4 backTranslation = glm::translate(glm::vec3(0.0f, -0.41f, -0.4f));
	glm::mat4 leftSideTranslation = glm::translate(glm::vec3(-1.48f, -0.41f, 0.0f));
	glm::mat4 rightSideTranslation = glm::translate(glm::vec3(1.48f, -0.41f, 0.0f));
	glm::mat4 midTrayTranslation = glm::translate(glm::vec3(0.0f, -0.7f, 0.0f));
	glm::mat4 bottomTrayTranslation = glm::translate(glm::vec3(0.0f, -1.3f, 0.0f));
	glm::mat4 middleLeftFrontLegTranslation = glm::translate(glm::vec3(-0.49f, -0.8f, 0.43f));
	glm::mat4 middleLeftBackLegTranslation = glm::translate(glm::vec3(-0.49f, -0.8f, -0.44f));
	glm::mat4 middleRightFrontLegTranslation = glm::translate(glm::vec3(0.49f, -0.8f, 0.43f));
	glm::mat4 middleRightBackLegTranslation = glm::translate(glm::vec3(0.49f, -0.8f, -0.44f));
	glm::mat4 filterInletTranslation = glm::translate(glm::vec3(1.0f, 1.1f, -0.35f));
	glm::mat4 filterInlet2Translation = glm::translate(glm::vec3(1.0f, 1.28f, -0.45f));
	glm::mat4 filterInlet3Translation = glm::translate(glm::vec3(1.0f, 0.03f, -0.6f));
	glm::mat4 filterOutletTranslation = glm::translate(glm::vec3(1.3f, 1.1f, -0.35f));
	glm::mat4 filterOutlet2Translation = glm::translate(glm::vec3(1.3f, 1.28f, -0.45f));
	glm::mat4 filterOutlet3Translation = glm::translate(glm::vec3(1.3f, 0.03f, -0.6f));
	glm::mat4 tankTranslation = glm::translate(glm::vec3(0.0f, 0.6f, 0.0f));
	glm::mat4 planeTranslation = glm::translate(glm::vec3(0.0f, 8.4f, 0.0f));
	glm::mat4 leftFrontSideTranslation = glm::translate(glm::vec3(-1.502f, 0.6f, 0.466f));
	glm::mat4 rightFrontSideTranslation = glm::translate(glm::vec3(1.502f, 0.6f, 0.466f));
	glm::mat4 leftBackSideTranslation = glm::translate(glm::vec3(-1.502f, 0.6f, -0.466f));
	glm::mat4 rightBackSideTranslation = glm::translate(glm::vec3(1.502f, 0.6f, -0.466f));
	glm::mat4 topFrontFrameTranslation = glm::translate(glm::vec3(0.0f, 1.07f, 0.466f));
	glm::mat4 bottomFrontFrameTranslation = glm::translate(glm::vec3(0.0f, 0.14f, 0.466f));
	glm::mat4 topBackFrameTranslation = glm::translate(glm::vec3(0.0f, 1.07f, -0.466f));
	glm::mat4 bottomBackFrameTranslation = glm::translate(glm::vec3(0.0f, 0.14f, -0.466f));
	glm::mat4 topLeftFrameTranslation = glm::translate(glm::vec3(-1.502f, 1.07f, 0.0f));
	glm::mat4 bottomLeftFrameTranslation = glm::translate(glm::vec3(-1.502f, 0.14f, 0.0f));
	glm::mat4 topRightFrameTranslation = glm::translate(glm::vec3(1.502f, 1.07f, 0.0f));
	glm::mat4 bottomRightFrameTranslation = glm::translate(glm::vec3(1.502f, 0.14f, 0.0f));
	glm::mat4 pebblesTranslation = glm::translate(glm::vec3(0.0f, 0.17f, 0.0f));
	glm::mat4 fish1Translation = glm::translate(glm::vec3(1.0f, 0.15f, 0.4f));	// brings fish forward for testing
	glm::mat4 driftwoodTranslation = glm::translate(glm::vec3(0.0f, 0.3f, 0.0f));

	// Draws the items in the scene
	//glDrawArrays(GL_TRIANGLES, 0, 36);		// Draws the triangle*/
	// render the idividual shapes in use
	renderCube(rotation, topTranslation, scaleTop, gTextureId4);	// top of stand (cube)
	renderCube(verticalCubeRotation, backTranslation, scaleBack, gTextureId4);	// back of stand (cube)
	renderCube(rotation, lefDrawerTranslation, scaleDrawers, gTextureId4);	// left drawer (cube)
	renderCube(rotation, rightDrawerTranslation, scaleDrawers, gTextureId4);	// right drawer (cube)
	renderCylinder(rotation, knob1Translation, scaleKnobs, gTextureId);		// knob of left drawer (cylinder)
	renderCylinder(rotation, knob2Translation, scaleKnobs, gTextureId);		// knob of right drawer (cylinder)
	renderCube(verticalCubeRotation, leftFrontLegTranslation, scaleLeg, gTextureId4);	// left front leg (cube)
	renderCube(verticalCubeRotation, rightFrontLegTranslation, scaleLeg, gTextureId4);	// right front leg (cube)
	renderCube(verticalCubeRotation, leftBackLegTranslation, scaleLeg, gTextureId4);		// left back leg (cube)
	renderCube(verticalCubeRotation, rightBackLegTranslation, scaleLeg, gTextureId4);	// right back leg (cube)
	renderCube(horizontalCubeRotation, leftSideTranslation, scaleSides, gTextureId4);	// left side (cube)
	renderCube(horizontalCubeRotation, rightSideTranslation, scaleSides, gTextureId4);	// right side (cube)
	renderCube(rotation, midTrayTranslation, scaleMidTray, gTextureId4);		// middle tray (cube)
	renderCube(rotation, bottomTrayTranslation, scaleBottomTray, gTextureId4);	// bottom tray (cube)
	renderCube(verticalCubeRotation, middleLeftFrontLegTranslation, scaleLeg, gTextureId4);	// middle left front leg (cube)
	renderCube(verticalCubeRotation, middleRightFrontLegTranslation, scaleLeg, gTextureId4);	// middle right front leg (cube)
	renderCube(verticalCubeRotation, middleLeftBackLegTranslation, scaleLeg, gTextureId4);	// middle left back leg (cube)
	renderCube(verticalCubeRotation, middleRightBackLegTranslation, scaleLeg, gTextureId4);	// middle right back leg (cube)
	renderCylinder(verticalCylinderRotation, filterInletTranslation, scaleFilterInlet, gTextureId3);	// filter inlet (cylinder)
	renderCylinder(rotation, filterInlet2Translation, scaleFilterInlet2, gTextureId3);	// second part where inlet turns (cylinder)
	renderCylinder(verticalCylinderRotation, filterInlet3Translation, scaleFilterInlet3, gTextureId3);	// third part of the inlet pipe going down to filter (cylinder)
	renderCylinder(verticalCylinderRotation, filterOutletTranslation, scaleFilterInlet, gTextureId3);	// first part of the filter outlet pipe (cylinder)
	renderCylinder(rotation, filterOutlet2Translation, scaleFilterInlet2, gTextureId3);	// second part where outlet turns (cylinder)
	renderCylinder(verticalCylinderRotation, filterOutlet3Translation, scaleFilterInlet3, gTextureId3);	// third part of the outlet pipe going down to the filter (cylidner)
	renderWater1(rotation, tankTranslation, scaleTank, gTextureId6);
	renderPlane(rotation, planeTranslation, scale, gTextureId2);	// draws the plane floor for everything to sit on
	renderCube(rotation, leftFrontSideTranslation, scaleVerticalFrameSides, gTextureId3);
	renderCube(rotation, rightFrontSideTranslation, scaleVerticalFrameSides, gTextureId3);
	renderCube(rotation, leftBackSideTranslation, scaleVerticalFrameSides, gTextureId3);
	renderCube(rotation, rightBackSideTranslation, scaleVerticalFrameSides, gTextureId3);
	renderCube(rotation, topFrontFrameTranslation, scaleHorizontalFrameSides, gTextureId3);
	renderCube(rotation, bottomFrontFrameTranslation, scaleHorizontalFrameSides, gTextureId3);
	renderCube(rotation, topBackFrameTranslation, scaleHorizontalFrameSides, gTextureId3);
	renderCube(rotation, bottomBackFrameTranslation, scaleHorizontalFrameSides, gTextureId3);
	renderCube(rotation, topLeftFrameTranslation, scaleLeftRightFrames, gTextureId3);
	renderCube(rotation, bottomLeftFrameTranslation, scaleLeftRightFrames, gTextureId3);
	renderCube(rotation, topRightFrameTranslation, scaleLeftRightFrames, gTextureId3);
	renderCube(rotation, bottomRightFrameTranslation, scaleLeftRightFrames, gTextureId3);
	renderCube(rotation, pebblesTranslation, scalePebbles, gTextureId7);
	renderFish(scaleFish1, gTextureId8, Fish1);
	renderFish(scaleFish1, gTextureId8, Fish2);
	renderFish(scaleFish3, gTextureId8, Fish3);
	renderPyramid(rotation, driftwoodTranslation, scaleDriftWood, gTextureId9);

	// Deactivate the Vertex Array Object and shader program
	glBindVertexArray(0);
	glUseProgram(0);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Implements the UCreateMesh function
void UCreateMesh(GLMesh &mesh)
{
	// float to use on the scaling pyramid texture
	const float REPEATP = 0.25;
	// float to use on scaling of cube textures
	const float REPEATCB = 0.25;
	// float to use scaling cylinder textures
	const float REPEATCY1 = 0.5;
	const float REPEATCY2 = 0.48;
	const float REPEATCY3 = 0.54;
	const float CYSIDE1 = 0.5;
	const float CYSIDE2 = 0.55;

	const float CARPET = 1;
	// float to use on the scaling
	const float REPEAT = 0.25;

	const float REPEATFISH = 0.4;
	// Position and Color data
	GLfloat verts[] = {
		//Positions          //Normals
		// ------------------------------------------------------
			// Vertex Positions		//Normals			// Texture Coords	
		// front side
		-0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	0.0f, REPEATCB,	// Top left front corner vertex 0		using 1 color per triangle
		-0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,	// Bottom left front corner vertex 1
		 0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCB, REPEATCB,	// Top right front corner vertex 2
		-0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,	// Bottom left front corner vertex 1
		 0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCB, REPEATCB,	// Top right front corner vertex 2
		 0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCB, 0.0f,	// Bottom right front corner vertex 3
		 // left side
		-0.5f,  0.5f,  0.5f,		-1.0f, 0.0f, 0.0f,	REPEATCB, REPEATCB,	// Top left front corner vertex 0
		-0.5f, -0.5f,  0.5f,		-1.0f, 0.0f, 0.0f,	REPEATCB, 0.0f,	// Bottom left front corner vertex 1
		-0.5f,  0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,	0.0f, REPEATCB,	// Top left back corner vertex 4
		-0.5f, -0.5f,  0.5f,		-1.0f, 0.0f, 0.0f,	REPEATCB, 0.0f,	// Bottom left front corner vertex 1
		-0.5f,  0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,	0.0f, REPEATCB,	// Top left back corner vertex 4
		-0.5f, -0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,		// Bottom left back corner vertex 5
		// right side
		 0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	0.0f, REPEATCB,	// Top right front corner vertex 2
		 0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,	// Bottom right front corner vertex 3
		 0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	REPEATCB, REPEATCB,	// Top right back corner vertex 6
		 0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,	// Bottom right front corner vertex 3
		 0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	REPEATCB, REPEATCB,	// Top right back corner vertex 6
		 0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	REPEATCB, 0.0f,	// Bottom right back corner vertex 7
		 // back
		-0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	0.0f, REPEATCB,	// Top left back corner vertex 4
		-0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,	// Bottom left back corner vertex 5
		 0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCB, REPEATCB,	// Top right back corner vertex 6
		-0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,	// Bottom left back corner vertex 5
		 0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCB, REPEATCB,	// Top right back corner vertex 6
		 0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCB, 0.0f,	// Bottom right back corner vertex 7
		 // top
		-0.5f,  0.5f,  0.5f,		1.0f, 1.0f, 1.0f,	0.0f, 0.0f,	// Top left front corner vertex 0
		 0.5f,  0.5f,  0.5f,		1.0f, 1.0f, 1.0f,	REPEATCB, 0.0f,	// Top right front corner vertex 2
		-0.5f,  0.5f, -0.5f,		1.0f, 1.0f, 1.0f,	0.0f, REPEATCB,	// Top left back corner vertex 4
		 0.5f,  0.5f,  0.5f,		1.0f, 1.0f, 1.0f,	REPEATCB, 0.0f,	// Top right front corner vertex 2
		-0.5f,  0.5f, -0.5f,		1.0f, 1.0f, 1.0f,	0.0f, REPEATCB,	// Top left back corner vertex 4
		 0.5f,  0.5f, -0.5f,		1.0f, 1.0f, 1.0f,	REPEATCB, REPEATCB,	// Top right back corner vertex 6
		 // bottom
		-0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 0.0f,	// Bottom left front corner vertex 1
		 0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,	REPEATCB, 0.0f,	// Bottom right front corner vertex 3
		 0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	REPEATCB, REPEATCB,	// Bottom right back corner vertex 7
		-0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 0.0f,	// Bottom left front corner vertex 1
		 0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	REPEATCB, REPEATCB,	// Bottom right back corner vertex 7
		-0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	0.0f, REPEATCB,	// Bottom left back corner vertex 5

		// Cylinder		I used a 13-sided figure to approximate a circle for the top and bottom
		// top
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		 0.0f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Top of circle vertex 1t of 13
		-0.2f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3, // Upper left circle vertex 2t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		-0.2f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3, // Upper left circle vertex 2t of 13
		-0.4f,  0.3f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Upper left circle vertex 3t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		-0.4f,  0.3f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Upper left circle vertex 3t of 13
		-0.5f,  0.1f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Upper left circle vertex 4t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		-0.5f,  0.1f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Upper left circle vertex 4t of 13
		-0.5f, -0.2f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower left circle vertex 5t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		-0.5f, -0.2f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower left circle vertex 5t of 13
		-0.4f, -0.4f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower left circle vertex 6t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		-0.4f, -0.4f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower left circle vertex 6t of 13
		-0.1f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower middle circle vertex 7t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		-0.1f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower middle circle vertex 7t of 13
		 0.1f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower middle circle vertex 8t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		 0.1f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower middle circle vertex 8t of 13
		 0.3f, -0.4f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower right circle vertex 9t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		 0.3f, -0.4f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower right circle vertex 9t of 13
		 0.5f, -0.2f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower right circle vertex 10t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		 0.5f, -0.2f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Lower right circle vertex 10t of 13
		 0.5f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Center right circle vertex 11t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		 0.5f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Center right circle vertex 11t of 13
		 0.4f,  0.3f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Upper right circle vertex 12t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		 0.4f,  0.3f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Upper right circle vertex 12t of 13
		 0.3f,  0.4f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Upper right circle vertex 13t of 13
		 0.0f,  0.0f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0t
		 0.3f,  0.4f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Upper right circle vertex 13t of 13
		 0.0f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	REPEATCY2, REPEATCY3,	// Top of circle vertex 1t of 13
		// bottom
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		 0.0f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Top of circle vertex 1b of 13
		-0.2f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3, // Upper left circle vertex 2b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		-0.2f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3, // Upper left circle vertex 2b of 13
		-0.4f,  0.3f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Upper left circle vertex 3b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		-0.4f,	0.3f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Upper left circle vertex 3b of 13
		-0.5f,  0.1f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Upper left circle vertex 4b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		-0.5f,  0.1f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Upper left circle vertex 4b of 13
		-0.5f, -0.2f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower left circle vertex 5b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		-0.5f, -0.2f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower left circle vertex 5b of 13
		-0.4f, -0.4f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower left circle vertex 6b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		-0.4f, -0.4f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower left circle vertex 6b of 13
		-0.1f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower middle circle vertex 7b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		-0.1f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower middle circle vertex 7b of 13
		 0.1f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower middle circle vertex 8b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		 0.1f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower middle circle vertex 8b of 13
		 0.3f, -0.4f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower right circle vertex 9b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		 0.3f, -0.4f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower right circle vertex 9b of 13
		 0.5f, -0.2f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower right circle vertex 10b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		 0.5f, -0.2f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Lower right circle vertex 10b of 13
		 0.5f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Center right circle vertex 11b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		 0.5f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Center right circle vertex 11b of 13
		 0.4f,  0.3f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Upper right circle vertex 12b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		 0.4f,  0.3f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Upper right circle vertex 12b of 13
		 0.3f,  0.4f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Upper right circle vertex 13b of 13
		 0.0f,  0.0f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY1, REPEATCY1,	// Center of the circle vertex 0b
		 0.3f,  0.4f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Upper right circle vertex 13b of 13
		 0.0f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	REPEATCY2, REPEATCY3,	// Top of circle vertex 1b of 13
		// sides
		// sides will require 2 triangles each for a total of 78 vertices - adjust the normals on these - only using 0.0 for testing
		 0.0f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Top of circle vertex 1t of 13
		-0.2f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2, // Upper left circle vertex 2t of 13
		 0.0f,  0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Top of circle vertex 1b of 13
		 0.0f,  0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Top of circle vertex 1b of 13
		-0.2f,  0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2, // Upper left circle vertex 2b of 13
		-0.2f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2, // Upper left circle vertex 2t of 13
		-0.2f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2, // Upper left circle vertex 2t of 13
		-0.4f,  0.3f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 3t of 13
		-0.2f,  0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2, // Upper left circle vertex 2b of 13
		-0.2f,  0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2, // Upper left circle vertex 2b of 13
		-0.4f,  0.3f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 3b of 13
		-0.4f,  0.3f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 3t of 13
		-0.4f,  0.3f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 3t of 13
		-0.5f,  0.1f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 4t of 13
		-0.4f,  0.3f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 3b of 13
		-0.4f,  0.3f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 3b of 13
		-0.5f,  0.1f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 4b of 13
		-0.5f,  0.1f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 4t of 13
		-0.5f,  0.1f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 4t of 13
		-0.5f, -0.2f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 5t of 13
		-0.5f,  0.1f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 4b of 13
		-0.5f,  0.1f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper left circle vertex 4b of 13
		-0.5f, -0.2f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 5b of 13
		-0.5f, -0.2f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 5t of 13
		-0.5f, -0.2f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 5t of 13
		-0.4f, -0.4f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 6t of 13
		-0.5f, -0.2f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 5b of 13
		-0.5f, -0.2f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 5b of 13
		-0.4f, -0.4f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 6b of 13
		-0.4f, -0.4f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 6t of 13
		-0.4f, -0.4f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 6t of 13
		-0.1f, -0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 7t of 13
		-0.4f, -0.4f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 6b of 13
		-0.4f, -0.4f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower left circle vertex 6b of 13
		-0.1f, -0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 7b of 13
		-0.1f, -0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 7t of 13
		-0.1f, -0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 7t of 13
		 0.1f, -0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 8t of 13
		-0.1f, -0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 7b of 13
		-0.1f, -0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 7b of 13
		 0.1f, -0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 8b of 13
		 0.1f, -0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 8t of 13
		 0.1f, -0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 8t of 13
		 0.3f, -0.4f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 9t of 13
		 0.1f, -0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 8b of 13
		 0.1f, -0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower middle circle vertex 8b of 13
		 0.3f, -0.4f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 9b of 13
		 0.3f, -0.4f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 9t of 13
		 0.3f, -0.4f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 9t of 13
		 0.5f, -0.2f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 10t of 13
		 0.3f, -0.4f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 9b of 13
		 0.3f, -0.4f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 9b of 13
		 0.5f, -0.2f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 10b of 13
		 0.5f, -0.2f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 10t of 13
		 0.5f, -0.2f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 10t of 13
		 0.5f,  0.0f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Center right circle vertex 11t of 13
		 0.5f, -0.2f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 10b of 13
		 0.5f, -0.2f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Lower right circle vertex 10b of 13
		 0.5f,  0.0f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Center right circle vertex 11b of 13
		 0.5f,  0.0f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Center right circle vertex 11t of 13
		 0.5f,	0.0f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Center right circle vertex 11t of 13
		 0.4f,  0.3f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 12t of 13
		 0.5f,  0.0f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Center right circle vertex 11b of 13
		 0.5f,  0.0f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Center right circle vertex 11b of 13
		 0.4f,  0.3f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 12b of 13
		 0.4f,  0.3f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 12t of 13
		 0.4f,  0.3f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 12t of 13
		 0.3f,  0.4f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 13t of 13
		 0.4f,  0.3f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 12b of 13
		 0.4f,  0.3f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 12b of 13
		 0.3f,  0.4f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 13b of 13
		 0.3f,  0.4f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 13t of 13
		 0.3f,  0.4f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 13t of 13
		 0.0f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Top of circle vertex 1t of 13
		 0.3f,  0.4f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 13b of 13
		 0.3f,  0.4f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Upper right circle vertex 13b of 13
		 0.0f,  0.5f, -0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Top of circle vertex 1b of 13
		 0.0f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,	CYSIDE1, CYSIDE2,	// Top of circle vertex 1t of 13

		// plane	-- followed the instructions in the video this week but made it 3D
		//top
		-5.0f, -5.0f, -5.0f,		0.0f, 1.0f, 0.0f,	0.0f, CARPET,	//v1 b-t-l
		-5.0f, -5.0f,  5.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f, //v2 f-t-l
		 5.0f, -5.0f, -5.0f,		0.0f, 1.0f, 0.0f,	CARPET, CARPET, //v3 b-t-r
		-5.0f, -5.0f,  5.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f, //v2 f-t-l
		 5.0f, -5.0f, -5.0f,		0.0f, 1.0f, 0.0f,	CARPET, CARPET, //v3 b-t-r
		 5.0f, -5.0f,  5.0f,		0.0f, 1.0f, 0.0f,	CARPET, 0.0f, //v4 f-t-r
		//bottom
		-5.0f, -5.1f, -5.0f,		0.0f, -1.0f, 0.0f,	0.0f, CARPET,	//v5 b-b-l
		-5.0f, -5.1f,  5.0f,		0.0f, -1.0f, 0.0f,	0.0f, 0.0f,	//v6 f-b-l
		 5.0f, -5.1f, -5.0f,		0.0f, -1.0f, 0.0f,	CARPET, CARPET,	//v7 b-b-r
		-5.0f, -5.1f,  5.0f,		0.0f, -1.0f, 0.0f,	0.0f, 0.0f,	//v6 f-b-lsnip
		 5.0f, -5.1f, -5.0f,		0.0f, -1.0f, 0.0f,	CARPET, CARPET,	//v7 b-b-r
		 5.0f, -5.1f,  5.0f,		0.0f, -1.0f, 0.0f,	CARPET, 0.0f, //v8 f-b-r
		//left
		-5.0f, -5.0f, -5.0f,		-1.0f, 0.0f, 0.0f,	0.0f, CARPET,	//v1 b-t-l
		-5.0f, -5.0f,  5.0f,		-1.0f, 0.0f, 0.0f,	CARPET, 0.0f, //v2 f-t-l
		-5.0f, -5.1f, -5.0f,		-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,	//v5 b-b-l
		-5.0f, -5.0f,  5.0f,		-1.0f, 0.0f, 0.0f,	CARPET, CARPET, //v2 f-t-l
		-5.0f, -5.1f, -5.0f,		-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,	//v5 b-b-l
		-5.0f, -5.1f,  5.0f,		-1.0f, 0.0f, 0.0f,	CARPET, 0.0f,	//v6 f-b-l
		//right
		 5.0f, -5.0f, -5.0f,		1.0f, 0.0f, 0.0f,	CARPET, CARPET, //v3 b-t-r
		 5.0f, -5.0f,  5.0f,		1.0f, 0.0f, 0.0f,	0.0f, CARPET, //v4 f-t-r
		 5.0f, -5.1f, -5.0f,		1.0f, 0.0f, 0.0f,	CARPET, 0.0f,	//v7 b-b-r
		 5.0f, -5.0f,  5.0f,		1.0f, 0.0f, 0.0f,	0.0f, CARPET, //v4 f-t-r
		 5.0f, -5.1f, -5.0f,		1.0f, 0.0f, 0.0f,	CARPET, 0.0f,	//v7 b-b-r
		 5.0f, -5.1f,  5.0f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f, //v8 f-b-r
		//back
		-5.0f, -5.0f, -5.0f,		0.0f, 0.0f, -1.0f,	0.0f, CARPET,	//v1 b-t-l
		 5.0f, -5.0f, -5.0f,		0.0f, 0.0f, -1.0f,	CARPET, CARPET, //v3 b-t-r
		-5.0f, -5.1f, -5.0f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,	//v5 b-b-l
		 5.0f, -5.0f, -5.0f,		0.0f, 0.0f, -1.0f,	CARPET, CARPET, //v3 b-t-r
		-5.0f, -5.1f, -5.0f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,	//v5 b-b-l
		 5.0f, -5.1f, -5.0f,		0.0f, 0.0f, -1.0f,	CARPET, 0.0f,	//v7 b-b-r
		//front
		-5.0f, -5.0f, 5.0f,			0.0f, 0.0f, 1.0f,	0.0f, CARPET, //v2 f-t-l
		 5.0f, -5.0f, 5.0f,			0.0f, 0.0f, 1.0f,	CARPET, CARPET, //v4 f-t-r
		-5.0f, -5.1f, 5.0f,			0.0f, 0.0f, 1.0f,	0.0f, 0.0f,	//v6 f-b-l
		 5.0f, -5.0f, 5.0f,			0.0f, 0.0f, 1.0f,	CARPET, CARPET, //v4 f-t-r
		-5.0f, -5.1f, 5.0f,			0.0f, 0.0f, 1.0f,	0.0f, 0.0f,	//v6 f-b-l
		 5.0f, -5.1f, 5.0f,			0.0f, 0.0f, 1.0f,	CARPET, 0.0f, //v8 f-b-r

		//FISH
		/* I had a really hard time with the texturing of the fish. The system would not take my coordinates for some reason
		if I had more time, I would have loved to put in a turtle and to make the fish more proper, but since it failed so many
		times in a row I decided it was best to just obtain an image of fish scales and use that for everything. I tried 3 different
		fish images and mapped out the coordinates 4-5 times with no success. I'll actually keep at it after this class, though.
		Graphics has been a fascinating topic to dive into.
		*/
		// front-facing side
		-0.2f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,	// top of dorsal fin v1
		-0.4f, 0.8f, 0.0f,		0.0f, 0.0f, 1.0f,	0.4f, 0.4f,	// back bottom of dorsal fin v2
		 0.4f, 0.8f, 0.0f,		0.0f, 0.0f, 1.0f,	0.5f, 0.0f,	// front bottom of dorsal fin v3
		-0.4f, 0.8f, 0.0f,		0.0f, 0.0f, 1.0f,	0.4f, 0.4f,	// back bottom of dorsal fin v2
		-0.6f, 0.65f, 0.02f,	0.0f, 0.0f, 1.0f,	0.2f, 0.5f,	// top base of fin v4
		-0.6f, 0.6f, 0.02f,		0.0f, 0.0f, 1.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.4f, 0.55f, 0.02f,	0.0f, 0.0f, 1.0f,	0.2f, 0.6f,	// bottom back of fish v6
		-0.4f, 0.8f, 0.0f,		0.0f, 0.0f, 1.0f,	0.4f, 0.4f,	// back bottom of dorsal fin v2
		-0.6f, 0.6f, 0.02f,		0.0f, 0.0f, 1.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.4f, 0.55f, 0.02f,	0.0f, 0.0f, 1.0f,	0.2f, 0.6f,	// bottom back of fish v6
		 0.4f, 0.8f, 0.0f,		0.0f, 0.0f, 1.0f,	0.5f, 0.0f,	// front bottom of dorsal fin v3
		-0.4f, 0.8f, 0.0f,		0.0f, 0.0f, 1.0f,	0.4f, 0.4f,	// back bottom of dorsal fin v2
		 0.4f, 0.55f, 0.02f,	0.0f, 0.0f, 1.0f,	0.6f, 0.5f, // bottom front of fish v7
		-0.4f, 0.55f, 0.02f,	0.0f, 0.0f, 1.0f,	0.2f, 0.6f,	// bottom back of fish v6
		 0.4f, 0.8f, 0.0f,		0.0f, 0.0f, 1.0f,	0.5f, 0.0f,	// front bottom of dorsal fin v3
		-0.6f, 0.65f, 0.02f,	0.0f, 0.0f, 1.0f,	0.2f, 0.5f,	// top base of fin v4
		-0.95f, 0.9f, 0.0f,		0.0f, 0.0f, 1.0f,	0.03f, 0.75f,//top front of tail v8
		-0.8f, 0.65f, 0.0f,		0.0f, 0.0f, 1.0f,	0.05f, 0.70f,//middle of tail v9
		-0.8f, 0.65f, 0.0f,		0.0f, 0.0f, 1.0f,	0.05f, 0.70f,//middle of tail v9
		-0.6f, 0.65f, 0.02f,	0.0f, 0.0f, 1.0f,	0.2f, 0.5f,	// top base of fin v4
		-0.6f, 0.6f, 0.02f,		0.0f, 0.0f, 1.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.8f, 0.65f, 0.0f,		0.0f, 0.0f, 1.0f,	0.05f, 0.70f,//middle of tail v9
		-0.6f, 0.6f, 0.02f,		0.0f, 0.0f, 1.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.9f, 0.3f, 0.0f,		0.0f, 0.0f, 1.0f,	0.05f, 0.4f,// bottom back of tail v10
		 0.4f, 0.55f, 0.02f,	0.0f, 0.0f, 1.0f,	0.6f, 0.5f, // bottom front of fish v7
		 0.55f, 0.675f, 0.02f,	0.0f, 0.0f, 1.0f,	0.65f, 0.53f,//bottom lip of fish face v11
		 0.4f, 0.675f, 0.02f,	0.0f, 0.0f, 1.0f,	0.6f, 0.52f,//bottom of fish jaw v12
		 0.4f, 0.675f, 0.02f,	0.0f, 0.0f, 1.0f,	0.6f, 0.52f,//bottom of fish jaw v12
		 0.4f, 0.68f, 0.02f,	0.0f, 0.0f, 1.0f,	0.6f, 0.52f,//top fish jaw v13
		 0.55f, 0.675f, 0.02f,	0.0f, 0.0f, 1.0f,	0.65f, 0.53f,//bottom lip of fish face v11
		 0.4f, 0.68f, 0.02f,	0.0f, 0.0f, 1.0f,	0.6f, 0.52f,//top fish jaw v13
		 0.4f, 0.7f, 0.02f,		0.0f, 0.0f, 1.0f,	0.6f, 0.54f,//top lip of fish v14
		 0.55f, 0.69f, 0.02f,	0.0f, 0.0f, 1.0f,	0.65f, 0.55f,//top lip furthest right v15
		 0.55f, 0.69f, 0.02f,	0.0f, 0.0f, 1.0f,	0.88f, 0.58f,//top lip furthest right v15
		 0.4f, 0.7f, 0.02f,		0.0f, 0.0f, 1.0f,	0.66f, 0.52f,//top lip of fish v14
		 0.4f, 0.8f, 0.0f,		0.0f, 0.0f, 1.0f,	0.5f, 0.0f,	// front bottom of dorsal fin v3
		//back side of fish
		-0.2f, 1.0f, 0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,	// top of dorsal fin v1
		-0.4f, 0.8f, 0.0f,		0.0f, 0.0f, -1.0f,	0.4f, 0.4f,	// back bottom of dorsal fin v2
		 0.4f, 0.8f, 0.0f,		0.0f, 0.0f, -1.0f,	0.5f, 0.0f,	// front bottom of dorsal fin v3
		-0.4f, 0.8f, 0.0f,		0.0f, 0.0f, -1.0f,	0.4f, 0.4f,	// back bottom of dorsal fin v2
		-0.6f, 0.65f, -0.02f,	0.0f, 0.0f, -1.0f,	0.2f, 0.5f,	// top base of fin v4
		-0.6f, 0.6f, -0.02f,	0.0f, 0.0f, -1.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.4f, 0.55f, -0.02f,	0.0f, 0.0f, -1.0f,	0.2f, 0.6f,	// bottom back of fish v6
		-0.4f, 0.8f, 0.0f,		0.0f, 0.0f, -1.0f,	0.4f, 0.4f,	// back bottom of dorsal fin v2
		-0.6f, 0.6f, -0.02f,	0.0f, 0.0f, -1.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.4f, 0.55f, -0.02f,	0.0f, 0.0f, -1.0f,	0.2f, 0.6f,	// bottom back of fish v6
		 0.4f, 0.8f, 0.0f,		0.0f, 0.0f, -1.0f,	0.5f, 0.0f,	// front bottom of dorsal fin v3
		-0.4f, 0.8f, 0.0f,		0.0f, 0.0f, -1.0f,	0.4f, 0.4f,	// back bottom of dorsal fin v2
		 0.4f, 0.55f, -0.02f,	0.0f, 0.0f, -1.0f,	0.6f, 0.5f, // bottom front of fish v7
		-0.4f, 0.55f, -0.02f,	0.0f, 0.0f, -1.0f,	0.2f, 0.6f,	// bottom back of fish v6
		 0.4f, 0.8f, 0.0f,		0.0f, 0.0f, -1.0f,	0.5f, 0.0f,	// front bottom of dorsal fin v3
		-0.6f, 0.65f, -0.02f,	0.0f, 0.0f, -1.0f,	0.2f, 0.5f,	// top base of fin v4
		-0.95f, 0.9f, 0.0f,		0.0f, 0.0f, -1.0f,	0.03f, 0.75f,//top front of tail v8
		-0.8f, 0.65f, 0.0f,		0.0f, 0.0f, -1.0f,	0.05f, 0.70f,//middle of tail v9
		-0.8f, 0.65f, 0.0f,		0.0f, 0.0f, -1.0f,	0.05f, 0.70f,//middle of tail v9
		-0.6f, 0.65f, -0.02f,	0.0f, 0.0f, -1.0f,	0.2f, 0.5f,	// top base of fin v4
		-0.6f, 0.6f, -0.02f,	0.0f, 0.0f, -1.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.8f, 0.65f, -0.0f,	0.0f, 0.0f, -1.0f,	0.05f, 0.70f,//middle of tail v9
		-0.6f, 0.6f, -0.02f,	0.0f, 0.0f, -1.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.9f, 0.3f, 0.0f,		0.0f, 0.0f, -1.0f,	0.05f, 0.4f,// bottom back of tail v10
		 0.4f, 0.55f, -0.02f,	0.0f, 0.0f, -1.0f,	0.6f, 0.5f, // bottom front of fish v7
		 0.55f, 0.675f, -0.02f,	0.0f, 0.0f, -1.0f,	0.65f, 0.53f,//bottom lip of fish face v11
		 0.4f, 0.675f, -0.02f,	0.0f, 0.0f, -1.0f,	0.6f, 0.52f,//bottom of fish jaw v12
		 0.4f, 0.675f, -0.02f,	0.0f, 0.0f, -1.0f,	0.6f, 0.52f,//bottom of fish jaw v12
		 0.4f, 0.68f, -0.02f,	0.0f, 0.0f, -1.0f,	0.6f, 0.52f,//top fish jaw v13
		 0.55f, 0.675f, -0.02f,	0.0f, 0.0f, -1.0f,	0.65f, 0.53f,//bottom lip of fish face v11
		 0.4f, 0.68f, -0.02f,	0.0f, 0.0f, -1.0f,	0.6f, 0.52f,//top fish jaw v13
		 0.4f, 0.7f, -0.02f,	0.0f, 0.0f, -1.0f,	0.6f, 0.54f,//top lip of fish v14
		 0.55f, 0.69f, -0.02f,	0.0f, 0.0f, -1.0f,	0.65f, 0.55f,//top lip furthest right v15
		 0.55f, 0.69f, -0.02f,	0.0f, 0.0f, -1.0f,	0.65f, 0.55f,//top lip furthest right v15
		 0.4f, 0.7f, -0.02f,	0.0f, 0.0f, -1.0f,	0.6f, 0.54f,//top lip of fish v14
		 0.4f, 0.8f, 0.0f,		0.0f, 0.0f, -1.0f,	0.5f, 0.0f,	// front bottom of dorsal fin v3
		//top and bottom of tail
			//top
		-0.95f, 0.9f, 0.0f,		0.0f, 1.0f, 0.0f,	0.03f, 0.75f,//top front of tail v8
		-0.6f, 0.65f, 0.02f,	0.0f, 1.0f, 0.0f,	0.2f, 0.5f,	// top base of fin v4
		-0.6f, 0.65f, -0.02f,	0.0f, 1.0f, 0.0f,	0.2f, 0.5f,	// top base of fin -v4
		-0.6f, 0.65f, 0.02f,	0.0f, 1.0f, 0.0f,	0.2f, 0.5f,	// top base of fin v4
		-0.6f, 0.65f, -0.02f,	0.0f, 1.0f, 0.0f,	0.2f, 0.5f,	// top base of fin -v4
		-0.4f, 0.8f, 0.0f,		0.0f, 1.0f, 0.0f,	0.4f, 0.4f,	// back bottom of dorsal fin v2
		 0.55f, 0.69f, -0.02f,	0.0f, 1.0f, 0.0f,	0.65f, 0.55f,//top lip furthest right v15
		 0.4f, 0.8f, 0.0f,		0.0f, 1.0f, 0.0f,	0.5f, 0.0f,	// front bottom of dorsal fin v3
		 0.55f, 0.69f, 0.02f,	0.0f, 1.0f, 0.0f,	0.65f, 0.55f,//top lip furthest right v15
			 //bottom
		-0.6f, 0.6f, -0.02f,	0.0f, -1.0f, 0.0f,	0.2f, 0.45f,// bottom base of fin -v5
		-0.6f, 0.6f, 0.02f,		0.0f, -1.0f, 0.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.9f, 0.3f, 0.0f,		0.0f, -1.0f, 0.0f,	0.05f, 0.4f,// bottom back of tail v10
		-0.6f, 0.6f, -0.02f,	0.0f, -1.0f, 0.0f,	0.2f, 0.45f,// bottom base of fin -v5
		-0.6f, 0.6f, 0.02f,		0.0f, -1.0f, 0.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.4f, 0.55f, -0.02f,	0.0f, -1.0f, 0.0f,	0.2f, 0.6f,	// bottom back of fish -v6
		-0.4f, 0.55f, -0.02f,	0.0f, -1.0f, 0.0f,	0.2f, 0.6f,	// bottom back of fish -v6
		-0.4f, 0.55f, 0.02f,	0.0f, -1.0f, 0.0f,	0.2f, 0.6f,	// bottom back of fish v6
		-0.6f, 0.6f, 0.02f,		0.0f, -1.0f, 0.0f,	0.2f, 0.45f,// bottom base of fin v5
		-0.4f, 0.55f, -0.02f,	0.0f, -1.0f, 0.0f,	0.2f, 0.6f,	// bottom back of fish -v6
		-0.4f, 0.55f, 0.02f,	0.0f, -1.0f, 0.0f,	0.2f, 0.6f,	// bottom back of fish -v6
		 0.4f, 0.55f, -0.02f,	0.0f, -1.0f, 0.0f,	0.6f, 0.5f, // bottom front of fish v7
		 0.4f, 0.55f, -0.02f,	0.0f, -1.0f, 0.0f,	0.6f, 0.5f, // bottom front of fish v7
		 0.4f, 0.55f, 0.02f,	0.0f, -1.0f, 0.0f,	0.6f, 0.5f, // bottom front of fish v7
		-0.4f, 0.55f, 0.02f,	0.0f, -1.0f, 0.0f,	0.2f, 0.6f,	// bottom back of fish v6
		 0.4f, 0.55f, -0.02f,	0.0f, -1.0f, 0.0f,	0.6f, 0.5f, // bottom front of fish v7
		 0.4f, 0.55f, 0.02f,	0.0f, -1.0f, 0.0f,	0.6f, 0.5f, // bottom front of fish v7
		 0.55f, 0.675f, -0.02f,	0.0f, -1.0f, 0.0f,	0.65f, 0.53f,//bottom lip of fish face v11
		 0.55f, 0.675f, 0.02f,	0.0f, -1.0f, 0.0f,	0.65f, 0.53f,//bottom lip of fish face v11
		 0.55f, 0.675f, -0.02f,	0.0f, -1.0f, 0.0f,	0.65f, 0.53f,//bottom lip of fish face v11
		 0.4f, 0.55f, 0.02f,	0.0f, -1.0f, 0.0f,	0.6f, 0.5f, // bottom front of fish v7
		 // fins
			// ventral fin
		-0.3f, 0.6f, 0.0f,		0.0f, 0.0f, 1.0f,	0.25f, 0.53f,// base of ventral fin
		-0.5f, 0.4f, 0.0f,		0.0f, 0.0f, 1.0f,	0.27f, 0.55f,// peak of ventral fin
		-0.5f, 0.55f, 0.0f,		0.0f, 0.0f, 1.0f,	0.23, 0.51f,// tail-base of ventral fin
			// side fins
		 0.1f, 0.7f, 0.01f,		0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// base of side fin
		-0.15f, 0.48f, 0.2f,	0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// back bottom side of fin
		-0.1f, 0.4f, 0.25f,		0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// furthest point of fin
		 0.1f, 0.7f, 0.01f,		0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// base of side fin
		-0.1f, 0.4f, 0.25f,		0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// furthest point of fin
		 0.13f, 0.7f, 0.01f,	0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// secondary point to widen base of fin
		 0.1f, 0.7f, -0.01f,	0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// base of side fin
		-0.15f, 0.48f, -0.2f,	0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// back bottom side of fin
		-0.1f, 0.4f, -0.25f,	0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// furthest point of fin
		 0.1f, 0.7f, -0.01f,	0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// base of side fin
		-0.1f, 0.4f, -0.25f,	0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// furthest point of fin
		 0.13f, 0.7f, -0.01f,	0.0f, 0.5f, 0.5f,	0.2f, 0.45f,// secondary point to widen base of fin

		/*OLD DATA FROM PYRAMID FOR THAT ASSIGNMENT - re-using here for driftwood*/
		//Bottom             //Negative Y Normal  Texture Coords.
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, REPEAT,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  REPEAT, REPEAT,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  REPEAT, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  REPEAT, REPEAT,

		 //Left Face         //Positive Z Normal
		-0.5f, -0.5f,  0.5f, -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  1.0f,  1.0f,  REPEAT, 0.0f,
		 0.0f,  0.5f,  0.0f, -1.0f,  1.0f,  1.0f,  0.125f, REPEAT,
		 //Right Face
		 0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  REPEAT, 0.0f,
		 0.0f,  0.5f,  0.0f,  1.0f,  1.0f,  1.0f,  0.125f, REPEAT,

		 //Front Face          //Negative X Normal
		-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  REPEAT, 0.0f,
		 0.0f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.125f, REPEAT,
		 //Back Face
		-0.5f, -0.5f, -0.5f, -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, -1.0f,  1.0f, -1.0f,  REPEAT, 0.0f,
		 0.0f,  0.5f,  0.0f, -1.0f,  1.0f, -1.0f,  0.125f, REPEAT,

		 //Right Face         //Positive X Normal
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 //Bottom Face        //Negative Y Normal
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		//Top Face           //Positive Y Normal
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

	glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vao);

	// Create 2 buffers: first one for the vertex data; second one for the indices
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	// Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}


void UDestroyMesh(GLMesh &mesh)
{
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(1, &mesh.vbo);
}


/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint &textureId)
{
	int width, height, channels;
	unsigned char *image = stbi_load(filename, &width, &height, &channels, 0);
	if (image)
	{
		flipImageVertically(image, width, height, channels);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return true;
	}

	// Error loading the image
	return false;
}


void UDestroyTexture(GLuint textureId)
{
	glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint &programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader
	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glUseProgram(programId);    // Uses the shader program

	return true;
}


void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}
