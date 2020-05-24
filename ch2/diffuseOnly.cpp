#include <iostream>
#include <string>

#define  GL_GLEXT_PROTOTYPES
#include "esUtil.h"
#include "glslprogram.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "vbotorus.h"

using namespace std;
string vertexShader = R"(
    #version 310 es
    precision mediump float;//给出默认浮点精度
    layout(location = 0) in highp vec3 VertexPosition;
    layout(location = 1) in highp vec3 VertexNormal;
    out vec3 LightIntensity;
    
    uniform vec4 LightPosition;// Light position in eye coord
    uniform vec3 Kd; // Diffuse reflectivity
    uniform vec3 Ld; // Light source intensity
    

    uniform mat4 ModelViewMatrix;
    uniform mat3 NormalMatrix;
    uniform mat4 ProjectionMatrix;
    uniform mat4 MVP; // Projection * ModelView

    void main()
    {
        // Convert normal and position to eye coords
        vec3 tnorm = normalize( NormalMatrix * VertexNormal);
        vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition,1.0);
        vec3 s = normalize(vec3(LightPosition - eyeCoords));
        
        // The diffuse shading equation
        LightIntensity = Ld * Kd * max( dot( s, tnorm ), 0.0 );

        gl_Position = MVP*vec4(VertexPosition,1.0);
    }
)";

string fragmentShder = R"(
    #version 310 es
    precision mediump float;//给出默认浮点精度
    in vec3 LightIntensity;
    layout (location = 0) out highp vec4 FragColor;
    
    void main() {
        FragColor = vec4(LightIntensity, 1.0);
    }
)";



struct  myContext : public ESContext
{
    void init();
    virtual ~myContext();
    virtual void onDraw() override;
    virtual void onShutdown() override;

    void setMatrices();
private:
    std::shared_ptr<VBOTorus> torus;
    GLSLProgram mProgram;

    mat4 model;
    mat4 view;
    mat4 projection;


};

myContext::~myContext()
{

}

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    // Convert GLenum parameters to strings
    //printf("%s:%s[%s](%d): %s\n", sourceStr, typeStr, severityStr, id, message);
    printf("(%d): %s\n", id, message);
}

void myContext::setMatrices()
{
    mat4 mv = view * model;
    mProgram.setUniform("ModelViewMatrix", mv);
    mProgram.setUniform("NormalMatrix",
        mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    mProgram.setUniform("MVP", projection * mv);
}


void myContext::init()
{
    //Compiling a shader
    torus = std::make_shared<VBOTorus>(0.7f, 0.3f, 30, 30);

    mProgram.compileShader(vertexShader, GLSLShader::VERTEX, "");
    mProgram.compileShader(fragmentShder, GLSLShader::FRAGMENT,"");
    mProgram.link();
    mProgram.validate();
    mProgram.use();

    glViewport(0, 0, 640, 480);
    glEnable(GL_DEPTH_TEST);

    model = mat4(1.0f);
    //model = glm::rotate(model, glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
    //model = glm::rotate(model, glm::radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
    view = glm::lookAt(vec3(0.0f, 0.0f, -4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    //view = mat4(1.0f);
    
    //projection = mat4(1.0f);
    projection = glm::perspective(45., 1., 1.0, 1000.);

    mProgram.setUniform("Kd", 0.9f, 0.5f, 0.3f);
    mProgram.setUniform("Ld", 1.0f, 1.0f, 1.0f);
    mProgram.setUniform("LightPosition", view * vec4(5.0f, 5.0f, 2.0f, 1.0f));

}

void myContext::onDraw()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    mProgram.use();

    setMatrices();
    torus->render();
}

void myContext::onShutdown()
{
    torus.reset();
}

std::shared_ptr<ESContext> esMain()
{
    std::shared_ptr<myContext>  context = std::make_shared<myContext>();

    esCreateWindow(context, "diffuse only", 640, 480, ES_WINDOW_RGB);

    context->init();
    //testContextNoSurf();
    //testGlm();
    //testglVer();
    return context;
}