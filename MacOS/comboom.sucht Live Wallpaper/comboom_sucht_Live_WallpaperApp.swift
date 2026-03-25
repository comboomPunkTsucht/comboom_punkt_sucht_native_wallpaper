//
//  comboom_sucht_Live_WallpaperApp.swift
//  comboom.sucht Live Wallpaper
//
//  Created by mcpeaps_HD on 25/03/2026.
//

import SwiftUI
@main
struct comboom_sucht_Live_WallpaperApp: App {
  @StateObject private var engineManager = EngineManager()

  var body: some Scene {
    MenuBarExtra("comboom.sucht Live Wallpaper", systemImage: "menubar.dock.rectangle") {
      VStack(alignment: .leading, spacing: 12) {
        Text("comboom.sucht Live Wallpaper")
          .font(.headline)

        Divider()

        TextField("H1", text: $engineManager.h1)
          .textFieldStyle(.roundedBorder)

        TextField("H2", text: $engineManager.h2)
          .textFieldStyle(.roundedBorder)

        Divider()

        HStack {
          Spacer()
          Button("Quit") {
            NSApplication.shared.terminate(nil)
          }
          .keyboardShortcut("q")
        }
      }
      .padding()
      .frame(width: 250)
      .onAppear {
        engineManager.start()
      }
    }
    .menuBarExtraStyle(.window)
  }
}
