#include <ui/runner.hpp>
#include <QApplication>
#include <ui/pw_screen.hpp>
#include <QStackedWidget>

#include "main_screen.hpp"
#include "settings_screen.hpp"
#include "constants/paths.hpp"

Runner::Runner(MainController &controller) : controller_(controller) {}

void Runner::run()
{
    QStackedWidget *stack = new QStackedWidget;

    QWidget* window = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(window);

    QSoundEffect* type_sound = new QSoundEffect(window);
    type_sound->setSource(QUrl::fromLocalFile((cnt::getAssetsBasePath()/"sounds"/"type.wav").c_str()));
    // todo button press sounds

    PWScreen* pw_screen = new PWScreen(controller_, type_sound, stack);
    MainScreen* main_screen = new MainScreen(controller_, stack);
    SettingsScreen* settings_screen = new SettingsScreen(controller_, stack);

    window->setLayout(mainLayout);

    stack->addWidget(pw_screen);
    stack->addWidget(main_screen);
    stack->addWidget(settings_screen);

    QObject::connect(pw_screen, &PWScreen::unlocked, main_screen, &MainScreen::refresh);

    QObject::connect(pw_screen, &PWScreen::unlocked, stack, [stack, main_screen]()
    {
        stack->setCurrentWidget(main_screen);
    });

    QObject::connect(main_screen, &MainScreen::settings, stack, [stack, settings_screen]()
    {
        stack->setCurrentWidget(settings_screen);
    });

    QObject::connect(settings_screen, &SettingsScreen::done, stack, [stack, main_screen]()
    {
        stack->setCurrentWidget(main_screen);
    });

    mainLayout->addWidget(stack);

    window->show();
}
