// std c++23
#include <iostream>
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#define FLAG_IMPLEMENTATION
#include "../thirdparty/flag.h"
#define CBPS_CONSTANTS_IMPLEMENTATION
#include "./constants.hpp"
#define NORD_COLORS_IMPLEMENTATION
#include "./nord.hpp"

#ifdef CBPS_CONSTANTS_IMPLEMENTATION

namespace CBPS {
  void print_usage( FILE *stream = stdout ) {
    std::fprintf( stream, "Usage: %s [Options]\n", flag_program_name() );
    CBPS::print_app_info( stdout );
    std::printf( "\nOptions:\n" );
    flag_print_options( stdout );
  }
}

#endif // CBPS_CONSTANTS_IMPLEMENTATION


int main( int argc, char *argv[] ) {
  // Parse command line flags using flag.h
  bool show_version = false;
  bool show_V_version = false;
  bool show_help = false;
  bool show_h_help = false;
  char *title = (char *)"Comboom Punkt Sucht Native Wallpaper";
  char *sub_title = (char *)"";
  uint64_t window_width = 1280;
  uint64_t window_height = 720;


  flag_bool_var( &show_version, "-version", false, "Show version information" );
  flag_bool_var( &show_V_version, "V", false, "Show version information (short)" );
  flag_bool_var( &show_help, "-help", false, "Show help information" );
  flag_bool_var( &show_h_help, "h", false, "Show help information (short)" );
  flag_str_var(
    &title,
    "-title",
    "Comboom Punkt Sucht Native Wallpaper",
    "Set window title" );
    flag_str_var(
      &sub_title,
      "-sub_title",
      "Comboom Punkt Sucht Native Wallpaper - Sub Title",
      "Set window sub title" );
    flag_uint64_var( &window_width, "-width", 1280, "Set window width" );
    flag_uint64_var( &window_height, "-height", 720, "Set window height" );

  if ( !flag_parse( argc, argv ) ) {
    flag_print_error( stderr );
    CBPS::print_usage( stderr );
    return 1;
  }

  if ( show_version || show_V_version ) {
    CBPS::print_app_version();
    return 0;
  }

  if ( show_help || show_h_help ) {
    CBPS::print_usage( stdout );
    return 0;
  }

  // Create Qt6 application
  QApplication app( argc, argv );

  // Create main window
  QMainWindow  mainWindow;
  mainWindow.setWindowTitle( QString::fromUtf8( CBPS::APP_NAME ) );
  mainWindow.resize(
    static_cast< int >( window_width ), static_cast< int >( window_height ) );

  // Create central widget with layout
  QWidget     *centralWidget = new QWidget( &mainWindow );
  QVBoxLayout *layout        = new QVBoxLayout( centralWidget );

  centralWidget->setStyleSheet(
    QString("background-color: %1;").arg(Nord::index_to_hexstr(Nord::NORD0)));

  // Add welcome label
  QLabel      *welcomeLabel =
    new QLabel( "Welcome to Comboom Punkt Sucht!", centralWidget );
  welcomeLabel->setStyleSheet(
    QString("font-size: 24px; font-weight: bold; margin: 20px; color: %1;").arg(Nord::index_to_hexstr(Nord::NORD4)));
  welcomeLabel->setAlignment( Qt::AlignCenter );
  layout->addWidget( welcomeLabel );

  // Add info label
  QLabel *infoLabel = new QLabel(
    "This is a basic C++23 Qt6 application with CMake", centralWidget );
  infoLabel->setAlignment( Qt::AlignCenter );
  layout->addWidget( infoLabel );

  // Add flag info label
  QLabel *flagLabel = new QLabel(
    QString("Using tsoding/flag.h for argument parsing\nWindow size: %1x%2").arg(QString::number(window_width)).arg(QString::number(window_height)),
    centralWidget );
  flagLabel->setAlignment( Qt::AlignCenter );
  flagLabel->setStyleSheet( QString("margin: 10px; color: %1;").arg(Nord::index_to_hexstr(Nord::NORD8)));
  layout->addWidget( flagLabel );

  // Add stretch to push content to top
  layout->addStretch( );

  // Add quit button
  QPushButton *quitButton = new QPushButton( "Quit", centralWidget );
  quitButton->setMaximumWidth( 200 );
  layout->addWidget( quitButton, 0, Qt::AlignCenter );

  QObject::connect(
    quitButton, &QPushButton::clicked, &app, &QApplication::quit );

  // Set central widget
  mainWindow.setCentralWidget( centralWidget );

  // Show the window
  mainWindow.show( );

  // Run the application event loop
  return app.exec( );
}