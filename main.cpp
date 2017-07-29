#include "jifswidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    JifsWidget widget;
    widget.show();
    return app.exec();
}
