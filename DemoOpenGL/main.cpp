//
//  main.cpp
//  DemoOpenGL
//
//  Created by yupenglei on 2023/3/20.
//

//#include <iostream>
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//    std::cout << "Hello, World!\n";
//    return 0;
//}

// 阻止警告信息生成
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"


// HomeBrew将GLEW安装为静态库
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Utils.h"

using namespace std;

#define numVAOs 1
#define numVBOs 2
Utils util = Utils();
float cameraX, cameraY, cameraZ;
float pyrLocX, pyrLocY, pyrLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variable allocation for display
GLuint mvLoc, projLoc, sampLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
glm::mat4 tMat, rMat;

GLuint brickTexture;

void setupVertices(void) {
    float pyramidPositions[54] = {
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,//前侧面
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        0.0f, 1.0f, 0.0f,//右侧面
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        0.0f, 1.0f, 0.0f,//后侧面
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,//左侧面
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,//底面1
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f//底面2
        };
    float textureCoordinates[36] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f,//前侧面
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f,//右侧面
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f,//后侧面
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f,//左侧面
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,//底面1
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f//底面2
    };
   
    // 创建缓冲区对象
    glGenBuffers(numVBOs, vbo);
    // 写入四棱锥
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);// 设为当前缓冲区
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);// 顶点数据写入缓冲区
    // 写入纹理
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
    
    // 创建顶点数组对象
    glGenVertexArrays(1, vao);
    // 设为当前顶点数组
    glBindVertexArray(vao[0]);
}

void init(GLFWwindow* window) {
    renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 4.0f;
    pyrLocX = 0.0f; pyrLocY = 0.0f; pyrLocZ = 0.0f;
    brickTexture = Utils::loadTexture("brick1.jpg");

    setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(renderingProgram);
    
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    sampLoc = glGetUniformLocation(renderingProgram,"samp");
    
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pyrLocX, pyrLocY, pyrLocZ));

    mMat = glm::rotate(mMat, -0.45f, glm::vec3(1.0f, 0.0f, 0.0f));
    mMat = glm::rotate(mMat,  0.61f, glm::vec3(0.0f, 1.0f, 0.0f));
    mMat = glm::rotate(mMat,  0.00f, glm::vec3(0.0f, 0.0f, 1.0f));
    
    mvMat = vMat * mMat;

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    
    glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    glUniform1i(sampLoc, 0);//绑定纹理，shader中不能使用binding布局
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 18);
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
    // -获取窗口分辨率（针对视网膜屏MAC）
    int actualScreenWidth, actualScreenHeight;
    glfwGetFramebufferSize(win, &actualScreenWidth, &actualScreenHeight);
    // -确保绘制到帧缓冲区的内容与窗口匹配（针对视网膜屏MAC）
    glViewport(0,0,actualScreenWidth, actualScreenHeight);
    
    aspect = (float)actualScreenWidth / (float)actualScreenHeight;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void){
    cout << "start main" << endl;
    // 初始化glfw
    if(!glfwInit()){
        exit(EXIT_FAILURE);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // -mac下openGL为4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    // -强制使用硬件支持的最新OpenGL版本，很多MAC默认使用老版本的OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(600, 600, "Hello OpenGL", NULL, NULL);
    
    // 绑定window和openGL上下文，创建窗口并不会自动绑定上下文
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    
    // 初始化glew
    if(glewInit()!=GLEW_OK){
        exit(EXIT_FAILURE);
    }
    // 开启垂直同步，GLFW窗口默认双缓冲
    glfwSwapInterval(1);
    
    window_size_callback(window, 0, 0);// 只有窗口变化才执行回调，这里先执行一次
    glfwSetWindowSizeCallback(window, window_size_callback);

    init(window);
    
    while (!glfwWindowShouldClose(window)) {
        // 自定义渲染函数
        display(window, glfwGetTime());
        // 绘制屏幕
        glfwSwapBuffers(window);
        // 处理窗口相关事件
        glfwPollEvents();
    }
    cout << "exit main" << endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
