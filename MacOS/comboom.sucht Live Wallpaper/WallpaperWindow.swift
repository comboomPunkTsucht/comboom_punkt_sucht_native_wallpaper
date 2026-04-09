import Cocoa
import MetalKit
import SwiftUI

class WallpaperWindow: NSWindow {

    private var assignedScreen: NSScreen
    private weak var metalView: MetalWallpaperView?
    private weak var engineManager: EngineManager?
    private weak var overlayHostingView: NSHostingView<TextOverlayView>?

  init(screen: NSScreen) {
    self.assignedScreen = screen
    let screenRect = screen.frame

    super.init(contentRect: screenRect, styleMask: .borderless, backing: .buffered, defer: false)

    let desktopLevel = Int(CGWindowLevelForKey(.desktopIconWindow)) - 1
    self.level = NSWindow.Level(desktopLevel)
    self.collectionBehavior = [.canJoinAllSpaces, .stationary, .ignoresCycle]
    self.ignoresMouseEvents = true

    let metalView = MetalWallpaperView(frame: screenRect)
    self.metalView = metalView

    if let engineShared = EngineManager.shared {
      self.engineManager = engineShared

      if let eginePtr = engineShared.engine {
        self.backgroundColor = NSColor(
          red: CGFloat(eginePtr.pointee.background_color.r)/255.0,
          green: CGFloat(eginePtr.pointee.background_color.g)/255.0,
          blue: CGFloat(eginePtr.pointee.background_color.b)/255.0,
          alpha: CGFloat(eginePtr.pointee.background_color.a)/255.0
        )
      }

      addOverlay(to: metalView, engineManager: engineShared, frame: screenRect)
    }

    self.contentView = metalView
    self.setFrame(screenRect, display: true)

    NotificationCenter.default.addObserver(self, selector: #selector(updateResolution), name: NSApplication.didChangeScreenParametersNotification, object: nil)
  }

  private func addOverlay(to metalView: MetalWallpaperView, engineManager: EngineManager, frame: CGRect) {
    let screenOverlay = TextOverlayView(engineManager: engineManager)
    let overlayView = NSHostingView(rootView: screenOverlay)

    self.overlayHostingView = overlayView

    overlayView.wantsLayer = true
    overlayView.layer?.backgroundColor = NSColor.clear.cgColor

    // Use frame-based layout, not autolayout
    overlayView.frame = frame
    overlayView.autoresizingMask = [.width, .height]

    metalView.addSubview(overlayView)
  }

  @objc func updateResolution() {
    let newFrame = self.assignedScreen.frame

    // Resize the window
    self.setFrame(newFrame, display: true)

    // IMPORTANT: Explicitly resize contentView (metalView) to fill the window
    // The window's contentView doesn't automatically resize
    if let contentView = self.contentView {
      contentView.frame = CGRect(x: 0, y: 0, width: newFrame.width, height: newFrame.height)
    }

    // Update overlay to match contentView bounds
    if let overlayView = overlayHostingView, let contentView = self.contentView {
      overlayView.frame = CGRect(x: 0, y: 0, width: contentView.frame.width, height: contentView.frame.height)
      overlayView.needsDisplay = true
    }
  }

  deinit {
    NotificationCenter.default.removeObserver(self)
  }
}


