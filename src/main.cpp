
#include "WaveWidget.h"

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QTimer>
#include <QCheckBox>
#include <QButtonGroup>
#include <QProgressBar>
#include <QLabel>

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
    vlayout->setStretch(2, 1);
    wave->setMinimumSize(1000, 600);

    // connect
    QObject::connect(normal, &QRadioButton::clicked, wave, &WaveWidget::use_normal_color);
    QObject::connect(reflect, &QRadioButton::clicked, wave, &WaveWidget::use_reflect);
    QObject::connect(refract, &QRadioButton::clicked, wave, &WaveWidget::use_refract);
    QObject::connect(pixel, &QCheckBox::toggled, wave, &WaveWidget::toggle_pixelization);
    QObject::connect(sine_wave, &QRadioButton::clicked, wave, &WaveWidget::use_sine_wave);
    QObject::connect(ripple, &QRadioButton::clicked, wave, &WaveWidget::use_ripple);
    QObject::connect(height_map, &QRadioButton::clicked, wave, &WaveWidget::use_height_map);

    /* Progress Bar */ {
        QWidget* progress_widget = new QWidget;
        QVBoxLayout* vlayout = new QVBoxLayout(progress_widget);
        vlayout->addWidget(new QLabel(u8"載入height map中......"));
        QProgressBar* progress = new QProgressBar;
        vlayout->addWidget(progress);

        progress->setRange(0, 200);
        progress->setValue(0);

        QObject::connect(wave, &WaveWidget::height_map_load, progress_widget, [progress, progress_widget](int id){
            progress->setValue(id);
            if (id == 200) QTimer::singleShot(20, progress_widget, &QWidget::close);
        });

        QTimer::singleShot(10, progress_widget, &QWidget::show);
    }

    QTimer::singleShot(10, window, &QWidget::show);
    return a.exec();
}
