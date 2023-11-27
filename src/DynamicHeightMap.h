
#ifndef DYNAMICHEIGHTMAP_H
#define DYNAMICHEIGHTMAP_H

#include <glad/gl.h>
#include "Plane_VAO.h"
#include "Shader.h"

class DynamicHeightMap
{
public:
    GLuint m_fbo;
    GLuint m_color_texture;
    GLuint m_depth_stencil_rbo;
    Plane_VAO m_plane_VAO;
    Shader m_shader_drop;
    Shader m_shader_update;

public:
    DynamicHeightMap();

    ~DynamicHeightMap();

    /**
     * @brief 更新height map
     */
    void update();

    /**
     * @brief
     * @param x
     * @param y
     * @pre 0 <= x,y <= 1
     */
    void add_drop(GLfloat x, GLfloat y);

    /**
     * @brief 綁定height map到特定sampler
     * @param sampler
     */
    void bind(GLuint sampler);

    /**
     * @brief 從特定sampler解除綁定
     * @param sampler
     */
    void unbind(GLuint sampler);
};

#endif // DYNAMICHEIGHTMAP_H
