#include <QVBoxLayout>
#include <QLabel>
#include <ui/pw_screen.hpp>

PWScreen::PWScreen(MainController &controller) : controller_(controller) { }

QWidget *PWScreen::build()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel* label = new QLabel("Will be good!!!");
    layout->addWidget(label);
    return widget;
}
