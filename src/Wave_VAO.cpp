
#include "Wave_VAO.h"
#include <glm/vec3.hpp>
#include <assert.h>

Wave_VAO::Wave_VAO()
    : VAO_Interface()
{
    constexpr int size = 33;
    constexpr float delta = 2.f / (size - 1);
    // 畫在(-1, 0, -1) ~ (1, 0, 1)
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

    // 生成element array
    GLuint elem_arr[(size - 1) * (size - 1) * 2 * 3];
    m_num_of_elements = sizeof(elem_arr) / sizeof(GLuint);
    {
        auto to_index = [size = size](int r, int c)->int {
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
        assert(i == m_num_of_elements);
    }

    glGenBuffers(1, &m_vbo_position);
    glGenBuffers(1, &m_ebo);
    glBindVertexArray(m_VAO_id);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point_arr), point_arr, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elem_arr), elem_arr, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Wave_VAO::~Wave_VAO()
{
    glDeleteBuffers(1, &m_vbo_position);
    glDeleteBuffers(1, &m_ebo);
}

void Wave_VAO::draw()
{
    glBindVertexArray(m_VAO_id);
    glDrawElements(GL_TRIANGLES, m_num_of_elements, GL_UNSIGNED_INT, (void*)0);
}

