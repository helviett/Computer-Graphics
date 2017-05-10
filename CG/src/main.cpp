#include <GL/glew.h>
#include <Magick++.h>
#include "../include/Shader.h"
#include "../include/ShaderProgram.h"
#include "../include/Window.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include "../include/gl_math.h"
#include "../include/Camera.h"
#include "../include/KeyboardEventHandler.h"
#include "../include/CameraEventHandler.h"
#include "../include/MouseEventHandler.h"
#include "../include/Texture.h"
#include "../include/Light.h"
#include <vector>
#include <GL/glut.h>

#define ToRadian(x) (float)((x) * M_PI / 180.0f)
#define ToDegree(x) ((x) * 180.0f / M_PI)

struct Vertex
{
    Vec3f pos;
    Vec2f tex;
    Vec3f normal;

    Vertex() {}

    Vertex(const Vec3f &pos, const Vec2f &tex)
    {
        this->pos = pos;
        this->tex = tex;
        normal    = {0.0f, 0.0f, 0.0f};
    }
};

//GLuint CamWorldPos;
GLuint VAO;
GLuint VBO;
GLuint IBO;
GLuint Sampler;
GLuint DLColor;
GLuint DLIntens;
GLuint DLDiffuse;
GLuint DLDirection;
GLuint NumPLight;
GLuint CAMERA;
GLuint WVP;
GLuint WorldTrans;

Texture *Tex = NULL;
float Scale = 0.0f;

Matrix4 Projection, Translation, Rotattion, MScale;
Camera Cam;
float zMove = 0.0f;
KeyboardEventHandler KEH;
CameraEventHandler CEH;
MouseEventHandler MEH;
DirectionalLight dl({1.0f, 1.0f, 1.0f}, 1.0f, {1.0f, .0f, .0f}, 0.5f);
std::vector<PointLight> pls;
GLuint prog;
struct PLS
{
    GLuint Color, AmbientIntensity, Position, DiffuseIntensity, Constant, Linear, Exp;
}GPLS[10];

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

void AddPointLight()
{
    PointLight pl;
    pl.Color() = {1.0f, 1.0f, 1.0f};
    pl.Pos() = Cam.Position();
    pl.Constant() = .0f;
    pl.Linear() = 0.1f;
    pl.Exp() = 0.4f;
    pl.DiffuseIntensity() = 0.5f;
    pl.AmbientIntensity() = 0.1f;
    pls.push_back(pl);
}

void CalcNormals(const unsigned int* Indices, unsigned int IndexCount, Vertex* Vertices, unsigned int VertexCount)
{
    for (unsigned int i = 0 ; i < IndexCount ; i += 3) {
        unsigned int Index0 = Indices[i];
        unsigned int Index1 = Indices[i + 1];
        unsigned int Index2 = Indices[i + 2];
        Vec3f v1 = Vertices[Index1].pos - Vertices[Index0].pos;
        Vec3f v2 = Vertices[Index2].pos - Vertices[Index0].pos;
        Vec3f Normal = v1.Cross(v2);
        Normal.Normalize();

        Vertices[Index0].normal = Vertices[Index0].normal + Normal;
        Vertices[Index1].normal = Vertices[Index0].normal + Normal;
        Vertices[Index2].normal = Vertices[Index0].normal + Normal;
    }

    for (unsigned int i = 0 ; i < VertexCount ; i++) {
        Vertices[i].normal.Normalize();
        //std::cout << Vertices[i].normal[0] << "\t" << Vertices[i].normal[1] << "\t" << Vertices[i].normal[2] << std::endl;
        auto v = -dl.Direction() * Vertices[i].normal;
        std::cout << v << std::endl;
    }
}

