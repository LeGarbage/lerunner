use crate::plugin::{Entry, Plugin, PluginInfo, Selectable, SubEntry};
use gtk::gio::{AppInfo, DesktopAppInfo};
use gtk::glib::{self, GStr, GString};
use relm4::gtk;
use relm4::gtk::gio::Icon;
use relm4::gtk::gio::prelude::AppInfoExt;

pub struct DesktopAction {
    desktop_entry: DesktopAppInfo,
    action_name: GString,
}

impl DesktopAction {
    pub const fn new(desktop_entry: DesktopAppInfo, action_name: GString) -> Self {
        Self {
            desktop_entry,
            action_name,
        }
    }
}

impl Selectable for DesktopAction {
    fn selected(&mut self) {
        _ = glib::spawn_command_line_async(format!(
            "uwsm-app -s app.slice -- {}:{}",
            self.desktop_entry.id().unwrap_or_default(),
            self.action_name
        ));
    }
}

impl SubEntry for DesktopAction {
    fn label(&self) -> GString {
        self.desktop_entry.action_name(&self.action_name)
    }
}

pub struct DesktopEntry {
    #[allow(clippy::struct_field_names)]
    desktop_entry: DesktopAppInfo,
    desktop_actions: Vec<DesktopAction>,
    confidence: i32,
}

impl DesktopEntry {
    fn new(desktop_entry: DesktopAppInfo) -> Self {
        let desktop_actions = desktop_entry
            .list_actions()
            .into_iter()
            .map(|action_name| DesktopAction::new(desktop_entry.clone(), action_name))
            .collect();

        Self {
            desktop_entry,
            desktop_actions,
            confidence: i32::default(),
        }
    }
}

impl Selectable for DesktopEntry {
    fn selected(&mut self) {
        _ = glib::spawn_command_line_async(format!(
            "uwsm-app -s app.slice -- {}",
            self.desktop_entry.id().unwrap_or_default()
        ));
    }
}

impl Entry for DesktopEntry {
    fn icon(&self) -> Option<Icon> {
        self.desktop_entry.icon()
    }

    fn label(&self) -> GString {
        self.desktop_entry.display_name()
    }

    fn confidence(&self) -> i32 {
        self.confidence
    }

    fn sub_entries(&mut self) -> Vec<&mut dyn SubEntry> {
        self.desktop_actions
            .iter_mut()
            .map(|action| action as &mut dyn SubEntry)
            .collect()
    }
}

pub struct DesktopEntries {
    desktop_entries: Vec<DesktopEntry>,
}

impl DesktopEntries {
    pub fn new() -> Self {
        let apps = AppInfo::all()
            .iter()
            .filter(|app| app.should_show())
            .filter_map(|app| {
                Some(DesktopEntry::new(DesktopAppInfo::new(
                    &app.id().unwrap_or_default(),
                )?))
            })
            .collect();
        Self {
            desktop_entries: apps,
        }
    }
}

impl Plugin for DesktopEntries {
    fn entries(&mut self, input: &GStr) -> Vec<&mut dyn Entry> {
        self.desktop_entries
            .iter_mut()
            .map(|entry| entry as &mut dyn Entry)
            .collect()
    }

    fn info(&self) -> PluginInfo {
        PluginInfo {
            name: "Applications".into(),
        }
    }
}
