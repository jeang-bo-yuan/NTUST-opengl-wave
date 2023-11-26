
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

// Ctor & Dtor ///////////////////////////////////////

WaveWidget::WaveWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_shader_p(nullptr), m_frame(0),
    m_Arc_Ball(glm::vec3(0, 0, 0), 3, glm::radians(45.f), glm::radians(20.f))
{
}

WaveWidget::~WaveWidget()
{
    this->makeCurrent();
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
    }
    catch(std::runtime_error& ex) {
        QMessageBox::critical(nullptr, "Compiling Shader Failed", ex.what());
        exit(EXIT_FAILURE);
    }

    /// @todo initialize the VAO
    m_wave_VAO_p = std::make_unique<Wave_VAO>(10);
    m_skybox_VAO_p = std::make_unique<Box_VAO>(50);

    /// @todo initialize the UBO
    m_matrices_UBO_p = std::make_unique<UBO>(2 * sizeof(glm::mat4), GL_DYNAMIC_DRAW);
    this->set_view_matrix_from_arc_ball();

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
    this->doneCurrent();
}

void WaveWidget::paintGL()
{
    qDebug() << m_frame;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_matrices_UBO_p->bind_to(0);

    m_skybox_shader_p->Use();
    glDepthMask(GL_FALSE);
    m_skybox_VAO_p->draw();
    glDepthMask(GL_TRUE);

    m_shader_p->Use();
    glUniform1ui(glGetUniformLocation(m_shader_p->Program, "frame"), m_frame);
    m_wave_VAO_p->draw();

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


