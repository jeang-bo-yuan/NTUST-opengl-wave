/**
 * @file WaveWidget.h
 * @brief 顯示sine wave
 */
#ifndef WAVEWIDGET_H
#define WAVEWIDGET_H

#include <glad/gl.h>
#include <QOpenGLWidget>
#include <QTimer>

#include <memory>

#include "ArcBall.h"
#include "DynamicHeightMap.h"
#include "Wave_VAO.h"
#include "Box_VAO.h"
#include "qtTextureCubeMap.h"
#include "UBO.h"
#include "Shader.h"
#include "Plane_VAO.h"

/**
 * @brief An OpenGL Widget that display sine-wave water
 */
class WaveWidget : public QOpenGLWidget
{
    Q_OBJECT

public slots:
    /// 使用原本的方法著色
    void use_normal_color();
    /// 讓水波反射cube map
    void use_reflect();
    /// 讓水波折射cube map
    void use_refract();

    /**
     * @brief 開啟或關閉pixelization
     * @param on - 為true時開啟
     */
    void toggle_pixelization(bool on);

public:
    WaveWidget(QWidget *parent = nullptr);

    /// makeCurrent then free up resources
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
    /// 將view matrix更新到UBO內
    void set_view_matrix_from_arc_ball();

private:
    // UBO
    std::unique_ptr<UBO> m_matrices_UBO_p;

    // sine wave
    std::unique_ptr<Wave_VAO> m_wave_VAO_p;
    std::unique_ptr<Shader> m_shader_p;
    std::unique_ptr<DynamicHeightMap> m_DHM_p;

    // skybox
    std::unique_ptr<Box_VAO> m_skybox_VAO_p;
    std::unique_ptr<qtTextureCubeMap> m_texture_cube_map_p;
    std::unique_ptr<Shader> m_skybox_shader_p;

    // Framebuffer & pixelization
    GLuint m_frame_buffer;
    GLuint m_color_texture;
    GLuint m_depth_stencil_rbo;
    std::unique_ptr<Plane_VAO> m_plane_VAO_p;
    std::unique_ptr<Shader> m_pixelization_shader_p;

    /// 每隔一段時間會更新這個widget一次
    QTimer m_timer;

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

    /// 是否要開啟pixelization
    bool m_pixelization;
};

#endif // WAVEWIDGET_H
