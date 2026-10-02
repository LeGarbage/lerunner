#include "plugin.hpp"
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <memory>
#include <sigc++/signal.h>
#include <vector>

class EntryButtonBase : public Gtk::Button {
    public:
    using type_signal_hovered = sigc::signal<void(EntryButtonBase &)>;
    type_signal_hovered signal_hovered();

    private:
    type_signal_hovered m_signal_hovered;
};

class EntryButton : public EntryButtonBase {
    public:
    EntryButton(std::shared_ptr<Entry> entry);

    std::vector<std::shared_ptr<SubEntry>> get_sub_entries() const;

    void on_clicked() override;

    void add_plugin_label(Plugin &plugin);

    private:
    std::shared_ptr<Entry> m_entry;

    Gtk::Box *m_plugin_box;
};

class SubEntryButton : public EntryButtonBase {
    public:
    SubEntryButton(std::shared_ptr<SubEntry> sub_entry);

    void on_clicked() override;

    private:
    std::shared_ptr<SubEntry> m_sub_entry;
};
