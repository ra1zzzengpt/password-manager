#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Hello World!");

    QVBoxLayout main_layout(&window);

    QLabel label("Password-Manager");

    label.setAlignment(Qt::AlignCenter);

    main_layout.addWidget(&label);

    window.show();
    return QApplication::exec();
}