void CreateVertexBuffer()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    Vertex vs[] = {

            Vertex({-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}),
            Vertex({-0.5f,  0.5f, -0.5f}, {0.0f, 0.5f}),
            Vertex({0.5f, -0.5f, -0.5f}, {0.5f, 0.0f}),
            Vertex({0.5f,  0.5f, -0.5f}, {0.5f, 0.5f}),

            Vertex({-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}),
            Vertex({-0.5f,  0.5f,  0.5f}, {0.0f, 0.5f}),
            Vertex({0.5f, -0.5f,  0.5f}, {0.5f, 0.0f}),
            Vertex({0.5f,  0.5f,  0.5f}, {0.5f, 0.5f}),

            Vertex({-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}),
            Vertex({-0.5f,  0.5f, -0.5f}, {0.0f, 0.5f}),
            Vertex({-0.5f, -0.5f,  0.5f}, {0.5f, 0.0f}),
            Vertex({-0.5f,  0.5f,  0.5f}, {0.5f, 0.5f}),

            Vertex({0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}),
            Vertex({0.5f,  0.5f, -0.5f}, {0.0f, 0.5f}),
            Vertex({0.5f, -0.5f,  0.5f}, {0.5f, 0.0f}),
            Vertex({0.5f,  0.5f,  0.5f}, {0.5f, 0.5f}),

            Vertex({-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}),
            Vertex({-0.5f, -0.5f,  0.5f}, {0.0f, 0.5f}),
            Vertex({0.5f, -0.5f, -0.5f}, {0.5f, 0.0f}),
            Vertex({0.5f, -0.5f,  0.5f}, {0.5f, 0.5f}),

            Vertex({-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}),
            Vertex({-0.5f,  0.5f,  0.5f}, {0.0f, 0.5f}),
            Vertex({0.5f,  0.5f, -0.5f}, {0.5f, 0.0f}),
            Vertex({0.5f,  0.5f,  0.5f}, {0.5f, 0.5f})
    };





    unsigned int Indices[] = { 1,  0,  2,
                               1,  2,  3,
                               4,  5,  6,
                               6,  5,  7,
                               8,  9, 10,
                               10, 9, 11,
                               13, 12, 14,
                               13, 14, 15,
                               17, 16, 18,
                               17, 18, 19,
                               21, 20, 22,
                               21, 22, 23 };
    CalcNormals(Indices, sizeof(Indices) / sizeof(Indices[0]), vs, sizeof(vs) / sizeof(vs[0]));
    for (int i = 0; i < 4 * 6; i++)
        std::cout << vs[i].normal[0] << '\t' << vs[i].normal[1] << '\t' << vs[i].normal[2] << '\t' << std::endl;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs), vs, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

    glBindVertexArray(VAO);
}

void CreateShaders()
{
    ShaderProgram sp;
    std::ifstream fragFin("../shaders/fragment/frag.glsl");
    std::ifstream vertFin("../shaders/vertex/vert.glsl");
    Shader vert(vertFin, GL_VERTEX_SHADER);
    Shader frag(fragFin, GL_FRAGMENT_SHADER);
    vert.compile();
    frag.compile();
    sp.attachShader(vert);
    sp.attachShader(frag);
    sp.link();
    if (sp.isValid())
        sp.use();
    else
        throw std::runtime_error("Shader program is not valid");
    Sampler = (GLuint) glGetUniformLocation(sp.program(), "Sampler");
    assert(Sampler != 0xFFFFFFFF);
    WVP = (GLuint)glGetUniformLocation(sp.program(), "WVP");
    assert(WVP != 0xFFFFFFFF);
    DLColor = (GLuint) glGetUniformLocation(sp.program(), "directionalLight.Base.Color");
    assert(DLColor!= 0xFFFFFFFF);
    DLIntens = (GLuint) glGetUniformLocation(sp.program(), "directionalLight.Base.AmbientIntensity");
    assert(DLIntens != 0xFFFFFFFF);
    DLDiffuse = (GLuint) glGetUniformLocation(sp.program(), "directionalLight.Base.DiffuseIntensity");
    assert(DLDiffuse != 0xFFFFFFFF);
    DLDirection = (GLuint) glGetUniformLocation(sp.program(), "directionalLight.Direction");
    assert(DLDirection != 0xFFFFFFFF);
    WorldTrans = (GLuint) glGetUniformLocation(sp.program(), "World");
    assert(WorldTrans != 0xFFFFFFFF);
    prog = sp.program();
}

