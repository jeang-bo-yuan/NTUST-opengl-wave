
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
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
    fflush(stderr);
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
    int version = gladLoaderLoadGL();
    if (version == 0) {
        QMessageBox::critical(this, "OpenGL Load Fail", "Cannot Load OpenGL");
        qApp->exit(EXIT_FAILURE);
    }
    qDebug() << "Load OpenGL " << GLAD_VERSION_MAJOR(version) << '.' << GLAD_VERSION_MINOR(version);

    /// set shader
    m_shader_p = std::make_unique<Shader>("shader/wave.vert", nullptr, nullptr, nullptr, "shader/wave.frag");

    /// initialize the VAO
    m_wave_VAO_p = std::make_unique<Wave_VAO>();

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
    if (m_perspective_changed) {
        glUniformMatrix4fv(
            glGetUniformLocation(m_shader_p->Program, "proj_matrix"),
            1, // 1 matrix
            false, // don't transpose
            glm::value_ptr(m_proj_matrix)
        );
        qDebug() << "Perspective Matrix is changed";
        m_perspective_changed = false;
    }

    if (m_eye_pos_changed) {
        glUniformMatrix4fv(
            glGetUniformLocation(m_shader_p->Program, "view_matrix"),
            1, // 1 matrix
            false, // don't transpose
            glm::value_ptr(m_Arc_Ball.view_matrix())
        );

        glUniform3fv(
            glGetUniformLocation(m_shader_p->Program, "eye_position"),
            1, // 1 vec3
            glm::value_ptr(m_Arc_Ball.calc_pos())
        );
        qDebug() << "Eye Position changed";
        m_eye_pos_changed = false;
    }

    glUniform1ui(
        glGetUniformLocation(m_shader_p->Program, "frame"),
        m_frame
    );
}


