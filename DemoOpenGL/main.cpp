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
float cubeLocX, cubeLocY, cubeLocZ;
float pyrLocX, pyrLocY, pyrLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variable allocation for display
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
glm::mat4 tMat, rMat;

stack<glm::mat4> mvStack;

void setupVertices(void) {
    float vertexPositions[108] = {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
        1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
        1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
    };
    float pyramidPositions[54] = {
        -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //front
        1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,    //right
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  //back
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  //left
        -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, //LF
        1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f  //RR
    };
   
    // 创建缓冲区对象
    glGenBuffers(numVBOs, vbo);
    // 写入立方体
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);// 设为当前缓冲区
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);// 顶点数据写入缓冲区
    // 写入四棱锥
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
    
    // 创建顶点数组对象
    glGenVertexArrays(1, vao);
    // 设为当前顶点数组
    glBindVertexArray(vao[0]);
}

void init(GLFWwindow* window) {
    renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 12.0f;
    cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
    pyrLocX = 2.0f; pyrLocY = 2.0f; pyrLocZ = 0.0f;
    setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(renderingProgram);
    
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mvStack.push(vMat);
    
    // -------------- 太阳-四棱锥
    mvStack.push(mvStack.top());
    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    mvStack.push(mvStack.top());
    mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 0.0, 0.0));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
    glBindBuffer(GL_ARRAY_BUFFER,vbo[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    mvStack.pop();
    // -------------- 地球-立方体
    mvStack.push(mvStack.top());
    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime)*4.0, 0.0f, cos((float)currentTime)*4.0));
    mvStack.push(mvStack.top());
    mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
    glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    mvStack.pop();
    
    // -------------- 月亮-小立方体
    mvStack.push(mvStack.top());
    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime)*2.0, cos((float)currentTime)*2.0));
    mvStack.push(mvStack.top());
    mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
    mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
    glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    mvStack.pop();
    
    // 每个物体都会留下一个mMat
    mvStack.pop();
    mvStack.pop();
    mvStack.pop();
    // pMat
    mvStack.pop();
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