Matrix4 GetRotationMatrix(float x, float y, float z)
{
    Matrix4 rx, ry, rz;

    x = ToRadian(x);
    y = ToRadian(y);
    z = ToRadian(z);

    rx[0][0] = 1.0f; rx[0][1] = 0.0f   ; rx[0][2] = 0.0f    ; rx[0][3] = 0.0f;
    rx[1][0] = 0.0f; rx[1][1] = cosf(x); rx[1][2] = -sinf(x); rx[1][3] = 0.0f;
    rx[2][0] = 0.0f; rx[2][1] = sinf(x); rx[2][2] = cosf(x) ; rx[2][3] = 0.0f;
    rx[3][0] = 0.0f; rx[3][1] = 0.0f   ; rx[3][2] = 0.0f    ; rx[3][3] = 1.0f;

    ry[0][0] = cosf(y); ry[0][1] = 0.0f; ry[0][2] = -sinf(y); ry[0][3] = 0.0f;
    ry[1][0] = 0.0f   ; ry[1][1] = 1.0f; ry[1][2] = 0.0f    ; ry[1][3] = 0.0f;
    ry[2][0] = sinf(y); ry[2][1] = 0.0f; ry[2][2] = cosf(y) ; ry[2][3] = 0.0f;
    ry[3][0] = 0.0f   ; ry[3][1] = 0.0f; ry[3][2] = 0.0f    ; ry[3][3] = 1.0f;

    rz[0][0] = cosf(z); rz[0][1] = -sinf(z); rz[0][2] = 0.0f; rz[0][3] = 0.0f;
    rz[1][0] = sinf(z); rz[1][1] = cosf(z) ; rz[1][2] = 0.0f; rz[1][3] = 0.0f;
    rz[2][0] = 0.0f   ; rz[2][1] = 0.0f    ; rz[2][2] = 1.0f; rz[2][3] = 0.0f;
    rz[3][0] = 0.0f   ; rz[3][1] = 0.0f    ; rz[3][2] = 0.0f; rz[3][3] = 1.0f;

    return rz * ry * rx;
}

Matrix4 Trans(float x, float y, float z)
{
    Matrix4 m;
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    return m;
}

void PrintLog()
{
    auto pos = Cam.Position();
    std:: cout << pos[0] << "\t" << pos[1] << "\t" << pos[2] << "\t" << std::endl;
}

