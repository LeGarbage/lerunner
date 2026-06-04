#include "window.hpp"

MainWindow::MainWindow()
    : button("Close") {
    set_decorated(false);

    set_default_size(800, 75);

    button.set_margin(10);
    button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked));
    set_child(button);
}

MainWindow::~MainWindow() = default;

void MainWindow::on_button_clicked() {
    close();
}
