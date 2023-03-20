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

using namespace std;

#define numVAOs 1

GLuint renderingProgram;
GLuint vao[numVAOs];

void printShaderLog(GLuint shader) {
    int len = 0;
    int chWrittn = 0;
    char *log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char *)malloc(len);
        glGetShaderInfoLog(shader, len, &chWrittn, log);
        cout << "Shader Info Log: " << log << endl;
        free(log);
    }
}

void printProgramLog(int prog) {
    int len = 0;
    int chWrittn = 0;
    char *log;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char *)malloc(len);
        glGetProgramInfoLog(prog, len, &chWrittn, log);
        cout << "Program Info Log: " << log << endl;
        free(log);
    }
}

bool checkOpenGLError() {
    bool foundError = false;
    int glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
        cout << "glError: " << glErr << endl;
        foundError = true;
        glErr = glGetError();
    }
    return foundError;
}

GLuint createShaderProgram(){
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;
    // 顶点着色器源码
    const char* vshaderSource =R"(
    #version 410
    void main(void) {
        gl_Position = vec4(0.0,0.0,0.0,1.0);
    }
)";
    // 片段着色器源码
    const char *fshaderSource =R"(
    #version 410
    out vec4 color;
    void main(void) {
        if(gl_FragCoord.x<295)
            color = vec4(0.0,1.0,0.0,1.0);
        else
            color=vec4(0.0,0.0,1.0,1.0);
    }
)";
    
    
    // 创建着色器代码对象
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // 创建着色器程序对象
    GLuint vfProgram = glCreateProgram();
    
    // 设置着色器源码
    glShaderSource(vShader,1,&vshaderSource,NULL);
    glShaderSource(fShader,1,&fshaderSource,NULL);
    
    // 编译着色器代码
    glCompileShader(vShader);
    checkOpenGLError();
    glGetShaderiv(vShader,GL_COMPILE_STATUS,&vertCompiled);
    if(vertCompiled!=1){
        cout << "vertex compilation failed" << endl;
        printShaderLog(vShader);
    }
    
    glCompileShader(fShader);
    checkOpenGLError();
    glGetShaderiv(fShader,GL_COMPILE_STATUS,&fragCompiled);
    if(fragCompiled!=1){
        cout << "fragment compilation failed" << endl;
        printShaderLog(fShader);
    }
    
    
    // 着色器程序添加着色器代码
    glAttachShader(vfProgram,vShader);
    glAttachShader(vfProgram,fShader);
    // 链接着色器程序
    glLinkProgram(vfProgram);
    checkOpenGLError();
    glGetProgramiv(vfProgram,GL_COMPILE_STATUS,&linked);
    if(linked!=1){
        cout << "link failed" << endl;
        printProgramLog(vfProgram);
    }
    
    
    return vfProgram;
}

void init(GLFWwindow* window){
    renderingProgram = createShaderProgram();
    // 创建顶点数组缓冲区
    glGenVertexArrays(numVAOs,vao);
    glBindVertexArray(vao[0]);
}
void display(GLFWwindow* window, double currentTime){
    glClear(GL_COLOR_BUFFER_BIT);
    // 运行着色器程序
    glUseProgram(renderingProgram);
    glPointSize(30.0f);
    // 绘制缓冲区内容
    glDrawArrays(GL_POINTS, 0, 1);
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
    
    // -获取窗口分辨率（针对视网膜屏MAC）
    int actualScreenWidth, actualScreenHeight;
    glfwGetFramebufferSize(window, &actualScreenWidth, &actualScreenHeight);
    
    // 绑定window和openGL上下文，创建窗口并不会自动绑定上下文
    glfwMakeContextCurrent(window);
    
    // -确保绘制到帧缓冲区的内容与窗口匹配（针对视网膜屏MAC）
    glViewport(0,0,actualScreenWidth, actualScreenHeight);
    glewExperimental = GL_TRUE;
    
    // 初始化glew
    if(glewInit()!=GLEW_OK){
        exit(EXIT_FAILURE);
    }
    // 开启垂直同步，GLFW窗口默认双缓冲
    glfwSwapInterval(1);
    
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
