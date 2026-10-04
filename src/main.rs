mod entry_button;
mod plugin;
mod plugins;
use entry_button::EntryButton;
use gtk::gdk::Key;
use gtk::glib::{GString, Propagation};
use gtk::prelude::*;
use relm4::factory::FactoryVecDeque;
use relm4::{Component, ComponentParts, ComponentSender, RelmApp, RelmWidgetExt, gtk};

struct Entries {
    selected_index: usize,
    entry_buttons: FactoryVecDeque<EntryButton>,
}

#[derive(Debug)]
enum EntryMsg {
    Changed(GString),
    Activated,
    UpSelectedButton,
    DownSelectedButton,
    SetSelectedButton,
    ToggleSelectedButton,
    Close,
}

#[relm4::component]
impl Component for Entries {
    type Init = u8;

    type Input = EntryMsg;
    type Output = ();
    type CommandOutput = ();

    view! {
        gtk::Window {
            set_title: Some("Simple app"),
            set_default_width: 640,
            set_default_height: -1,

            gtk::Box {
                set_orientation: gtk::Orientation::Vertical,

                gtk::Entry {
                    set_placeholder_text: Some("Search"),
                    set_icon_from_icon_name: (gtk::EntryIconPosition::Primary, Some("search-symbolic")),
                    connect_changed[sender] => move |entry| { sender.input(EntryMsg::Changed(entry.text())); },
                    connect_activate => EntryMsg::Activated,
                },

                gtk::Box {
                    set_orientation: gtk::Orientation::Vertical,
                    // TODO: Add generator for buttons
                },

                gtk::Label {
                    #[watch]
                    set_label: &format!("Counter: {}", model.counter),
                    set_margin_all: 5,
                }
            }
        }
    }

    // Initialize the UI.
    fn init(
        counter: Self::Init,
        root: Self::Root,
        sender: ComponentSender<Self>,
    ) -> ComponentParts<Self> {
        let model = Self { counter };

        // Insert the macro code generation here
        let widgets = view_output!();

        let key_controller = gtk::EventControllerKey::new();
        let key_sender = sender.clone();
        key_controller.connect_key_pressed(move |_, key, _, _| {
            if key == Key::Escape {
                key_sender.input(EntryMsg::Close);
                Propagation::Stop
            } else if key == Key::Up {
                key_sender.input(EntryMsg::UpSelectedButton);
                Propagation::Stop
            } else if key == Key::Down {
                key_sender.input(EntryMsg::DownSelectedButton);
                Propagation::Stop
            } else if key == Key::Tab {
                key_sender.input(EntryMsg::ToggleSelectedButton);
                Propagation::Stop
            } else {
                Propagation::Proceed
            }
        });
        root.add_controller(key_controller);

        let focus_controller = gtk::EventControllerFocus::new();
        focus_controller.connect_leave(move |_| {
            sender.input(EntryMsg::Close);
        });
        root.add_controller(focus_controller);

        ComponentParts { model, widgets }
    }

    fn update(&mut self, msg: Self::Input, _sender: ComponentSender<Self>, root: &Self::Root) {
        match msg {
            EntryMsg::Changed(text) => {
                self.counter = self
                    .counter
                    .wrapping_add(u8::try_from(text.len()).unwrap_or_default());
            }
            EntryMsg::Activated => {
                self.counter = self.counter.wrapping_sub(1);
            }
            EntryMsg::Close => root.close(),
            _ => todo!(),
        }
    }
}

fn main() {
    let app = RelmApp::new("relm4.test.simple");
    app.run::<Entries>(0);
}
