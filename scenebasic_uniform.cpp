#include "scenebasic_uniform.h"
#include "helper/glutils.h"
#include "helper/texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>




SceneBasic_Uniform::SceneBasic_Uniform() :
    tPrev(0.0f), angle(0.0f), rotSpeed(glm::pi<float>() / 8.0f),
    plane(10.0f, 10.0f, 100.0, 100.0),LightPos(2.0f, 4.0f, 2.0f)
{
    
    orge = ObjMesh::loadWithAdjacency("media/models/Rook.obj", true);
}

GLuint SceneBasic_Uniform::loadSkybox()
{
    
    std::vector<std::string> imagePaths = {
        "media/skybox/right.jpg", 
        "media/skybox/left.jpg",  
        "media/skybox/bottom.jpg",
        "media/skybox/top.jpg",   
        "media/skybox/front.jpg", 
        "media/skybox/back.jpg"   
    };

   

    
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

    
    int width, height, channels;
    for (size_t i = 0; i < imagePaths.size(); i++) 
    {
        unsigned char* data = Texture::loadPixels(imagePaths[i], width, height, 1, 3);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            Texture::deletePixels(data);
        }
        else
        {
            Texture::deletePixels(data);
        }
    }
    
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texId;
}

void SceneBasic_Uniform::drawSkybox()
{
    glDepthFunc(GL_LEQUAL);  
    this->progSkybox.use();

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(100.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
    glm::mat4 mvp = projection * view * model;

    
    progSkybox.setUniform("MVP", mvp);

    
    glBindVertexArray(skybox.cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS); 
}


void SceneBasic_Uniform::initScene()
{
    compile();                  
    glEnable(GL_DEPTH_TEST);

    
    camera.setPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    camera.setUp(glm::vec3(0.0f, 1.0f, 0.0f));

    
    GLFWwindow* win = glfwGetCurrentContext();
    camera.setWindow(win);

    
    glfwSetWindowUserPointer(win, this);
    glfwSetCursorPosCallback(win, [](GLFWwindow* window, double xpos, double ypos) {
        SceneBasic_Uniform* scene = static_cast<SceneBasic_Uniform*>(glfwGetWindowUserPointer(window));
        if (scene) scene->handleMouse(xpos, ypos);
    });
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    view = camera.getViewMatrix();
    projection = glm::mat4(1.0f);

    
    this->progSkybox.use();
    this->skyboxTex = loadSkybox();
    glActiveTexture(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyboxTex);
    this->progSkybox.setUniform("skyboxMap", 0);
    
    prog.use();
    prog.setUniform("Light.L", glm::vec3(1.0f));
    prog.setUniform("Light.La", glm::vec3(1.0f));
    prog.setUniform("Light.Ld", glm::vec3(1.0f));

    texBase = Texture::loadTexture("media/texture/sword/Rook_Base_Colour.png");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texBase);
    prog.setUniform("Material.BaseColor", 0);

    texMetal = Texture::loadTexture("media/texture/sword/Rook_Metallic.png");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texMetal);
    prog.setUniform("Material.Metallic", 1);

    texNormal = Texture::loadTexture("media/texture/sword/Rook_Normal.png");
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texNormal);
    prog.setUniform("Material.Normal", 2);

    texOcclusion = Texture::loadTexture("media/texture/sword/Rook_AO.png");
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texOcclusion);
    prog.setUniform("Material.Occlusion", 3);

    texRoughness = Texture::loadTexture("media/texture/sword/Rook_Roughness.png");
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texRoughness);
    prog.setUniform("Material.Roughness", 4);

    progPlane.use();
    progPlane.setUniform("Light.L", glm::vec3(1.0f));
    progPlane.setUniform("Light.La", glm::vec3(0.8f));
    progPlane.setUniform("Light.Ld", glm::vec3(0.9f));

    texBasePlane = Texture::loadTexture("media/texture/rock/forest/forest_ground_04_diff_1k.jpg", 3);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, texBasePlane);
    progPlane.setUniform("planeBaseColorMap", 5);

    texNormalPlane = Texture::loadTexture("media/texture/rock/forest/forest_ground_04_nor_gl_1k.jpg", 3);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, texNormalPlane);
    progPlane.setUniform("planeNormalMap", 6);

    texRoughnessPlane = Texture::loadTexture("media/texture/rock/forest/forest_ground_04_rough_1k.jpg", 3);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, texRoughnessPlane);
    progPlane.setUniform("planeRoughnessMap", 7);

    texDisplacementPlane = Texture::loadTexture("media/texture/rock/forest/forest_ground_04_disp_1k.jpg", 3);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, texDisplacementPlane);
    progPlane.setUniform("planeOcclusionMap", 8);

    return;
}


