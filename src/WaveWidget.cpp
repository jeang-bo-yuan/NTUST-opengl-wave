
#include "WaveWidget.h"
#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QtGlobal>
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


WaveWidget::WaveWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_shader_p(nullptr)
{
}

WaveWidget::~WaveWidget()
{
    delete m_shader_p;
}

void WaveWidget::initializeGL()
{
    int version = gladLoaderLoadGL();
    if (version == 0) {
        QMessageBox::critical(this, "OpenGL Load Fail", "Cannot Load OpenGL");
        qApp->exit(EXIT_FAILURE);
    }
    qDebug() << "Load OpenGL " << GLAD_VERSION_MAJOR(version) << '.' << GLAD_VERSION_MINOR(version);

    // set view matrix
    m_eye_pos = glm::vec3(2, 1, 2);
    m_view_matrix = glm::lookAt(m_eye_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    // set shader
    m_shader_p = new Shader("shader/wave.vert", nullptr, nullptr, nullptr, "shader/wave.frag");

    // initialize the VAO
    this->init_VAO();

    glClearColor(.5f, .5f, .5f, 1.f);

    // During init, enable debug output
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );
}

void WaveWidget::resizeGL(int w, int h)
{
    m_proj_matrix = glm::perspective(50.f * 3.14f / 180.f, (float)w / h, 200.f, 0.1f);
}

void WaveWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(m_water_VAO);
    glUseProgram(m_shader_p->Program);
    this->set_uniform_data();

    glDrawElements(GL_TRIANGLES, m_num_of_elements, GL_UNSIGNED_INT, (void*)0);
}

void WaveWidget::init_VAO()
{
    constexpr size_t size = 33;
    constexpr float delta = 2.f / (size - 1);
    // 畫在(-1, -1) ~ (1, 1)
    // 每條邊取33個點（含兩側）
    glm::vec3 point_arr[size][size];
    {
        float z = -1.f;
        for (int r = 0; r < size; ++r) {
            float x = -1.f;
            for (int c = 0; c < size; ++c) {
                point_arr[r][c] = glm::vec3(x, 0, z);
                x += delta;
            }
            z += delta;
        }
    }

    GLuint elem_arr[(size - 1) * (size - 1) * 2 * 3];
    m_num_of_elements = sizeof(elem_arr) / sizeof(GLuint);
    {
        auto to_index = [size](int r, int c)->int {
            return r * size + c;
        };

        int i = 0;
        for (int r = 0; r < size - 1; ++r) {
            for (int c = 0; c < size - 1; ++c) {
                elem_arr[i++] = to_index(r, c);
                elem_arr[i++] = to_index(r, c + 1);
                elem_arr[i++] = to_index(r + 1, c);

                elem_arr[i++] = to_index(r + 1, c);
                elem_arr[i++] = to_index(r, c + 1);
                elem_arr[i++] = to_index(r + 1, c + 1);
            }
        }
        Q_ASSERT(i == m_num_of_elements);
    }

    glGenVertexArrays(1, &m_water_VAO);
    glBindVertexArray(m_water_VAO);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point_arr), point_arr, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(0);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elem_arr), elem_arr, GL_STATIC_DRAW);

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void WaveWidget::set_uniform_data()
{
    glUniformMatrix4fv(
        glGetUniformLocation(m_shader_p->Program, "view_matrix"),
        1, // 1 matrix
        false, // don't transpose
        glm::value_ptr(m_view_matrix)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(m_shader_p->Program, "proj_matrix"),
        1, // 1 matrix
        false, // don't transpose
        glm::value_ptr(m_proj_matrix)
    );

    glUniform3fv(
        glGetUniformLocation(m_shader_p->Program, "eye_position"),
        1, // 1 vec3
        glm::value_ptr(m_eye_pos)
    );
}


