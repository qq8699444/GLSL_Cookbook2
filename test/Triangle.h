#pragma  once
#include <string>

#include "esUtil.h"
#include "glm/glm.hpp"
using glm::mat4;
using glm::vec3;
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderUtil.h"


class Triangle
{
public:
    Triangle()
    {
        //调用初始化顶点数据的initVertexData方法
        initVertexData();
        //调用初始化着色器的intShader方法
        initShader();

        //
        initVboBuffer();
        initVaoObject();
    }

    void initVertexData()
    {
        vCount = 3;

    }

    void showAttribInfo()
    {
        //glUseProgram(mProgram);
        GLint numAttribs;
        glGetProgramInterfaceiv(mProgram, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);
        std::cout << "numAttribs:" << numAttribs << std::endl;
        
        GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
        for (int i = 0 ; i < numAttribs; i++)
        {
            GLint results[3];
            glGetProgramResourceiv(mProgram, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

            GLint nameBufSize = results[0] + 1;
            char * name = new char[nameBufSize];
            glGetProgramResourceName(mProgram, GL_PROGRAM_INPUT, i, nameBufSize, nullptr, name);
            //printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
            printf("%-5d %s (%d)\n", results[2], name, (results[1]));
            //GL_FLOAT_VEC3
            delete[] name;
        }
    }


    void showUniformInfo()
    {
        GLint numUniforms = 0;
        glGetProgramInterfaceiv(mProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
        std::cout << "numUniforms:" << numUniforms << std::endl;

        GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };
        for (int i = 0; i < numUniforms; i++)
        {
            GLint results[4];
            glGetProgramResourceiv(mProgram, GL_UNIFORM, i, 4, properties, 4, NULL, results);
            if (results[3] != -1)
                continue; // Skip uniforms in blocks
            GLint nameBufSize = results[0] + 1;
            char * name = new char[nameBufSize];
            glGetProgramResourceName(mProgram, GL_UNIFORM, i, nameBufSize, NULL, name);
            printf("%-5d %s (%d)\n", results[2], name,
                (results[1]));
            delete[] name;
        }
    }
    void initShader()
    {
        //加载顶点着色器的脚本内容
        mVertexShader = ShaderUtil::loadFromAssetsFile("vertex.sh");
        //加载片元着色器的脚本内容
        mFragmentShader = ShaderUtil::loadFromAssetsFile("frag.sh");
        //基于顶点着色器与片元着色器创建程序
        mProgram = ShaderUtil::createProgram(mVertexShader, mFragmentShader);
        
        //获取程序中总变换矩阵引用
        //muMVPMatrixHandle = glGetUniformLocation(mProgram, "uMVPMatrix");
    }

    void initVboBuffer()
    {
        GLuint vboHandles[2];

        // Create and populate the buffer objects
        glGenBuffers(2, vboHandles);
        positionBufferHandle = vboHandles[0];
        colorBufferHandle = vboHandles[1];

        // Populate the position buffer
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle); checkGLError("glBindBuffer");
        glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_STATIC_DRAW); checkGLError("glBufferData");

