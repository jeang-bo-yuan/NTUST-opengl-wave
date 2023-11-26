
#ifndef PLANE_VAO_H
#define PLANE_VAO_H

#include "VAO_Interface.h"

/**
 * @brief 從(-1, -1) ~ (1, 1)的2D平面
 */
class Plane_VAO : public VAO_Interface {
    GLuint m_vbo;

public:
    Plane_VAO() {
        GLfloat points[] = {
            // coord     texture coord
            -1, -1,      0, 0,
            -1, 1,       0, 1,
            1, 1,        1, 1,
            1, -1,       1, 0
        };
        glBindVertexArray(m_VAO_id);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void draw() override {
        glBindVertexArray(m_VAO_id);
        glDrawArrays(GL_QUADS, 0, 4);
        glBindVertexArray(0);
    }

    ~Plane_VAO() {
        glDeleteBuffers(1, &m_vbo);
    }
};

#endif // PLANE_VAO_H
