#ifndef EMUDISPLAYWIDGET_H
#define EMUDISPLAYWIDGET_H

#include <QWidget>
#include <QColor>

#include <typedefs.h>

class EmuDisplayWidget : public QWidget
{
    Q_OBJECT
private:
    u8* workingFrame = nullptr;

    int pixelSize = 24;

public:
    explicit EmuDisplayWidget(QWidget *parent = nullptr);

    QColor pixOffColor = Qt::black;
    QColor pixOnColor = Qt::white;

public slots:
    void draw(u8* _frame);

protected:
    void paintEvent(QPaintEvent* _event) override;
};

#endif // EMUDISPLAYWIDGET_H
