
#include "WaveWidget.h"

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>

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


    WaveWidget* wave = new WaveWidget;
    vlayout->addWidget(wave);
    wave->setMinimumSize(1000, 600);

    // connect
    QObject::connect(normal, &QRadioButton::clicked, wave, &WaveWidget::use_normal_color);
    QObject::connect(reflect, &QRadioButton::clicked, wave, &WaveWidget::use_reflect);
    QObject::connect(refract, &QRadioButton::clicked, wave, &WaveWidget::use_refract);

    window->show();
    return a.exec();
}
