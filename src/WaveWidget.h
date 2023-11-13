
#ifndef WAVEWIDGET_H
#define WAVEWIDGET_H

#include <glad/gl.h>
#include <QOpenGLWidget>
#include <QTimer>
#include <Shader.h>

#include "ArcBall.h"

/**
 * @brief An OpenGL Widget that display sine-wave water
 */
class WaveWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    WaveWidget(QWidget *parent = nullptr);
    ~WaveWidget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void init_VAO();
    /// set uniform data for the shader
    void set_uniform_data();

private:
    /// @brief Vertex Array Object for the water surface
    /// @details
    /// - attribute: [0]={point array}
    /// - element array: 數個三角形
    GLuint m_water_VAO;
    /// the length of the element array
    GLsizei m_num_of_elements;

    Shader* m_shader_p;

    QTimer m_timer;

    glm::mat4x4 m_proj_matrix;
    GLuint m_frame;

    ArcBall m_Arc_Ball;

    bool m_perspective_changed;
};

#endif // WAVEWIDGET_H
