#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/glslprogram.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "helper/Camera.h"
#include "helper/skybox.h"

class SceneBasic_Uniform : public Scene {
private:
    Plane plane;
    
    std::unique_ptr<ObjMesh> orge;

    GLSLProgram prog;
	GLSLProgram progPlane;
    GLuint texBase, texMetal, texNormal, texOcclusion, texRoughness;
    GLuint texBasePlane, texNormalPlane, texRoughnessPlane, texDisplacementPlane;
    
    Camera camera;
    float angle;
    glm::mat4 model, view, projection;
    float tPrev;
    float rotSpeed;

    GLSLProgram progSkybox;
    SkyBox skybox;
    GLuint skyboxTex;       

	glm::vec3 LightPos;


    void compile();
    void setMatrices(GLSLProgram& shader);
    GLuint loadSkybox();
    void drawSkybox();

public:
    SceneBasic_Uniform();
    void initScene();
    void update(float t);
    void render();
    void resize(int, int);

    
    virtual void handleMouse(double xpos, double ypos) override;
};

#endif
