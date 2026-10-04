use gtk::gio::Icon;
use gtk::glib::{GStr, GString};
use relm4::gtk;

pub trait Selectable {
    fn selected(&mut self);
}

pub trait SubEntry: Selectable {
    fn label(&self) -> GString;
}

pub trait Entry: Selectable {
    fn icon(&self) -> Option<Icon>;
    fn label(&self) -> GString;
    fn confidence(&self) -> i32;
    fn sub_entries(&mut self) -> Vec<&mut dyn SubEntry>;
}

pub struct PluginInfo {
    pub name: GString,
}

pub trait Plugin {
    fn entries(&mut self, input: &GStr) -> Vec<&mut dyn Entry>;
    fn info(&self) -> PluginInfo;
}
