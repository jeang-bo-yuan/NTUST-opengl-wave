
#include "WaveWidget.h"

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QTimer>
#include <QCheckBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget* window = new QWidget;
    QVBoxLayout* vlayout = new QVBoxLayout(window);
    vlayout->setMargin(0);

    QHBoxLayout* buttons = new QHBoxLayout;
    buttons->setAlignment(Qt::AlignCenter);
    buttons->setMargin(20);
    buttons->setSpacing(50);
    vlayout->addLayout(buttons);
    QRadioButton* normal = new QRadioButton("Use Normal Color");
    QRadioButton* reflect = new QRadioButton("Reflect");
    QRadioButton* refract = new QRadioButton("Refract");
    buttons->addWidget(normal);
    buttons->addWidget(reflect);
    buttons->addWidget(refract);
    normal->setChecked(true);

    QCheckBox* pixel = new QCheckBox("Pixelization");
    pixel->setChecked(false);
    buttons->addWidget(pixel);

    WaveWidget* wave = new WaveWidget;
    vlayout->addWidget(wave);
    vlayout->setStretch(1, 1);
    wave->setMinimumSize(1000, 600);

    // connect
    QObject::connect(normal, &QRadioButton::clicked, wave, &WaveWidget::use_normal_color);
    QObject::connect(reflect, &QRadioButton::clicked, wave, &WaveWidget::use_reflect);
    QObject::connect(refract, &QRadioButton::clicked, wave, &WaveWidget::use_refract);
    QObject::connect(pixel, &QCheckBox::toggled, wave, &WaveWidget::toggle_pixelization);

    QTimer::singleShot(0, window, &QWidget::show);
    return a.exec();
}
