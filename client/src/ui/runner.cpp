#include <ui/runner.hpp>
#include <QApplication>
#include <QMessageBox>
#include <ui/pw_screen.hpp>
#include <QStackedWidget>

#include "main_screen.hpp"

Runner::Runner(MainController &controller) : controller_(controller) {}

void Runner::run(int argc, char *argv[])
{
    QApplication app{argc, argv};
    QStackedWidget *stack = new QStackedWidget;

    PWScreen* pw_screen = new PWScreen(controller_, stack);
    MainScreen* main_screen = new MainScreen(controller_, stack);

    QWidget* window = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(window);

    window->setLayout(mainLayout);

    stack->addWidget(pw_screen);
    stack->addWidget(main_screen);

    QObject::connect(pw_screen, &PWScreen::unlocked, main_screen, &MainScreen::refresh);

    QObject::connect(pw_screen, &PWScreen::unlocked, stack, [stack, main_screen]()
    {
        stack->setCurrentWidget(main_screen);
    });

    mainLayout->addWidget(stack);

    window->show();
    QApplication::exec();
}
