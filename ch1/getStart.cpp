#include <iostream>

#define  GL_GLEXT_PROTOTYPES
#include "esUtil.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Triangle.h"


using namespace  std;
#define ARM_COMPUTE_UNUSED(var) (void)(var)

#define ARM_COMPUTE_ERROR_ON_MSG(cond, ...) \
    do                                      \
    {                                       \
        if(cond)                            \
        {                                   \
            printf(__VA_ARGS__); \
        }                                   \
    } while(0)

void testGlm()
{
    glm::vec4 position = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 5.0),
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0));

    glm::mat4 model(1.0f); // The identity matrix
    model = glm::rotate(model, 90.0f, glm::vec3(0.0f, 1.0f, 0.0));
    glm::mat4 mv = view * model;
    glm::vec4 transformed = mv * position;
}
void testContextNoSurf()
{
    EGLDisplay _display; /**< Underlying EGL Display. */
    EGLContext _context; /**< Underlying EGL Context. */
    
    EGLBoolean res;
    _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    ARM_COMPUTE_ERROR_ON_MSG(_display == EGL_NO_DISPLAY, "Failed to get display: 0x%x.", eglGetError());

    res = eglInitialize(_display, nullptr, nullptr);

    ARM_COMPUTE_ERROR_ON_MSG(res == EGL_FALSE, "Failed to initialize egl: 0x%x.", eglGetError());
    ARM_COMPUTE_UNUSED(res);

    const char *egl_extension_st = eglQueryString(_display, EGL_EXTENSIONS);
    cout << "extension:" << egl_extension_st << endl;

    ARM_COMPUTE_ERROR_ON_MSG((strstr(egl_extension_st, "EGL_KHR_create_context") == nullptr), "Failed to query EGL_KHR_create_context");
    ARM_COMPUTE_ERROR_ON_MSG((strstr(egl_extension_st, "EGL_KHR_surfaceless_context") == nullptr), "Failed to query EGL_KHR_surfaceless_context");
    ARM_COMPUTE_UNUSED(egl_extension_st);

    const EGLint config_attribs[] =
    {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
        EGL_NONE
    };
    EGLConfig cfg;
    EGLint    count;

    res = eglChooseConfig(_display, config_attribs, &cfg, 1, &count);

    ARM_COMPUTE_ERROR_ON_MSG(res == EGL_FALSE, "Failed to choose config: 0x%x.", eglGetError());
    ARM_COMPUTE_UNUSED(res);

    res = eglBindAPI(EGL_OPENGL_ES_API);

    ARM_COMPUTE_ERROR_ON_MSG(res == EGL_FALSE, "Failed to bind api: 0x%x.", eglGetError());

    const EGLint attribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    _context = eglCreateContext(_display,
        cfg,
        EGL_NO_CONTEXT,
        attribs);

    ARM_COMPUTE_ERROR_ON_MSG(_context == EGL_NO_CONTEXT, "Failed to create context: 0x%x.", eglGetError());
    ARM_COMPUTE_UNUSED(res);

    res = eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, _context);

    ARM_COMPUTE_ERROR_ON_MSG(res == EGL_FALSE, "Failed to make current: 0x%x.", eglGetError());
    ARM_COMPUTE_UNUSED(res);
    
}


void testglVer()
{
    const GLubyte *renderer = glGetString(GL_RENDERER);
    
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *version = glGetString(GL_VERSION);
    cout << "GL_RENDERER:" << renderer << endl;
    cout << "GL_VENDOR:" << GL_VENDOR << endl;
    cout << "GL_VERSION:" << GL_VERSION << endl;

    const GLubyte *glslVersion =
        glGetString(GL_SHADING_LANGUAGE_VERSION);
    cout << "GL_SHADING_LANGUAGE_VERSION:" << glslVersion << endl;

    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    cout << "major:" << major <<  ",minor:" << minor << endl;

    GLint   nExtension;
    glGetIntegerv(GL_NUM_EXTENSIONS, &nExtension);
    for (int i=0;i< nExtension;i++)
    {
        //cout << "extension " << i << ":" << glGetStringi(GL_EXTENSIONS, i) << endl;
    }
}

struct  myContext: public ESContext
{
    void init();
    virtual ~myContext();
    virtual void onDraw() override;
    virtual void onShutdown() override;
private:
    std::shared_ptr<Triangle> triangle;

};

myContext::~myContext()
{

}

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    // Convert GLenum parameters to strings
    //printf("%s:%s[%s](%d): %s\n", sourceStr, typeStr, severityStr, id, message);
    printf("(%d): %s\n",  id, message);
}

void myContext::init()
{
    //Compiling a shader
    triangle = std::make_shared<Triangle>();


    //Using GLM for mathematics
    testGlm();
    
    //Determining the GLSL and OpenGL version
    testglVer();

    //Getting a list of active vertex input attributes and locations
    triangle->showAttribInfo();

   //Getting a list of active uniform variables
    triangle->showUniformInfo();

    //Getting debug messages
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debugCallback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
        GL_DONT_CARE, 0, NULL, GL_TRUE);
}

void myContext::onDraw()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //triangle->drawSelf();

    //Sending data to a shader using verte attributes and vertex buffer objects
    triangle->drawSelfViaVao();
}

void myContext::onShutdown()
{
    triangle.reset();
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