void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(VAO);

    Vec3f p = CEH.GetCameraPosition(KEH.keys(), Cam.Target(), Cam.Up(), Cam.Position());
    Matrix4 res = Cam.GetProjectionPerspectiveMatrix()  * Cam.GetUVNMatrix() * Cam.SetPosition(p[0], p[1], p[2]);
    Matrix4 world = EyeMatrix4();

    glUniformMatrix4fv(WVP, 1, GL_TRUE, (GLfloat *)&res);
    glUniformMatrix4fv(WorldTrans, 1, GL_TRUE, (GLfloat *)&world);
    glUniform3f(DLColor, dl.Color()[0], dl.Color()[1], dl.Color()[2]);
    Vec3f Direction = dl.Direction();
    Direction.Normalize();
    glUniform3f(DLDirection, Direction[0], Direction[1], Direction[2]);
    glUniform1f(DLDiffuse, dl.DiffuseIntensity());
    glUniform1f(DLIntens, dl.AmbientIntensity());
    glUniform1i(Sampler, 0);
    p = Cam.Position();
    if (pls.size() > 0)
    {
        CAMERA = (GLuint) glGetUniformLocation(prog, "Cam");
        glUniform3f(CAMERA, p[0], p[1], p[2]);
        for (int i = 0; i < pls.size(); i++)
        {
            char Name[128];
            snprintf(Name, sizeof(Name), "PointLights[%d].Base.Color", i);
            GPLS[i].Color = glGetUniformLocation(prog, Name);
            snprintf(Name, sizeof(Name), "PointLights[%d].Base.AmbientIntensity", i);
            GPLS[i].AmbientIntensity= glGetUniformLocation(prog, Name);
            snprintf(Name, sizeof(Name), "PointLights[%d].Base.DiffuseIntensity", i);
            GPLS[i].DiffuseIntensity = glGetUniformLocation(prog, Name);
            snprintf(Name, sizeof(Name), "PointLights[%d].Position", i);
            GPLS[i].Position = glGetUniformLocation(prog, Name);
            snprintf(Name, sizeof(Name), "PointLights[%d].Atten.Constant", i);
            GPLS[i].Constant = glGetUniformLocation(prog, Name);
            snprintf(Name, sizeof(Name), "PointLights[%d].Atten.Linear", i);
            GPLS[i].Linear = glGetUniformLocation(prog, Name);
            snprintf(Name, sizeof(Name), "PointLights[%d].Atten.Exp", i);
            GPLS[i].Exp = glGetUniformLocation(prog, Name);

            glUniform3f(GPLS[i].Color, pls[i].Color()[0], pls[i].Color()[1], pls[i].Color()[2]);
            glUniform1f(GPLS[i].AmbientIntensity, pls[i].AmbientIntensity());
            glUniform1f(GPLS[i].DiffuseIntensity, pls[i].DiffuseIntensity());
            glUniform3f(GPLS[i].Position, pls[i].Pos()[0], pls[i].Pos()[1], pls[i].Pos()[2]);
            glUniform1f(GPLS[i].Constant, pls[i].Constant());
            glUniform1f(GPLS[i].Linear, pls[i].Linear());
            glUniform1f(GPLS[i].Exp, pls[i].Exp());
        }
    }
    glUniform1i(NumPLight, pls.size());
    Tex->Bind(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, 0);
    PrintLog();
    glutSwapBuffers();
}

void KeyPressed(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'o':
            dl.AmbientIntensity() += 0.05;
            break;
        case 'p':
            dl.AmbientIntensity() -= 0.05;
            break;
        case 'k':
            dl.DiffuseIntensity() += 0.05;
            std::cout << dl.DiffuseIntensity() << std::endl;
            break;
        case 'l':
            dl.DiffuseIntensity() -= 0.05;
            break;
        case 'v':
            AddPointLight();
            break;
    }

    KEH.Press(key, x, y);
}

void KeyUp(unsigned char key, int x, int y)
{
    KEH.Release(key, x, y);
}

void MouseClick(int button , int state, int x, int y)
{
    MEH.GetMouseInfo(button, state, x, y);
}

void MouseMove(int x, int y)
{
    MEH.MouseMove(x, y);
    Point angles = CEH.GetRotationAngles(MEH.Info());
    Cam.Rotate(angles);
}
int main(int argc, char *argv[])
{
    Projection = EyeMatrix4();
    Rotattion = EyeMatrix4();
    Translation = EyeMatrix4();
    MScale = EyeMatrix4();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    Window w(1024, 768, 300, 300, "Window");
    glClearColor(.5f, .5f, 1.0f, 1.0f);
    glutIdleFunc(RenderScene);
    glutKeyboardUpFunc(KeyUp);
    glutKeyboardFunc(KeyPressed);
    glutMouseFunc(MouseClick);
//    glutPassiveMotionFunc(MouseMove);
    glutMotionFunc(MouseMove);
    // glutDisplayFunc(RenderScene);
    GLenum res = glewInit();
    if (res != GLEW_OK)
        return 1;
    CreateVertexBuffer();
    CreateShaders();
    glutGameModeString("1920x1200@64");
    glutEnterGameMode();
    glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    MEH.MouseMove(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    MEH.MouseMove(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    Tex = new Texture(GL_TEXTURE_2D, "../rsc/stone2.jpg");
    if (!Tex->Load())
        return 1;
    glutMainLoop();
}