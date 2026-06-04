#pragma once

#include <gtkmm/button.h>
#include <gtkmm/window.h>

class MainWindow : public Gtk::Window {
    public:
    MainWindow();
    ~MainWindow() override;

    private:
    void on_button_clicked();
    Gtk::Button button;
};
