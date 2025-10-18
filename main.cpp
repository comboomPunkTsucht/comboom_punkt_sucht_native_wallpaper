#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <iostream>

#define FLAG_IMPLEMENTATION
#include "flag.h"

int main(int argc, char *argv[])
{
    // Parse command line flags using flag.h
    bool *show_version = flag_bool("version", false, "Show version information");
    char **window_title = flag_str("title", "Comboom Punkt Sucht Native Wallpaper", "Set window title");
    uint64_t *window_width = flag_uint64("width", 800, "Set window width");
    uint64_t *window_height = flag_uint64("height", 600, "Set window height");
    
    if (!flag_parse(argc, argv)) {
        flag_print_error(stderr);
        flag_print_options(stderr);
        return 1;
    }
    
    if (*show_version) {
        std::cout << "Comboom Punkt Sucht Native Wallpaper v1.0.0" << std::endl;
        std::cout << "C++23 Qt6 Application" << std::endl;
        return 0;
    }
    
    // Create Qt6 application
    QApplication app(argc, argv);
    
    // Create main window
    QMainWindow mainWindow;
    mainWindow.setWindowTitle(QString::fromUtf8(*window_title));
    mainWindow.resize(static_cast<int>(*window_width), static_cast<int>(*window_height));
    
    // Create central widget with layout
    QWidget *centralWidget = new QWidget(&mainWindow);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    // Add welcome label
    QLabel *welcomeLabel = new QLabel("Welcome to Comboom Punkt Sucht!", centralWidget);
    welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin: 20px;");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(welcomeLabel);
    
    // Add info label
    QLabel *infoLabel = new QLabel("This is a basic C++23 Qt6 application with CMake", centralWidget);
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoLabel);
    
    // Add flag info label
    QLabel *flagLabel = new QLabel(QString("Using tsoding/flag.h for argument parsing\nWindow size: %1x%2")
                                        .arg(*window_width)
                                        .arg(*window_height), 
                                    centralWidget);
    flagLabel->setAlignment(Qt::AlignCenter);
    flagLabel->setStyleSheet("margin: 10px; color: #666;");
    layout->addWidget(flagLabel);
    
    // Add stretch to push content to top
    layout->addStretch();
    
    // Add quit button
    QPushButton *quitButton = new QPushButton("Quit", centralWidget);
    quitButton->setMaximumWidth(200);
    layout->addWidget(quitButton, 0, Qt::AlignCenter);
    
    QObject::connect(quitButton, &QPushButton::clicked, &app, &QApplication::quit);
    
    // Set central widget
    mainWindow.setCentralWidget(centralWidget);
    
    // Show the window
    mainWindow.show();
    
    // Run the application event loop
    return app.exec();
}
