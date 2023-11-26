
#include "WaveWidget.h"

#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QtGlobal>
#include <QMouseEvent>
#include <QWheelEvent>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stddef.h>

void GLAPIENTRY
MessageCallback( GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam )
{
    QString text("GL CALLBACK: %1 type = 0x%2, severity = 0x%3, message = %4\n");
    text = text.arg(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "",
                    QString::number(type, 16),
                    QString::number(severity, 16),
                    message);

    std::cerr << qPrintable(text) << '\n';
}

// Slots ////////////////////////////////////////////

void WaveWidget::use_normal_color()
{
    this->makeCurrent();
    m_shader_p->Use();
    glUniform1ui(glGetUniformLocation(m_shader_p->Program, "how_to_render"), 0);
    this->doneCurrent();
}

void WaveWidget::use_reflect()
{
    this->makeCurrent();
    m_shader_p->Use();
    glUniform1ui(glGetUniformLocation(m_shader_p->Program, "how_to_render"), 1);
    this->doneCurrent();
}

void WaveWidget::use_refract()
{
    this->makeCurrent();
    m_shader_p->Use();
    glUniform1ui(glGetUniformLocation(m_shader_p->Program, "how_to_render"), 2);
    this->doneCurrent();
}

void WaveWidget::toggle_pixelization(bool on)
{
    this->m_pixelization = on;
}

// Ctor & Dtor ///////////////////////////////////////

WaveWidget::WaveWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_shader_p(nullptr), m_frame(0),
    m_Arc_Ball(glm::vec3(0, 0, 0), 3, glm::radians(45.f), glm::radians(20.f)),
    m_pixelization(false)
{
}

WaveWidget::~WaveWidget()
{
    this->makeCurrent();
    glDeleteFramebuffers(1, &m_frame_buffer);
    glDeleteTextures(1, &m_color_texture);
    glDeleteRenderbuffers(1, &m_depth_stencil_rbo);
}

// OpenGL /////////////////////////////////////////////

void WaveWidget::initializeGL()
{
    /// glad load OpenGL
    int version = gladLoaderLoadGL();
    if (version == 0) {
        QMessageBox::critical(nullptr, "OpenGL Load Fail", "Cannot Load OpenGL");
        exit(EXIT_FAILURE);
    }
    qDebug() << "Load OpenGL " << GLAD_VERSION_MAJOR(version) << '.' << GLAD_VERSION_MINOR(version);

    /// @todo load texture
    try {
        m_texture_cube_map_p = std::make_unique<qtTextureCubeMap>(":/right.jpg", ":/left.jpg", ":/top.jpg", ":/bottom.jpg", ":/front.jpg", ":/back.jpg");
    }
    catch (std::invalid_argument& ex) {
        QMessageBox::critical(nullptr, "Load fail", ex.what());
        exit(EXIT_FAILURE);
    }
    m_texture_cube_map_p->bind_to(0);

    /// @todo set shader
    try {
        m_shader_p = std::make_unique<Shader>("shader/wave.vert", nullptr, nullptr, nullptr, "shader/wave.frag");
        m_shader_p->Use();
        glUniform1ui(glGetUniformLocation(m_shader_p->Program, "skybox"), 0);

        m_skybox_shader_p = std::make_unique<Shader>("shader/skybox.vert", nullptr, nullptr, nullptr, "shader/skybox.frag");
        m_skybox_shader_p->Use();
        glUniform1ui(glGetUniformLocation(m_skybox_shader_p->Program, "skybox"), 0);

        m_pixelization_shader_p = std::make_unique<Shader>("shader/pixel.vert", nullptr, nullptr, nullptr, "shader/pixel.frag");
        m_pixelization_shader_p->Use();
        glUniform1ui(glad_glGetUniformLocation(m_pixelization_shader_p->Program, "color_buffer"), 0);
    }
    catch(std::runtime_error& ex) {
        QMessageBox::critical(nullptr, "Compiling Shader Failed", ex.what());
        exit(EXIT_FAILURE);
    }

    /// @todo initialize the VAO
    m_wave_VAO_p = std::make_unique<Wave_VAO>(10);
    m_skybox_VAO_p = std::make_unique<Box_VAO>(50);
    m_plane_VAO_p = std::make_unique<Plane_VAO>();

    /// @todo initialize the UBO
    m_matrices_UBO_p = std::make_unique<UBO>(2 * sizeof(glm::mat4), GL_DYNAMIC_DRAW);
    this->set_view_matrix_from_arc_ball();

    /// set up FBO
    glGenFramebuffers(1, &m_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
    {
        glGenTextures(1, &m_color_texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_color_texture);
        glTexImage2D(GL_TEXTURE_2D, /* level */0, /* internal */GL_RGB, width(), height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture, 0);

        glGenRenderbuffers(1, &m_depth_stencil_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth_stencil_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width(), height());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(.5f, .5f, .5f, 1.f);
    glEnable(GL_DEPTH_TEST);

    /// During init, enable debug output
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );

    /// set up timer
    m_timer.setInterval(20);
    connect(&m_timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    m_timer.start();
}

void WaveWidget::resizeGL(int w, int h)
{
    glm::mat4 proj = glm::perspective(glm::radians(50.f), (float)w / h, 0.1f, 200.f);
    // 更新UBO
    this->makeCurrent();
    m_matrices_UBO_p->BufferSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));

    // 更新FBO關聯的color buffer和depth buffer
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    glTexImage2D(GL_TEXTURE_2D, /* level */0, /* internal */GL_RGB, width(), height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_stencil_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width(), height());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    this->doneCurrent();
}

