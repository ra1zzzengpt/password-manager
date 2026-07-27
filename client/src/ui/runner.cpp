#include "runner.hpp"
#include <QApplication>
#include "pw_screen.hpp"
#include <QVBoxLayout>

Runner::Runner(MainController &controller) : controller_(controller) {}

void Runner::run(int argc, char *argv[])
{
    PWScreen pw_screen{controller_};
    QApplication app{argc, argv};
    QWidget* window = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(window);
    window->setLayout(mainLayout);
    mainLayout->addWidget(pw_screen.build());
    window->show();
    QApplication::exec();
}
