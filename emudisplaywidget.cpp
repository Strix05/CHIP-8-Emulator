#include "emudisplaywidget.h"

#include <QPainter>

EmuDisplayWidget::EmuDisplayWidget(QWidget *parent)
    : QWidget{parent}
{}

void EmuDisplayWidget::draw(u8 *_frame)
{
    workingFrame = _frame;
    update();
}

void EmuDisplayWidget::paintEvent(QPaintEvent *_event)
{
    if(workingFrame != nullptr){
        QWidget::paintEvent(_event);
        QPainter painter(this);
        for(int y = 0; y < 32; ++y){
            for(int x = 0; x < 64; ++x){
                if(workingFrame[y * 64 + x] == 0x0){
                    painter.fillRect(x * pixelSize, y * pixelSize, pixelSize, pixelSize, pixOffColor);
                }
                else{
                    painter.fillRect(x * pixelSize, y * pixelSize, pixelSize, pixelSize, pixOnColor);
                }
            }
        }
    }
}
