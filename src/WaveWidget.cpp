
#include "WaveWidget.h"

#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QtGlobal>
#include <QMouseEvent>
#include <QWheelEvent>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    QMessageBox::warning(nullptr, "OpenGL CallBack", text);
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
    m_Arc_Ball(glm::vec3(0, 0, 0), 3, glm::radians(45.f), glm::radians(20.f)),
    m_eye_pos_changed(true), m_perspective_changed(true)
{
}

WaveWidget::~WaveWidget()
{
}

// OpenGL /////////////////////////////////////////////

void WaveWidget::initializeGL()
{
    /// glad load OpenGL
    int version = gladLoaderLoadGL();
    if (version == 0) {
        QMessageBox::critical(this, "OpenGL Load Fail", "Cannot Load OpenGL");
        exit(EXIT_FAILURE); // QT event loop hasn't start, so call exit() instead of qApp->exit()
    }
    qDebug() << "Load OpenGL " << GLAD_VERSION_MAJOR(version) << '.' << GLAD_VERSION_MINOR(version);

    /// @todo load texture
    try {
        m_texture_cube_map_p = std::make_unique<qtTextureCubeMap>(":/right.jpg", ":/left.jpg", ":/top.jpg", ":/bottom.jpg", ":/front.jpg", ":/back.jpg");
    }
    catch (std::invalid_argument& ex) {
        QMessageBox::critical(this, "Load fail", ex.what());
        exit(EXIT_FAILURE);
    }
    m_texture_cube_map_p->bind_to(0);

    /// @todo set shader
    m_shader_p = std::make_unique<Shader>("shader/wave.vert", nullptr, nullptr, nullptr, "shader/wave.frag");
    m_shader_p->Use();
    glUniform1ui(glGetUniformLocation(m_shader_p->Program, "skybox"), 0);
    m_skybox_shader_p = std::make_unique<Shader>("shader/skybox.vert", nullptr, nullptr, nullptr, "shader/skybox.frag");
    m_skybox_shader_p->Use();
    glUniform1ui(glGetUniformLocation(m_skybox_shader_p->Program, "skybox"), 0);

    /// @todo initialize the VAO
    m_wave_VAO_p = std::make_unique<Wave_VAO>();
    m_skybox_VAO_p = std::make_unique<Box_VAO>(100);

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
    m_proj_matrix = glm::perspective(glm::radians(50.f), (float)w / h, 0.1f, 200.f);
    m_perspective_changed = true;
}

void WaveWidget::paintGL()
{
    qDebug() << m_frame;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shader_p->Program);
    this->set_uniform_data();
    m_wave_VAO_p->draw();

    m_skybox_shader_p->Use();
    this->set_uniform_data();
    m_skybox_VAO_p->draw();

    ++m_frame;
    m_perspective_changed = false;
    m_eye_pos_changed = false;
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
    m_eye_pos_changed = true;
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

    m_eye_pos_changed = true;

}

// Set Up Uniform Data //////////////////////////////////////

void WaveWidget::set_uniform_data()
{
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    if (m_perspective_changed) {
        glUniformMatrix4fv(
            glGetUniformLocation(program, "proj_matrix"),
            1, // 1 matrix
            false, // don't transpose
            glm::value_ptr(m_proj_matrix)
        );
        qDebug() << "Perspective Matrix is changed";
    }

    if (m_eye_pos_changed) {
        glUniformMatrix4fv(
            glGetUniformLocation(program, "view_matrix"),
            1, // 1 matrix
            false, // don't transpose
            glm::value_ptr(m_Arc_Ball.view_matrix())
        );

        glUniform3fv(
            glGetUniformLocation(program, "eye_position"),
            1, // 1 vec3
            glm::value_ptr(m_Arc_Ball.calc_pos())
        );
        qDebug() << "Eye Position changed";
    }

    glUniform1ui(
        glGetUniformLocation(program, "frame"),
        m_frame
    );
}


