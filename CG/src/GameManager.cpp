//
// Created by keltar on 5/14/17.
//

#include <iostream>
#include "../include/GameManager.h"

GameManager::GameManager()
{

}

void GameManager::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glUniform3f(dlu.color, directionalLight.Color()[0], directionalLight.Color()[1], directionalLight.Color()[2]);
    Vec3f Direction = directionalLight.Direction();
    Direction.Normalize();
    glUniform3f(dlu.direction, Direction[0], Direction[1], Direction[2]);
    glUniform1f(dlu.diffuse, directionalLight.DiffuseIntensity());
    glUniform1f(dlu.intense, directionalLight.AmbientIntensity());

    Vec3f camNewPos = ceh.GetCameraPosition(keh.keys(), this->activeCam);
    Matrix4 camCoords = activeCam->GetProjectionPerspectiveMatrix()  * activeCam->GetUVNMatrix() * activeCam->SetPosition(camNewPos);
    if (pointLights.size() > 0)
    {
        Vec3f camCurPos = activeCam->Position();
        glUniform3f(activeCamUniform, camCurPos[0], camCurPos[1], camCurPos[2]);
    }
    for (int i = 0; i < models.size(); i++)
    {
        auto world = models[i]->GetTrans();
        auto tmp = camCoords * world;
        glUniformMatrix4fv(WVP, 1, GL_TRUE, (GLfloat *)&tmp);
        glUniformMatrix4fv(worldTrans, 1, GL_TRUE, (GLfloat *)&world);
        models[i]->Draw();
    }
    glutSwapBuffers();
}

void GameManager::Init()
{
    activeCam = new Camera();

    InitShaderProgram();
    InitUniformLocations();
    InitPlatform();
    InitDirectionalLight();
    AddCube();
    glUniform1i(sampler, 0);
}

void GameManager::InitShaderProgram()
{
    std::ifstream fragFin("../shaders/fragment/frag.glsl");
    std::ifstream vertFin("../shaders/vertex/vert.glsl");
    Shader vert(vertFin, GL_VERTEX_SHADER);
    Shader frag(fragFin, GL_FRAGMENT_SHADER);
    vert.compile();
    frag.compile();
    program.attachShader(vert);
    program.attachShader(frag);
    program.link();
    if (program.isValid())
        program.use();
    else
        throw std::runtime_error("Shader program is not valid");
}

void GameManager::InitUniformLocations()
{
    sampler = (GLuint) glGetUniformLocation(program.program(), "Sampler");
    assert(sampler != 0xFFFFFFFF);
    WVP = (GLuint)glGetUniformLocation(program.program(), "WVP");
    assert(WVP != 0xFFFFFFFF);
    dlu.color = (GLuint) glGetUniformLocation(program.program(), "directionalLight.Base.Color");
    assert(dlu.color != 0xFFFFFFFF);
    dlu.intense = (GLuint) glGetUniformLocation(program.program(), "directionalLight.Base.AmbientIntensity");
    assert(dlu.intense != 0xFFFFFFFF);
    dlu.diffuse = (GLuint) glGetUniformLocation(program.program(), "directionalLight.Base.DiffuseIntensity");
    assert(dlu.diffuse != 0xFFFFFFFF);
    dlu.direction = (GLuint) glGetUniformLocation(program.program(), "directionalLight.Direction");
    assert(dlu.direction != 0xFFFFFFFF);
    worldTrans = (GLuint) glGetUniformLocation(program.program(), "World");
    assert(worldTrans != 0xFFFFFFFF);
}

void GameManager::InitPlatform()
{
    Vertex vs[] = {
            Vertex({-10.0f, 0.0, -10.0f}, {0.0f, 0.0f}),
            Vertex({-10.0f, 0.0,  10.0},  {0.0f, 1.0f}),
            Vertex({ 10.0f, 0.0, -10.0f}, {1.0f, 0.0f}),
            Vertex({ 10.0f, 0.0,  10.0},  {1.0f, 1.0f}),
    };

    GLuint inds[] = {
            1,  0,  2,
            1,  2,  3
    };
    Model *m = new Model(vs, inds, sizeof(vs) / sizeof(vs[0]), sizeof(inds) / sizeof(inds[0]), GL_TEXTURE_2D, "../rsc/black.jpg");
    models.push_back(m);
}

