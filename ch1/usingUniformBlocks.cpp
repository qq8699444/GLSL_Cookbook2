#include <iostream>
#include <string>

#define  GL_GLEXT_PROTOTYPES
#include "esUtil.h"
#include "ShaderUtil.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;
string vertexShader = R"(
    #version 310 es
    precision mediump float;//给出默认浮点精度
    layout(location = 0) in highp vec3 VertexPosition;
    layout(location = 1) in highp vec3 VertexTexCoord;
    out vec3 TexCoord;
    void main()
    {
        TexCoord = VertexTexCoord;
        gl_Position = vec4(VertexPosition, 1.0);
    }
)";

string fragmentShder = R"(
    #version 310 es
    precision mediump float;//给出默认浮点精度
    in highp vec3 TexCoord;
    layout (location = 0) out highp vec4 FragColor;
    layout (binding = 0) uniform BlobSettings {
        highp vec4 InnerColor;
        highp vec4 OuterColor;
        highp float RadiusInner;
        highp float RadiusOuter;
    };
    void main() {
        float dx = TexCoord.x - 0.0;
        float dy = TexCoord.y - 0.0;
        float dist = sqrt(dx * dx + dy * dy);
        FragColor =
        mix( InnerColor, OuterColor,
        smoothstep( RadiusInner, RadiusOuter, dist ));
    }
)";



class FuzzyCircle
{
public:
    FuzzyCircle();
    ~FuzzyCircle();

    void initShader();
    void drawSelf();
private:
    int mProgram;//自定义渲染管线程序id
    GLuint uboHandle;

    int maPositionHandle; //
    int maColorHandle;

    float vertices[12] = //
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.f, 0.0f,
        1.0f, 1.f, 0.0f
    };

    float texCoords[12] =//
    {
        -1.0f, -1.0f,
        1.0f,  -1.0f,
        -1.0f, 1.0f,
        1.0f,  1.0f
    };

};

FuzzyCircle::FuzzyCircle()
{
    initShader();
}

FuzzyCircle::~FuzzyCircle()
{
}

void FuzzyCircle::initShader()
{
    mProgram = ShaderUtil::createProgram(vertexShader, fragmentShder);

    GLuint blockIndex = glGetUniformBlockIndex(mProgram, "BlobSettings"); checkGLError("glGetUniformBlockIndex");
    cout << "blockIndex:" << blockIndex << endl;

    GLint blockSize;
    glGetActiveUniformBlockiv(mProgram, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize); checkGLError("glGetActiveUniformBlockiv");
    cout << "blockSize:" << blockSize << endl;

    GLubyte * blockBuffer;
    blockBuffer = (GLubyte *)malloc(blockSize);

    const GLchar *names[] = { "InnerColor", "OuterColor", "RadiusInner", "RadiusOuter" };
    GLuint indices[4];
    glGetUniformIndices(mProgram, 4, names, indices); checkGLError("glGetUniformIndices");
    cout << "blockSize:" << indices[0] << endl;

    GLint offset[4];
    glGetActiveUniformsiv(mProgram, 4, indices, GL_UNIFORM_OFFSET, offset); checkGLError("glGetActiveUniformsiv");

    GLfloat outerColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    GLfloat innerColor[] = { 1.0f, 1.0f, 0.75f, 1.0f };
    GLfloat innerRadius = 0.25f, outerRadius = 0.45f;

    memcpy(blockBuffer + offset[0], innerColor,  4 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[1], outerColor,  4 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[2], &innerRadius,   sizeof(GLfloat));
    memcpy(blockBuffer + offset[3], &outerRadius,   sizeof(GLfloat));

    glGenBuffers(1, &uboHandle); checkGLError("glGenBuffers");
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle); checkGLError("glBindBuffer");
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW); checkGLError("glBufferData");

    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle); checkGLError("glBindBufferBase");
}


void FuzzyCircle::drawSelf()
{
    glUseProgram(mProgram);

    maPositionHandle = glGetAttribLocation(mProgram, "VertexPosition");

    maColorHandle = glGetAttribLocation(mProgram, "VertexTexCoord");

    glVertexAttribPointer(
        maPositionHandle,
        3,
        GL_FLOAT,
        false,
        0,
        vertices
    );
    //将顶点颜色数据传送进渲染管线
    glVertexAttribPointer
    (
        maColorHandle,
        2,
        GL_FLOAT,
        false,
        0,
        texCoords
    );
    glEnableVertexAttribArray(maPositionHandle);//启用顶点位置数据
    glEnableVertexAttribArray(maColorHandle);//启用顶点着色数据  
    //绘制三角形
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

struct  myContext : public ESContext
{
    void init();
    virtual ~myContext();
    virtual void onDraw() override;
    virtual void onShutdown() override;
private:
    std::shared_ptr<FuzzyCircle> circle;

};

myContext::~myContext()
{

}

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    // Convert GLenum parameters to strings
    //printf("%s:%s[%s](%d): %s\n", sourceStr, typeStr, severityStr, id, message);
    printf("(%d): %s\n", id, message);
}

void myContext::init()
{
    //Compiling a shader
    circle = std::make_shared<FuzzyCircle>();


}

void myContext::onDraw()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    circle->drawSelf();
}

void myContext::onShutdown()
{
    circle.reset();
}

std::shared_ptr<ESContext> esMain()
{
    std::shared_ptr<myContext>  context = std::make_shared<myContext>();

    esCreateWindow(context, "Hello Triangle", 640, 480, ES_WINDOW_RGB);

    context->init();
    //testContextNoSurf();
    //testGlm();
    //testglVer();
    return context;
}