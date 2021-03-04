#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __cplusplus
extern "C"
{
#endif
#include <string.h>
#include <libavutil/avassert.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
}
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "render.h"
using namespace std;
// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// const char *vertexShaderSource = "#version 330 core\n"
//                                  "layout (location = 0) in vec3 aPos;\n"
//                                  "void main()\n"
//                                  "{\n"
//                                  "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//                                  "}\0";

// const char *fragmentShaderSource = "#version 330 core\n"
//                                    "out vec4 FragColor;\n"
//                                    "void main()\n"
//                                    "{\n"
//                                    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//                                    "}\0";

bool init = false;
static GLFWwindow *window;
static int shaderProgram;
static unsigned int EBO;
static unsigned int VBO, VAO;

int Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    //vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    char *vertexShaderSource = (char *)malloc(sizeof(char) * 10240);
    GetStringFromPath(vertexShaderSource, "/Users/zhumingwei/project/c++/flvparser/shader.vs");
    // strcpy(sp, a);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    free(vertexShaderSource);
    //fragment shader
    char *fragmentShaderSource = (char *)malloc(sizeof(char) * 10240);
    GetStringFromPath(fragmentShaderSource, "/Users/zhumingwei/project/c++/flvparser/shader.fs");
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    free(fragmentShaderSource);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 左下角
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // 左上角
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3};

    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //绑定
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

int GetStringFromPath(char *strings, const char *path)
{

    std::ifstream vShaderFile(path);
    try
    {
        // 打开文件
        vShaderFile.open(path);

        std::stringstream vShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vShaderFile.rdbuf();
        // 关闭文件处理器
        vShaderFile.close();

        const char *vertexCode;
        // 转换数据流到string
        string s = vShaderStream.str();
        vertexCode = s.c_str();
        std::strcpy(strings, vertexCode);
        return 0;
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    return NULL;
}

void RenderFrame(AVFrame *frame)
{
    if (!init)
    {
        if (Init() < 0)
        {
            std::cout << "init frame error " << std::endl;
        }
        else
        {
            init = true;
        }
    }

    //绑定纹理
    GLuint _planarTextureHandles[3]; //y u v
    glGenTextures(3, _planarTextureHandles);

    feedTextureWithImageData(_planarTextureHandles, frame);

    // render loop
    // -----------
    // while (!glfwWindowShouldClose(window))
    // {
    processInput(window);
    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //draw triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
    // break;
    // }
}

void feedTextureWithImageData(GLuint handlers[3], AVFrame *avframe)
{
    dfBindTexture(avframe->data[0], avframe->linesize[0], avframe->height, handlers[0]);
    dfBindTexture(avframe->data[1], avframe->linesize[1], avframe->height / 2, handlers[1]);
    dfBindTexture(avframe->data[2], avframe->linesize[2], avframe->height / 2, handlers[2]);

    glUniform1i(glGetUniformLocation(shaderProgram, "samplerY"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "samplerU"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "samplerV"), 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, handlers[0]);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, handlers[1]);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, handlers[2]);
}

void dfBindTexture(uint8_t *data, int width, int height, GLuint handler)
{
    glBindTexture(GL_TEXTURE_2D, handler);
    //设置放大和缩小时，纹理的过滤选项为：线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //设置纹理X,Y轴的纹理环绕选项为：边缘像素延伸AVPixelFormat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //加载图像数据到纹理，GL_LUMINANCE指明了图像数据的像素格式为只有亮度，虽然第三个和第七个参数都使用了GL_LUMINANCE，
    //但意义是不一样的，前者指明了纹理对象的颜色分量成分，后者指明了图像数据的像素格式
    //获得纹理对象后，其每个像素的r,g,b,a值都为相同，为加载图像的像素亮度，在这里就是YUV某一平面的分量值
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    //解绑
    glBindTexture(GL_TEXTURE_2D, 0);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void dispose_render()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}
