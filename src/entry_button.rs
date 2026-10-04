use crate::plugin::Entry;
use gtk::prelude::*;
use relm4::factory::FactoryComponent;
use relm4::gtk;
use std::rc::Rc;

pub struct EntryButton {
    entry: Rc<dyn Entry>,
}

enum EntryButtonMsg {
    Selected,
}

enum EntryButtonOutput {
    SetSelection,
}

#[relm4::factory]
impl FactoryComponent for EntryButton {
    view! {
        #[root]
        gtk::Button {
            set_can_focus: false,

            gtk::Box {
                set_spacing: 40,

                gtk::Box {
                    set_homogeneous: true,
                },

                gtk::Box {
                    set_spacing: 10,

                    if &self.entry.sub_entries().len() > &0 {
                        gtk::Image {
                            set_icon_name: Some("arrow-right-symbolic")
                        }
                    } else {
                        gtk::Image {}
                    },

                    match &self.entry.icon() {
                        Some(icon) => gtk::Image {
                            #[watch]
                            set_from_gicon: icon,
                        }
                        None => gtk::Image {}
                    },

                    gtk::Label {
                        set_label: &self.entry.label()
                    }
                }
            }
        }
    }
}