        //Populate the color buffer
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle); checkGLError("glBindBuffer");
        glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colors,GL_STATIC_DRAW); checkGLError("glBufferData");


        //
        glBindBuffer(GL_ARRAY_BUFFER, 0); checkGLError("glBindBuffer 0");
    }


    void initVaoObject()
    {
        // Create and set-up the vertex array object
        glGenVertexArrays(1, &vaoHandle); checkGLError("glGenVertexArrays");
        glBindVertexArray(vaoHandle); checkGLError("glBindVertexArray");

        // Enable the vertex attribute arrays
        glEnableVertexAttribArray(0); // Vertex position
        checkGLError("glEnableVertexAttribArray");
        glEnableVertexAttribArray(1); // Vertex color
        checkGLError("glEnableVertexAttribArray");

        // Map index 0 to the position buffer
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle); checkGLError("glBindBuffer");
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); checkGLError("glVertexAttribPointer");
        // Map index 1 to the color buffer
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle); checkGLError("glBindBuffer");
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL); checkGLError("glVertexAttribPointer");
    }
    void drawSelf()
    {
        //指定使用某套shader程序
        glUseProgram(mProgram);
        //初始化变换矩阵
        //Matrix.setRotateM(mMMatrix, 0, 0, 0, 1, 0);
        //设置沿Z轴正向位移1
        //Matrix.translateM(mMMatrix, 0, 0, 0, 1);
        //mMMatrix = glm::translate(glm::vec3(0.f, 0.f, 1.f));
        //设置绕x轴旋转
        //glm::rotate(mMMatrix, 0, xAngle, 1, 0, 0);

        //将变换矩阵传入渲染管线
        //glUniformMatrix4fv(muMVPMatrixHandle, 1, false, getFianlMatrix(mMMatrix));

        //获取程序中顶点位置属性引用  
        maPositionHandle = glGetAttribLocation(mProgram, "VertexPosition");
        //获取程序中顶点颜色属性引用
        maColorHandle = glGetAttribLocation(mProgram, "VertexColor");
        //
        //testVert();

        //将顶点位置数据传送进渲染管线
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
            3,
            GL_FLOAT,
            false,
            0,
            colors
        );
        glEnableVertexAttribArray(maPositionHandle);//启用顶点位置数据
        glEnableVertexAttribArray(maColorHandle);//启用顶点着色数据  
        //绘制三角形
        glDrawArrays(GL_TRIANGLES, 0, vCount);
    }


    void drawSelfViaVao()
    {
        glUseProgram(mProgram); checkGLError("glUseProgram");

        auto m4 = mat4(1.0f);
        auto rAxis = vec3(0.0f, 0.0f, 1.0f);
        mat4 rotationMatrix = glm::rotate(m4,  45.0f, rAxis);

        GLuint location = glGetUniformLocation(mProgram, "RotationMatrix");
        if (location >= 0)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, &rotationMatrix[0][0]);
        }

        glBindVertexArray(vaoHandle); checkGLError("glBindVertexArray");


        glDrawArrays(GL_TRIANGLES, 0, vCount); checkGLError("glBindBuffer");
    }
#if 0
    float* getFianlMatrix(glm::mat4 spec)
    {
        glm::mat4 tmp;
        //Matrix.multiplyMM(mMVPMatrix, 0, mVMatrix, 0, spec, 0);
        tmp = mVMatrix * spec;

        //Matrix.multiplyMM(mMVPMatrix, 0, mProjMatrix, 0, mMVPMatrix, 0);
        mMVPMatrix = mProjMatrix * tmp;
        return glm::value_ptr(mMVPMatrix);
    }

    void testVert()
    {
        for (int i = 0; i < vCount; i++)
        {
            glm::vec4   vert(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2], 1.0f);
            glm::vec4   ret = mMVPMatrix * vert;
            float* pt = glm::value_ptr(ret);
            printf("%f %f %f %f\n", pt[0], pt[1], pt[2], pt[3]);
        }
    }
#endif
protected:
private:
    int vCount = 3;

    float UNIT_SIZE = 0.2f;
    float vertices[9] = //顶点坐标数组
    {
        -0.8f, -0.8f, 0.0f,
        0.8f, -0.8f, 0.0f,
        0.0f, 0.8f, 0.0f
    };

    float colors[9] =//顶点颜色数组
    {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    //
    int mProgram;//自定义渲染管线程序id
    int muMVPMatrixHandle;//总变换矩阵引用
    int maPositionHandle; //顶点位置属性引用
    int maColorHandle; //顶点颜色属性引用
    std::string mVertexShader;//顶点着色器代码脚本
    std::string mFragmentShader;//片元着色器代码脚本

    //

    GLuint positionBufferHandle;
    GLuint colorBufferHandle;
    GLuint vaoHandle;
public:
    glm::mat4 mProjMatrix;//4x4 投影矩阵
    glm::mat4 mVMatrix;//摄像机位置朝向的参数矩阵
    glm::mat4 mMVPMatrix;//最后起作用的总变换矩阵
    glm::mat4  mMMatrix;
};