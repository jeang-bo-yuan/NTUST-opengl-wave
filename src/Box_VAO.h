/**
 * @file Box_VAO.h
 * @brief 繪制方塊時用的VAO
 */
#ifndef BOX_VAO_H
#define BOX_VAO_H

#include "VAO_Interface.h"


/// 繪制方塊時用的VAO
/// @details
/// 提供的attribute:
/// - (location=0) 頂點的位置(-size, -size, -size) ~ (size, size, size)
/// - (location=2) 每面的法向量（從(0,0,0)向外指）
class Box_VAO : public VAO_Interface
{
private:
    // 包含點的座標及每面的法向量
    GLuint m_vbo;

public:
    /// @brief constructor，生成VBO、EBO
    /// @param size - 方塊的大小
    Box_VAO(GLfloat size);

    /// 呼叫 glDeleteBuffers
    virtual ~Box_VAO();

    /// 畫出來，方塊的範圍：(-size, -size, -size) ~ (size, size, size)
    /// @details 畫出方塊的每一面
    void draw() override;

    /// 繪製一個沒有「蓋子」的方塊
    void draw_without_top();
};

#endif // BOX_VAO_H
