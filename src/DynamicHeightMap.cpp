
#include "DynamicHeightMap.h"
#include <stddef.h>
#include <assert.h>
#include <QDebug>

#define IMG_SIZE 100

DynamicHeightMap::DynamicHeightMap()
    : m_plane_VAO(), m_shader_drop("shader/DHM/simple.vert", nullptr, nullptr, nullptr, "shader/DHM/drop.frag"),
    m_shader_update("shader/DHM/simple.vert", nullptr, nullptr, nullptr, "shader/DHM/update.frag")
{
    glGenFramebuffers(1, &m_fbo);
    glGenTextures(1, &m_color_texture);

    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // create color buffer
        glBindTexture(GL_TEXTURE_2D, m_color_texture);
        glTexImage2D(GL_TEXTURE_2D, /* level */0, /* internal */GL_RGB, IMG_SIZE, IMG_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture, 0);

        // create depth buffer
        glGenRenderbuffers(1, &m_depth_stencil_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth_stencil_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, IMG_SIZE, IMG_SIZE);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // set up sampler
    m_shader_drop.Use();
    glUniform1i(glGetUniformLocation(m_shader_drop.Program, "u_water"), 1);
    m_shader_update.Use();
    glUniform1i(glGetUniformLocation(m_shader_update.Program, "u_water"), 1);
    glUseProgram(0);
}

DynamicHeightMap::~DynamicHeightMap()
{
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(1, &m_color_texture);
    glDeleteRenderbuffers(1, &m_depth_stencil_rbo);
}

void DynamicHeightMap::update()
{
    glDepthFunc(GL_ALWAYS);

    GLint old_fbo;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_fbo);
    GLint old_viewport[4];
    glGetIntegerv(GL_VIEWPORT, old_viewport);

    // 綁定自己的frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    assert(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glViewport(0, 0, IMG_SIZE, IMG_SIZE);
    m_shader_update.Use();
    this->bind(1);
    m_plane_VAO.draw();

    glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);
    glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
    this->unbind(1);
    glDepthFunc(GL_LESS);
}

void DynamicHeightMap::add_drop(GLfloat x, GLfloat y)
{
    glDepthFunc(GL_ALWAYS);

    GLint old_fbo;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_fbo);
    GLint old_viewport[4];
    glGetIntegerv(GL_VIEWPORT, old_viewport);

    // 綁定自己的frame buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    assert(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glViewport(0, 0, IMG_SIZE, IMG_SIZE);
    m_shader_drop.Use();
    this->bind(1);
    // call glUniform to pass (x, y)
    glUniform2f(glGetUniformLocation(m_shader_drop.Program, "u_center"), x, y);
    m_plane_VAO.draw();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, old_fbo);
    glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
    this->unbind(1);
    glDepthFunc(GL_LESS);
}

void DynamicHeightMap::bind(GLuint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
}

void DynamicHeightMap::unbind(GLuint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_2D, 0);
}

