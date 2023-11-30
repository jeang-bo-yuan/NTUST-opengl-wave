
#include "WaveWidget.h"

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QTimer>
#include <QCheckBox>
#include <QButtonGroup>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget* window = new QWidget;
    QVBoxLayout* vlayout = new QVBoxLayout(window);
    vlayout->setMargin(0);

    // 使用的顏色
    QHBoxLayout* buttons = new QHBoxLayout;
    buttons->setAlignment(Qt::AlignCenter);
    buttons->setMargin(20);
    buttons->setSpacing(50);
    vlayout->addLayout(buttons);
    QButtonGroup* group_color = new QButtonGroup(window);
    QRadioButton* normal = new QRadioButton("Use Normal Color");
    QRadioButton* reflect = new QRadioButton("Reflect");
    QRadioButton* refract = new QRadioButton("Refract");
    group_color->addButton(normal);
    group_color->addButton(reflect);
    group_color->addButton(refract);
    buttons->addWidget(normal);
    buttons->addWidget(reflect);
    buttons->addWidget(refract);
    normal->setChecked(true);

    // 像素化
    QCheckBox* pixel = new QCheckBox("Pixelization");
    pixel->setChecked(false);
    buttons->addWidget(pixel);

    // 選擇波形
    QHBoxLayout* buttons2 = new QHBoxLayout;
    buttons2->setAlignment(Qt::AlignCenter);
    buttons2->setMargin(10);
    buttons2->setSpacing(50);
    vlayout->addLayout(buttons2);
    QRadioButton* sine_wave = new QRadioButton("Sine Wave");
    QRadioButton* ripple = new QRadioButton("Ripple");
    QRadioButton* height_map = new QRadioButton("Height Map");
    buttons2->addWidget(sine_wave);
    buttons2->addWidget(ripple);
    buttons2->addWidget(height_map);
    sine_wave->setChecked(true);

    // 顯示wave
    WaveWidget* wave = new WaveWidget;
    vlayout->addWidget(wave);
    vlayout->setStretch(1, 1);
    wave->setMinimumSize(1000, 600);

    // connect
    QObject::connect(normal, &QRadioButton::clicked, wave, &WaveWidget::use_normal_color);
    QObject::connect(reflect, &QRadioButton::clicked, wave, &WaveWidget::use_reflect);
    QObject::connect(refract, &QRadioButton::clicked, wave, &WaveWidget::use_refract);
    QObject::connect(pixel, &QCheckBox::toggled, wave, &WaveWidget::toggle_pixelization);
    QObject::connect(sine_wave, &QRadioButton::clicked, wave, &WaveWidget::use_sine_wave);
    QObject::connect(ripple, &QRadioButton::clicked, wave, &WaveWidget::use_ripple);
    QObject::connect(height_map, &QRadioButton::clicked, wave, &WaveWidget::use_height_map);

    QTimer::singleShot(0, window, &QWidget::show);
    return a.exec();
}
