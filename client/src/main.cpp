#include <ui/runner.hpp>


int main(int argc, char* argv[])
{
    MainController mainController;
    Runner runner{mainController};
    runner.run(argc, argv);
}