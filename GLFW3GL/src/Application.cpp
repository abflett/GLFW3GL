#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") << std::endl;
	std::cerr << "    type = 0x" << std::hex << type << std::dec << std::endl;
	std::cerr << "    severity = 0x" << std::hex << severity << std::dec << std::endl;
	std::cerr << "    message = " << message << std::endl;
	std::cerr << std::endl;
}

std::string ReadFile(const std::string& filepath)
{
	std::ifstream file(filepath);
	if (!file.is_open())
	{
		std::cerr << "Error: Failed to open file " << filepath << std::endl;
		return "";
	}

	std::stringstream ss;
	std::string line;
	while (getline(file, line))
	{
		ss << line << '\n';
	}

	return ss.str();
}

static GLuint CompileShader(GLuint type, const std::string& source)
{
	GLuint id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	GLint compileStatus;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE) {
		GLint logLength;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> log(logLength + 1);
		glGetShaderInfoLog(id, logLength, nullptr, log.data());

		// Print the compilation log
		std::cerr << "Failed to compile "
			<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader: "
			<< log.data()
			<< std::endl;

		glDeleteShader(id);
		return 0;
	}

	return id;
}

static GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLuint program = glCreateProgram();
	GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDetachShader(program, vs);
	glDetachShader(program, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		std::cerr << "Error: Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "GLFW3GL", NULL, NULL);
	if (!window)
	{
		std::cerr << "Error: Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);


	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Error: Failed to initialize GLEW" << std::endl;
		return -1;
	}


#ifdef _DEBUG
	std::cout << glGetString(GL_VERSION) << "\n";
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif

	GLfloat positions[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f
	};

	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glEnableVertexAttribArray(0);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	std::string vertexShaderSource = ReadFile("res/shaders/Basic.vert");
	std::string fragmentShaderSource = ReadFile("res/shaders/Basic.frag");

	GLuint shader = CreateShader(vertexShaderSource, fragmentShaderSource);
	glUseProgram(shader);

	int uniformLocation = glGetUniformLocation(shader, "u_color");
	glUniform4f(uniformLocation, 0.2f, 0.3f, 0.8f, 1.0f);

	GLfloat red = 0.00f;
	GLfloat r_increment = 0.001f;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(vao);

		if (red > 1.0f || red < 0.0f)
		{
			r_increment *= -1;
		}
		red += r_increment;
		glUniform4f(uniformLocation, red, 0.3f, 0.8f, 1.0f);

		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}