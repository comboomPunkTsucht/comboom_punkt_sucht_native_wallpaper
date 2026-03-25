import Cocoa
import MetalKit
import SwiftUI

class WallpaperWindow: NSWindow {

    // Wir merken uns den Monitor, zu dem dieses Fenster gehört
  private var assignedScreen: NSScreen

  init(screen: NSScreen) {
    self.assignedScreen = screen
    let screenRect = screen.frame

    super.init(contentRect: screenRect, styleMask: .borderless, backing: .buffered, defer: false)

    let desktopLevel = Int(CGWindowLevelForKey(.desktopIconWindow)) - 1
    self.level = NSWindow.Level(desktopLevel)
    self.collectionBehavior = [.canJoinAllSpaces, .stationary, .ignoresCycle]
    self.ignoresMouseEvents = true


    let metalView = MetalWallpaperView(frame: screenRect)

    if let engineShared = EngineManager.shared {
      if let eginePtr = engineShared.engine {
        self.backgroundColor = NSColor(
          red: CGFloat(eginePtr.pointee.background_color.r)/255.0,
          green: CGFloat(eginePtr.pointee.background_color.g)/255.0,
          blue: CGFloat(eginePtr.pointee.background_color.b)/255.0,
          alpha: CGFloat(eginePtr.pointee.background_color.a)/255.0
        )
      }
      let overlayView = NSHostingView(rootView: TextOverlayView(engineManager: engineShared))
      overlayView.frame = screenRect
      overlayView.wantsLayer = true
      overlayView.layer?.backgroundColor = NSColor.clear.cgColor

      metalView.addSubview(overlayView)
    }

    self.contentView = metalView
    self.setFrame(screenRect, display: true)

      // NEU: Höre live auf Auflösungsänderungen (z.B. Skalierung in den Mac-Einstellungen)
    NotificationCenter.default.addObserver(self, selector: #selector(updateResolution), name: NSApplication.didChangeScreenParametersNotification, object: nil)
  }

    // NEU: Diese Funktion feuert automatisch, sobald sich die Auflösung ändert
  @objc func updateResolution() {
      // Die brandneuen Koordinaten des Bildschirms auslesen
    let newFrame = self.assignedScreen.frame

      // Fenster und alle Views (Metal + SwiftUI) sofort in die neue Form pressen!
    self.setFrame(newFrame, display: true)
    self.contentView?.frame = newFrame
  }
}
