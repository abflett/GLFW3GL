#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

static GLuint CompileShader(GLuint type, const std::string& source)
{
    GLuint id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* infoLog = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cerr << "Failed to compile, " 
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " error: "
                  << infoLog
                  << "\n";
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
        return -1;

    window = glfwCreateWindow(800, 600, "GLFW3GL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Error: glewInit()" << "\n";
    }

    std::cout << glGetString(GL_VERSION) << "\n";
    
    GLfloat positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW) ;

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);

    std::string vertexShader = R"(
        #version 330 core
        layout (location = 0) in vec4 position;

        void main()
        {
            gl_Position = position;
        }
    )";

    std::string fragmentShader = R"(
        #version 330 core
        layout (location = 0) out vec4 color;

        void main()
        {
            color = vec4(0.0, 1.0, 0.0, 1.0);
        }
    )";

    GLuint shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteBuffers(1, &buffer);
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}