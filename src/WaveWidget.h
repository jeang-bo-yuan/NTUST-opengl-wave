/**
 * @file WaveWidget.h
 * @brief 顯示sine wave
 */
#ifndef WAVEWIDGET_H
#define WAVEWIDGET_H

#include <glad/gl.h>
#include <QOpenGLWidget>
#include <QTimer>
#include <Shader.h>

#include <memory>

#include "ArcBall.h"
#include "Wave_VAO.h"
#include "Box_VAO.h"
#include "qtTextureCubeMap.h"

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
    /// 初始化
    void initializeGL() override;
    /// 調整視窗大小時，重新設置perspective projection matrix
    void resizeGL(int w, int h) override;
    /// 繪製sine wave
    void paintGL() override;

    /// 滑鼠點擊時，記下點的位置+開啟mouse tracking
    void mousePressEvent(QMouseEvent*) override;
    /// 移動滑鼠時，看移動多少去調整Arc Ball的角度
    void mouseMoveEvent(QMouseEvent*) override;
    /// 鬆開滑鼠時，關閉mouse tracking
    void mouseReleaseEvent(QMouseEvent*) override;

    /// 滾動滑鼠滾輪時，放大或縮小
    void wheelEvent(QWheelEvent*) override;

private:
    /// 為目前的shader設置uniform data
    void set_uniform_data();

private:
    // sine wave
    std::unique_ptr<Wave_VAO> m_wave_VAO_p;
    std::unique_ptr<Shader> m_shader_p;

    // skybox
    std::unique_ptr<Box_VAO> m_skybox_VAO_p;
    std::unique_ptr<qtTextureCubeMap> m_texture_cube_map_p;
    std::unique_ptr<Shader> m_skybox_shader_p;

    /// 每隔一段時間會更新這個widget一次
    QTimer m_timer;

    /// projection matrix，在 set_uniform_data 中傳給shader
    glm::mat4x4 m_proj_matrix;
    /// 第幾幀，在 set_uniform_data 中傳給shader
    GLuint m_frame;

    /// 記錄視角的球面座標
    ArcBall m_Arc_Ball;

    /// 上次滑鼠點的位置
    QPoint m_last_clicked;
    /// 上次點擊時，alpha的值
    float m_last_alpha;
    /// 上次點擊時，beta的值
    float m_last_beta;

    /// 視角（眼睛的位置）有沒有動過
    bool m_eye_pos_changed;
    /// perspective projection matrix有沒有動過
    bool m_perspective_changed;
};

#endif // WAVEWIDGET_H