void SceneBasic_Uniform::compile()
{
    try {
        progSkybox.compileShader("shader/skybox.vert");
        progSkybox.compileShader("shader/skybox.frag");
        progSkybox.link();
        
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        
		progPlane.compileShader("shader/plane.vert");
		progPlane.compileShader("shader/plane.frag");
		progPlane.link();
    }
    catch (GLSLProgramException& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}


void SceneBasic_Uniform::update(float t)
{
    float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;

    
    camera.processInput(deltaT);

    
    angle += 0.1f * deltaT;
    if (this->m_animate) {
        angle += rotSpeed * deltaT;
        if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
    }
}


void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = camera.getViewMatrix();

    
    prog.use();
    prog.setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    model = glm::mat4(1.0f);
    prog.setUniform("Material.Ka", glm::vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Kd", glm::vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("Material.Ks", glm::vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Material.Shininess", 100.0f);
    model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
    model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f));
    setMatrices(prog);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texBase);
    prog.setUniform("Material.BaseColor", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texMetal);
    prog.setUniform("Material.Metallic", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texNormal);
    prog.setUniform("Material.Normal", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texOcclusion);
    prog.setUniform("Material.Occlusion", 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texRoughness);
    prog.setUniform("Material.Roughness", 4);

    orge->render();

    // 渲染平面
    progPlane.use();
    progPlane.setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    model = glm::mat4(1.0f);
    progPlane.setUniform("Material.Ka", glm::vec3(1.0f, 1.0f, 1.0f));
    progPlane.setUniform("Material.Kd", glm::vec3(0.7f, 0.7f, 0.7f));
    progPlane.setUniform("Material.Ks", glm::vec3(0.4f, 0.5f, 0.5f));
    progPlane.setUniform("Material.Shininess", 100.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    setMatrices(progPlane);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, texBasePlane);
    progPlane.setUniform("planeBaseColorMap", 5);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, texNormalPlane);
    progPlane.setUniform("planeNormalMap", 6);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, texRoughnessPlane);
    progPlane.setUniform("planeRoughnessMap", 7);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, texDisplacementPlane);
    progPlane.setUniform("planeOcclusionMap", 8);

    plane.render();

    // 渲染天空盒
    drawSkybox();
}



void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    projection = glm::perspective(glm::radians(50.0f),
                                  float(w) / float(h),
                                  0.3f,
                                  100.0f);
}

// 针对不同的 GLSLProgram 设置 MVP / NormalMatrix
void SceneBasic_Uniform::setMatrices(GLSLProgram & shader)
{
    glm::mat4 mv = view * model;
    shader.setUniform("ModelViewMatrix", mv);

    glm::mat3 normMat = glm::mat3(glm::vec3(mv[0]),
                                  glm::vec3(mv[1]),
                                  glm::vec3(mv[2]));
    shader.setUniform("NormalMatrix", normMat);

    shader.setUniform("MVP", projection * mv);
}

// 处理鼠标移动
void SceneBasic_Uniform::handleMouse(double xpos, double ypos)
{
    camera.processMouseMovement(xpos, ypos);
}