void WaveWidget::paintGL()
{
    qDebug() << m_frame;

    GLint old_fbo;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_fbo);

    if (m_pixelization) {
        // 綁定自己的frame buffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frame_buffer);
        assert(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_matrices_UBO_p->bind_to(0);

    m_skybox_shader_p->Use();
    glDepthMask(GL_FALSE);
    m_skybox_VAO_p->draw();
    glDepthMask(GL_TRUE);

    m_shader_p->Use();
    glUniform1ui(glGetUniformLocation(m_shader_p->Program, "frame"), m_frame);
    m_wave_VAO_p->draw();

    if (m_pixelization) {
        // 換回預設的frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_color_texture);
        // 做後處理
        m_pixelization_shader_p->Use();
        m_plane_VAO_p->draw();
    }

    ++m_frame;
}

// Mouse Event ////////////////////////////////////////////

void WaveWidget::mousePressEvent(QMouseEvent *e)
{
    this->setMouseTracking(true);
    m_last_clicked = e->pos();
    m_last_alpha = m_Arc_Ball.alpha();
    m_last_beta = m_Arc_Ball.beta();
}

void WaveWidget::mouseMoveEvent(QMouseEvent *e)
{
    int delta_x = e->x() - m_last_clicked.x();
    int delta_y = e->y() - m_last_clicked.y();

    m_Arc_Ball.set_alpha(m_last_alpha + glm::radians<float>(delta_x));
    m_Arc_Ball.set_beta(m_last_beta + glm::radians<float>(delta_y));

    this->makeCurrent();
    this->set_view_matrix_from_arc_ball();
    this->doneCurrent();
}

void WaveWidget::mouseReleaseEvent(QMouseEvent *e)
{
    this->setMouseTracking(false);
}

// Wheel Event //////////////////////////////////////////////

void WaveWidget::wheelEvent(QWheelEvent *e)
{
    QPoint degree_move = e->angleDelta();
    qDebug() << "Wheel is scrolled";

    if (!degree_move.isNull()) {
        m_Arc_Ball.set_r(m_Arc_Ball.r() + degree_move.y() / 120.f);
    }

    this->makeCurrent();
    this->set_view_matrix_from_arc_ball();
    this->doneCurrent();
}

// Private Methods /////////////////////////////////////////

void WaveWidget::set_view_matrix_from_arc_ball()
{
    glm::mat4 view = m_Arc_Ball.view_matrix();
    m_matrices_UBO_p->BufferSubData(0, sizeof(glm::mat4), glm::value_ptr(view));

    m_shader_p->Use();
    glUniform3fv(
        glGetUniformLocation(m_shader_p->Program, "eye_position"),
        1,
        glm::value_ptr(m_Arc_Ball.calc_pos())
    );
}


