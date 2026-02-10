#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


int main()
{
	// Initialize GLFW
	glfwInit();



	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);



	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, 800, 800);



	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"} \n";

	// Create Vertex Buffer Object
	// Stores the vertices in the GPU memory more efficient than sending it one at a time
	// glGenBuffers generates the buffer and stores the reference ID in the variable passed
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// Bind the VBO to the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, VBO); 

	// binds the vertices to the currently bound GL_ARRAY_BUFFER (which is our VBO)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex shader

	// create shader object and get its referenceid from OpenGL, we pass in the type of shader we want to create which is GL_VERTEX_SHADER
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// The glShaderSource function takes the shader object to compile to as its first argument. The second argument specifies how many strings we're passing as source code
	// //which is only one. The third parameter is the actual source code of the vertex shader and we can leave the 4th parameter to NULL.
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

		
	// ================================================
	// Fragment shader
	// calculates the color of each fragment (pixel) that will be drawn on the screen

	// similar to vertex shader, we create a shader object for the fragment shader, we pass in the type of shader we want to create which is GL_FRAGMENT_SHADER
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// to check succcessful compilation after glCompileShader
	// define an int variable to check for success and a char array to store the info log in case of failure
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	//Then we check if compilation was successful with glGetShaderiv.If compilation failed, we should retrieve the error message with glGetShaderInfoLog and print the error message.
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// check after compiling the fragment shader as well
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// if no errors were found the vertex shader and fragment shader was compiled successfully 

	// now we need to link the shaders together to create a shader program that we can use for rendering
	// ===============================================
	// Shader program
	// shader program is the final linked version of the vertex and fragment shader that we can use for rendering

	// glCreateProgram creates a shader program and returns its reference ID which we store in the variable shaderProgram
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	// now we attach the previously compiled shaders to the program object then link using glLinkProgram
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors (similar to shader compilation)
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// the result is a program object that we can activate by calling glUseProgram with the reference ID of the shader program
	glUseProgram(shaderProgram);

	// every shader and rendering call after this will use this program object

	// finally we delete the fragment and vertex shader as we no longer need them anymore
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// up to this point we sent the input vertex data to the gpu and told the gpu how it should process the vertex data
	// within a vertex and fragment shader.

	// but OpenGL does not yet know how it should interpret the vertex data in memory and how it should connect
	// the vertex data to the vertex shader's attributes

	// the vertex buffer is stored linearly with position data stored as 32-bit floating point values
	// each position or vertex is 3 of those values (x, y z)
	// there is no space between the values 

	// ================ glVertexAttributePointer ===============
	// first parameter specifies which vertex attribute we want to configure. in the vertex shader, we 
	// specified the location of the position vertex attribute in the vertex shader with layout (location = 0). this sets the location
	// of the vertex attribute to 0

	// the second parameter specifies the size of the vertex attribute, the vertex attribute is a vec3 so it is composed of 3 values

	// the third argument specifies the type of data, which is GL_FLOAT

	// the fourth parameter specifies if we want the data to be normalized (not relevant)

	// the fifth argument is known as the "stride" and tells us the space between consecutive vertex attributes
	// since the next vertex attribute is 3 floats away that is the stride

	// the last argument is the offset of where the position data begins in the buffer (explored later)

	// 0. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 1. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 2. use our shader program when we want to render an object
	glUseProgram(shaderProgram);
	// 3. now draw the object 

	// ============ Vertex Array Object ===========
	// repeatign the process above is tedious 
	// VAO allows use to store these state configs into an object and bind the object ot restore the state
	// core OpenGl REQUIRES us to use VAO

	//a vertex array object stores:
	// 1. calls to glEnableVertexAttribArray or glDisableVertexAttribArray
	// 2. Vertex attribute configurations via glVertexAttribPointer
	// 3. Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// to use VAO we bind the VAO using glBindVertexArray
	// as soons as we want to draw an object, we simply bind the VAO with the preferred settings before drwaing
	// the object thats in it

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// EBO
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// ======= Element buffer object ======
	// if we wanted to draw a rectangle, it would need two triangles, but then it would mean we are drawing some points twice
	// EBO makes sure we only draw once








	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// whenever we call glClear() the color buffer gets filled with the color specified in glClearColor()
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);

		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}