//
//  MetalWallpaperView.swift
//  comboom.sucht Live Wallpaper
//
//  Created by mcpeaps_HD on 25/03/2026.
//

import MetalKit

class MetalWallpaperView: MTKView {

    // Unser Metal-Renderer
  var renderer: Renderer?

  init(frame: CGRect) {
      // GPU (Device) finden
    guard let device = MTLCreateSystemDefaultDevice() else {
      fatalError("Metal wird auf diesem Mac nicht unterstützt!")
    }

    super.init(frame: frame, device: device)

      // MTKView konfigurieren
    if let engineShared = EngineManager.shared {
      if let eginePtr = engineShared.engine {
        self.clearColor = MTLClearColor(
          red: Double(eginePtr.pointee.background_color.r)/255.0,
          green: Double(eginePtr.pointee.background_color.g)/255.0,
          blue: Double(eginePtr.pointee.background_color.b)/255.0,
          alpha: Double(eginePtr.pointee.background_color.a)/255.0
        )
      }
    }
    self.framebufferOnly = true
    self.colorPixelFormat = .bgra8Unorm

      // Da unser CADisplayLink den Takt vorgibt, deaktiveren wir das automatische Update
    self.isPaused = true
    self.enableSetNeedsDisplay = false

      // Renderer initialisieren und als Delegate setzen
    renderer = Renderer(device: device, metalKitView: self)
    self.delegate = renderer

    // Wichtig: drawableSize explizit setzen
    self.drawableSize = self.bounds.size
  }

  required init(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }

  override var frame: CGRect {
    didSet {
      // When frame changes, update drawable size too
      self.drawableSize = self.bounds.size
    }
  }

  override var bounds: CGRect {
    didSet {
      // When bounds change, update drawable size too
      self.drawableSize = self.bounds.size
    }
  }
}
