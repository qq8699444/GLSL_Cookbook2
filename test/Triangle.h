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
        //���ó�ʼ���������ݵ�initVertexData����
        initVertexData();
        //���ó�ʼ����ɫ����intShader����
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
        //���ض�����ɫ���Ľű�����
        mVertexShader = ShaderUtil::loadFromAssetsFile("vertex.sh");
        //����ƬԪ��ɫ���Ľű�����
        mFragmentShader = ShaderUtil::loadFromAssetsFile("frag.sh");
        //���ڶ�����ɫ����ƬԪ��ɫ����������
        mProgram = ShaderUtil::createProgram(mVertexShader, mFragmentShader);
        
        //��ȡ�������ܱ任��������
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
        //ָ��ʹ��ĳ��shader����
        glUseProgram(mProgram);
        //��ʼ���任����
        //Matrix.setRotateM(mMMatrix, 0, 0, 0, 1, 0);
        //������Z������λ��1
        //Matrix.translateM(mMMatrix, 0, 0, 0, 1);
        //mMMatrix = glm::translate(glm::vec3(0.f, 0.f, 1.f));
        //������x����ת
        //glm::rotate(mMMatrix, 0, xAngle, 1, 0, 0);

        //���任��������Ⱦ����
        //glUniformMatrix4fv(muMVPMatrixHandle, 1, false, getFianlMatrix(mMMatrix));

        //��ȡ�����ж���λ����������  
        maPositionHandle = glGetAttribLocation(mProgram, "VertexPosition");
        //��ȡ�����ж�����ɫ��������
        maColorHandle = glGetAttribLocation(mProgram, "VertexColor");
        //
        //testVert();

        //������λ�����ݴ��ͽ���Ⱦ����
        glVertexAttribPointer(
            maPositionHandle,
            3,
            GL_FLOAT,
            false,
            0,
            vertices
        );
        //��������ɫ���ݴ��ͽ���Ⱦ����
        glVertexAttribPointer
        (
            maColorHandle,
            3,
            GL_FLOAT,
            false,
            0,
            colors
        );
        glEnableVertexAttribArray(maPositionHandle);//���ö���λ������
        glEnableVertexAttribArray(maColorHandle);//���ö�����ɫ����  
        //����������
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
    float vertices[9] = //������������
    {
        -0.8f, -0.8f, 0.0f,
        0.8f, -0.8f, 0.0f,
        0.0f, 0.8f, 0.0f
    };

    float colors[9] =//������ɫ����
    {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    //
    int mProgram;//�Զ�����Ⱦ���߳���id
    int muMVPMatrixHandle;//�ܱ任��������
    int maPositionHandle; //����λ����������
    int maColorHandle; //������ɫ��������
    std::string mVertexShader;//������ɫ������ű�
    std::string mFragmentShader;//ƬԪ��ɫ������ű�

    //

    GLuint positionBufferHandle;
    GLuint colorBufferHandle;
    GLuint vaoHandle;
public:
    glm::mat4 mProjMatrix;//4x4 ͶӰ����
    glm::mat4 mVMatrix;//�����λ�ó���Ĳ�������
    glm::mat4 mMVPMatrix;//��������õ��ܱ任����
    glm::mat4  mMMatrix;
};