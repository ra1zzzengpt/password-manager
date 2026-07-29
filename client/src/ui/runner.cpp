#include "runner.hpp"
#include <QApplication>
#include "pw_screen.hpp"
#include <QVBoxLayout>
#include <QStackedWidget>

#include "main_screen.hpp"

Runner::Runner(MainController &controller) : controller_(controller) {}

void Runner::run(int argc, char *argv[])
{
    QApplication app{argc, argv};
    PWScreen pw_screen{controller_};
    MainScreen main_screen{controller_};

    QStackedWidget *stack = new QStackedWidget;

    QWidget* window = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(window);

    window->setLayout(mainLayout);

    stack->addWidget(pw_screen.build(window,stack));
    stack->addWidget(main_screen.build(window,stack));

    mainLayout->addWidget(stack);
    window->show();
    QApplication::exec();
}
