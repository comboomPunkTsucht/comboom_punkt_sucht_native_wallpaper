#ifdef __linux__

#include "system_tray.hpp"
#include <libappindicator/app-indicator.h>
#include <gtk/gtk.h>
#include <memory>
#include <iostream>

class SystemTrayLinux : public SystemTray {
private:
    AppIndicator* indicator = nullptr;
    GtkWidget* menu = nullptr;
    bool quit_flag = false;
    std::string h1_text;
    std::string h2_text;
    std::string app_name;

    TextInputCallback h1_callback;
    TextInputCallback h2_callback;
    QuitCallback quit_callback;

    static void on_h1_activate(GtkMenuItem* menuitem, gpointer user_data);
    static void on_h2_activate(GtkMenuItem* menuitem, gpointer user_data);
    static void on_quit_activate(GtkMenuItem* menuitem, gpointer user_data);

    bool show_input_dialog(const std::string& title, std::string& input_text);

public:
    SystemTrayLinux() = default;
    ~SystemTrayLinux() override;

    bool init(const std::string& app_name_str, const std::string& icon_path) override;
    void set_h1_callback(TextInputCallback callback) override { h1_callback = callback; }
    void set_h2_callback(TextInputCallback callback) override { h2_callback = callback; }
    void set_quit_callback(QuitCallback callback) override { quit_callback = callback; }

    void set_h1_text(const std::string& text) override;
    void set_h2_text(const std::string& text) override;

    void run() override;
    void quit() override { quit_flag = true; }
    bool should_quit() const override { return quit_flag; }

    // Used by static callbacks
    void handle_h1();
    void handle_h2();
    void handle_quit();
};

// Static pointers for callbacks
static SystemTrayLinux* g_tray_instance = nullptr;

void SystemTrayLinux::on_h1_activate(GtkMenuItem* menuitem, gpointer user_data) {
    if (g_tray_instance) {
        g_tray_instance->handle_h1();
    }
}

void SystemTrayLinux::on_h2_activate(GtkMenuItem* menuitem, gpointer user_data) {
    if (g_tray_instance) {
        g_tray_instance->handle_h2();
    }
}

void SystemTrayLinux::on_quit_activate(GtkMenuItem* menuitem, gpointer user_data) {
    if (g_tray_instance) {
        g_tray_instance->handle_quit();
    }
}

bool SystemTrayLinux::init(const std::string& app_name_str, const std::string& icon_path) {
    app_name = app_name_str;
    h1_text = "comboom.sucht";
    h2_text = "Live Wallpaper";
    g_tray_instance = this;

    // Initialize GTK if not already done
    if (!gtk_init_check(nullptr, nullptr)) {
        std::cerr << "Failed to initialize GTK" << std::endl;
        return false;
    }

    // Create AppIndicator
    indicator = app_indicator_new(
        "comboom-sucht-live-wallpaper",
        "dialog-information",
        APP_INDICATOR_CATEGORY_APPLICATION_STATUS
    );

    if (!indicator) {
        std::cerr << "Failed to create AppIndicator" << std::endl;
        return false;
    }

    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);

    // Create menu
    menu = gtk_menu_new();

    // Title (inactive)
    GtkWidget* title_item = gtk_menu_item_new_with_label(app_name.c_str());
    gtk_widget_set_sensitive(title_item, FALSE);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), title_item);

    // Separator
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

    // H1 edit button
    GtkWidget* h1_item = gtk_menu_item_new_with_label("Edit H1 (Title)");
    g_signal_connect(h1_item, "activate", G_CALLBACK(on_h1_activate), nullptr);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), h1_item);

    // H2 edit button
    GtkWidget* h2_item = gtk_menu_item_new_with_label("Edit H2 (Subtitle)");
    g_signal_connect(h2_item, "activate", G_CALLBACK(on_h2_activate), nullptr);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), h2_item);

    // Separator
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

    // Quit button
    GtkWidget* quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(on_quit_activate), nullptr);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit_item);

    // Show all menu items
    gtk_widget_show_all(menu);

    // Set menu to indicator
    app_indicator_set_menu(indicator, GTK_MENU(menu));

    return true;
}

void SystemTrayLinux::set_h1_text(const std::string& text) {
    h1_text = text;
}

void SystemTrayLinux::set_h2_text(const std::string& text) {
    h2_text = text;
}

bool SystemTrayLinux::show_input_dialog(const std::string& title, std::string& input_text) {
    GtkWidget* dialog = gtk_dialog_new_with_buttons(
        title.c_str(),
        nullptr,
        GTK_DIALOG_MODAL,
        "_OK", GTK_RESPONSE_OK,
        "_Cancel", GTK_RESPONSE_CANCEL,
        nullptr
    );

    GtkWidget* content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget* label = gtk_label_new("Enter text:");
    gtk_container_add(GTK_CONTAINER(content_area), label);

    GtkWidget* entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), input_text.c_str());
    gtk_container_add(GTK_CONTAINER(content_area), entry);

    gtk_widget_show_all(dialog);

    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    bool result = false;

    if (response == GTK_RESPONSE_OK) {
        input_text = gtk_entry_get_text(GTK_ENTRY(entry));
        result = true;
    }

    gtk_widget_destroy(dialog);

    return result;
}

void SystemTrayLinux::handle_h1() {
    std::string new_text = h1_text;
    if (show_input_dialog("Edit H1 (Title)", new_text)) {
        set_h1_text(new_text);
        if (h1_callback) {
            h1_callback(new_text);
        }
    }
}

void SystemTrayLinux::handle_h2() {
    std::string new_text = h2_text;
    if (show_input_dialog("Edit H2 (Subtitle)", new_text)) {
        set_h2_text(new_text);
        if (h2_callback) {
            h2_callback(new_text);
        }
    }
}

void SystemTrayLinux::handle_quit() {
    quit_flag = true;
    if (quit_callback) {
        quit_callback();
    }
}

void SystemTrayLinux::run() {
    while (!quit_flag) {
        // Process GTK events
        while (gtk_events_pending()) {
            gtk_main_iteration();
        }

        // Small sleep to avoid busy-waiting
        g_usleep(10000);  // 10ms
    }

    // Cleanup
    if (indicator) {
        g_object_unref(indicator);
        indicator = nullptr;
    }
    if (menu) {
        g_object_unref(menu);
        menu = nullptr;
    }
    g_tray_instance = nullptr;
}

SystemTrayLinux::~SystemTrayLinux() {
    if (indicator) {
        g_object_unref(indicator);
    }
    if (menu) {
        g_object_unref(menu);
    }
    g_tray_instance = nullptr;
}

std::unique_ptr<SystemTray> SystemTray::create() {
    return std::make_unique<SystemTrayLinux>();
}

#endif // __linux__