void GameManager::AddPointLight()
{
    PointLight *pl = new PointLight();
    pl->Color() = {0.28627450980392155f, 0.49411764705882355f, 0.4627450980392157f};
    pl->Pos() = activeCam->Position();
    pl->Constant() = .0f;
    pl->Linear() = .1f;
    pl->Exp() = .0f;
    pl->DiffuseIntensity() = .5f;
    pl->AmbientIntensity() = .3f;
    pointLights.push_back(pl);
    glUniform1i(pointLightsNumber, (GLint) pointLights.size());
    activeCamUniform = (GLuint) glGetUniformLocation(program.program(), "Cam");
    plus.push_back(PointLightUniform());
    for (int i = 0; i < plus.size(); i++)
    {
        char Name[128];
        snprintf(Name, sizeof(Name), "PointLights[%d].Base.Color", i);
        plus[i].color = (GLuint) glGetUniformLocation(program.program(), Name);
        snprintf(Name, sizeof(Name), "PointLights[%d].Base.AmbientIntensity", i);
        plus[i].ambientIntensity= (GLuint) glGetUniformLocation(program.program(), Name);
        snprintf(Name, sizeof(Name), "PointLights[%d].Base.DiffuseIntensity", i);
        plus[i].diffuseIntensity= (GLuint) glGetUniformLocation(program.program(), Name);
        snprintf(Name, sizeof(Name), "PointLights[%d].Position", i);
        plus[i].position = (GLuint) glGetUniformLocation(program.program(), Name);
        snprintf(Name, sizeof(Name), "PointLights[%d].Atten.Constant", i);
        plus[i].constant = (GLuint) glGetUniformLocation(program.program(), Name);
        snprintf(Name, sizeof(Name), "PointLights[%d].Atten.Linear", i);
        plus[i].linear = (GLuint) glGetUniformLocation(program.program(), Name);
        snprintf(Name, sizeof(Name), "PointLights[%d].Atten.Exp", i);
        plus[i].exp = (GLuint) glGetUniformLocation(program.program(), Name);

        glUniform3f(plus[i].color, pointLights[i]->Color()[0], pointLights[i]->Color()[1], pointLights[i]->Color()[2]);
        glUniform1f(plus[i].ambientIntensity, pointLights[i]->AmbientIntensity());
        glUniform1f(plus[i].diffuseIntensity, pointLights[i]->DiffuseIntensity());
        glUniform3f(plus[i].position, pointLights[i]->Pos()[0], pointLights[i]->Pos()[1], pointLights[i]->Pos()[2]);
        glUniform1f(plus[i].constant, pointLights[i]->Constant());
        glUniform1f(plus[i].linear, pointLights[i]->Linear());
        glUniform1f(plus[i].exp, pointLights[i]->Exp());
    }
}

void GameManager::AddCube()
{
    auto c = activeCam->Position();
    Vertex vs[] = {

            Vertex({-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}),
            Vertex({-0.5f,  0.5f, -0.5f}, {0.0f, 0.5f}),
            Vertex({ 0.5f, -0.5f, -0.5f}, {0.5f, 0.0f}),
            Vertex({ 0.5f,  0.5f, -0.5f}, {0.5f, 0.5f}),

            Vertex({-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}),
            Vertex({-0.5f,  0.5f,  0.5f}, {0.0f, 0.5f}),
            Vertex({ 0.5f, -0.5f,  0.5f}, {0.5f, 0.0f}),
            Vertex({ 0.5f,  0.5f,  0.5f}, {0.5f, 0.5f}),

            Vertex({-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}),
            Vertex({-0.5f,  0.5f, -0.5f}, {0.0f, 0.5f}),
            Vertex({-0.5f, -0.5f,  0.5f}, {0.5f, 0.0f}),
            Vertex({-0.5f,  0.5f,  0.5f}, {0.5f, 0.5f}),

            Vertex({ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}),
            Vertex({ 0.5f,  0.5f, -0.5f}, {0.0f, 0.5f}),
            Vertex({ 0.5f, -0.5f,  0.5f}, {0.5f, 0.0f}),
            Vertex({ 0.5f,  0.5f,  0.5f}, {0.5f, 0.5f}),

            Vertex({-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}),
            Vertex({-0.5f, -0.5f,  0.5f}, {0.0f, 0.5f}),
            Vertex({ 0.5f, -0.5f, -0.5f}, {0.5f, 0.0f}),
            Vertex({ 0.5f, -0.5f,  0.5f}, {0.5f, 0.5f}),

            Vertex({-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}),
            Vertex({-0.5f,  0.5f,  0.5f}, {0.0f, 0.5f}),
            Vertex({ 0.5f,  0.5f, -0.5f}, {0.5f, 0.0f}),
            Vertex({ 0.5f,  0.5f,  0.5f}, {0.5f, 0.5f})
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
    Model *m = new Model(vs, Indices, sizeof(vs) / sizeof(vs[0]), sizeof(Indices) / sizeof(Indices[0]), GL_TEXTURE_2D, "../rsc/stone2.jpg");
    m->SetPosition(c[0], c[1], c[2]);
    models.push_back(m);
}

void GameManager::InitDirectionalLight()
{
    directionalLight = DirectionalLight({1.0f, 1.0f, 1.0f}, 1.0f, {1.0f, .0f, .0f}, 0.5f);
}

void GameManager::MouseClick(int button, int state, int x, int y)
{
    meh.GetMouseInfo(button, state, x, y);
}

void GameManager::KeyPress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'o':
            directionalLight.AmbientIntensity() += 0.05;
            break;
        case 'p':
            directionalLight.AmbientIntensity() -= 0.05;
            break;
        case 'k':
            directionalLight.DiffuseIntensity() += 0.05;
            break;
        case 'l':
            directionalLight.DiffuseIntensity() -= 0.05;
            break;
        case 'v':
            AddPointLight();
            break;
        case 'b':
            AddCube();
            break;
    }
    keh.Press(key, x, y);
}

void GameManager::KeyUp(unsigned char key, int x, int y)
{
    keh.Release(key, x, y);
}

void GameManager::MouseMove(int x, int y)
{
    meh.MouseMove(x, y);
    Point angles = ceh.GetRotationAngles(meh.Info());
    activeCam->Rotate(angles);
}

//void GameManager::InitGlutCallbacks()
//{
//    glutKeyboardUpFunc(KeyUp);
//    glutKeyboardFunc(KeyPressed);
//    glutMouseFunc(MouseClick);
//    glutMotionFunc(MouseMove);
//}

