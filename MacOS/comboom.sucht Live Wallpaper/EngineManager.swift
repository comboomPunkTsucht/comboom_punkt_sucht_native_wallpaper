import Cocoa
import QuartzCore
import SwiftUI
import MetalKit // <-- Metal hinzufügen
import Combine

class EngineManager: ObservableObject {

    // Singleton, damit der Renderer auf die Engine zugreifen kann
  static var shared: EngineManager?

  var engine: UnsafeMutablePointer<CBPSWallpaperEngine>?
  private var displayLink: CADisplayLink?
  private var lastTime: CFTimeInterval = 0

    // NEU: Arrays, um mehrere Monitore zu verwalten!
  private var wallpaperWindows: [WallpaperWindow] = []
  private var mtkViews: [MTKView] = []

    // @Published sorgt dafür, dass die SwiftUI-Menüleiste sofort reagiert
  @Published var h1: String {
    didSet {
      UserDefaults.standard.set(h1, forKey: "saved_h1")
      // Hier ggf. deine C-Engine updaten lassen
    }
  }

  @Published var h2: String {
    didSet {
      UserDefaults.standard.set(h2, forKey: "saved_h2")
      // Hier ggf. deine C-Engine updaten lassen
    }
  }
  private var sceen_width: UInt32 = 7690
  private var sceen_height: UInt32 = 4320

  init() {
      // Wir verzögern den Start um den Bruchteil einer Sekunde,
      // damit macOS Zeit hat, die Bildschirme (NSScreen.main) richtig zu laden
    self.h1 = UserDefaults.standard.string(forKey: "saved_h1") ?? "mcpeaps_HD"
    self.h2 = UserDefaults.standard.string(forKey: "saved_h2") ?? "comboom.sucht"
    DispatchQueue.main.async {
      self.start()
    }
  }

  func start() {
    Self.shared = self // Singleton setzen
    var randomSeed: UInt32 = UInt32(
      truncatingIfNeeded: UInt64(Date().timeIntervalSince1970 * 1000) % UInt64(UInt32.max))  // Zufälliger Seed für die C-Engine
    cbps_engine_set_seed(randomSeed) // Zufälligen Seed an die C-Engine übergeben

      // C-Engine initialisieren
    engine = cbps_engine_create(
      self.sceen_width, self.sceen_height, 400,
      "", ""
    )

      // NEU: Für JEDEN angeschlossenen Monitor ein eigenes Fenster bauen!
    for screen in NSScreen.screens {
      let window = WallpaperWindow(screen: screen)
      window.makeKeyAndOrderFront(nil)
      self.wallpaperWindows.append(window) // In der Liste speichern

      if let view = window.contentView as? MetalWallpaperView {
        self.mtkViews.append(view) // Metal-View in der Liste speichern
      }
    }

    lastTime = CACurrentMediaTime()

      // macOS 15 Weg: DisplayLink über den Hauptbildschirm erstellen
      // Den Taktgeber (DisplayLink) an den Hauptmonitor hängen
    if let screen = NSScreen.screens.first {
      displayLink = screen.displayLink(target: self, selector: #selector(updateLoop(link:)))
      displayLink?.add(to: .main, forMode: .common)
    }
  }

  @objc func updateLoop(link: CADisplayLink) {
    let currentTime = link.timestamp
    let deltaTime = Float(currentTime - lastTime)
    var mouseX: Int32 = 0
    var mouseY: Int32 = 0
    self.lastTime = currentTime

    if let enginePtr = engine {

      let mouseLoc = NSEvent.mouseLocation

      //print("Maus Position: X: \(mouseLoc.x), Y: \(mouseLoc.y)")

      mouseX = Int32(mouseLoc.x)

        // 1. FIX: Nutze .screens.first! .main wird im Hintergrund zu nil!
      if let screen = NSScreen.screens.first {
        mouseY = Int32(screen.frame.height - mouseLoc.y)
        self.sceen_width = UInt32(screen.frame.width)
        self.sceen_height = UInt32(screen.frame.height)
      }

        // 2. FIX: KEIN DispatchQueue.main.async hier!
      self.h1.withCString { h1CStr in
        self.h2.withCString { h2CStr in
          cbps_engine_update(
            enginePtr,
            deltaTime,
            h1CStr,
            h2CStr,
            self.sceen_width, self.sceen_height,
            mouseX, mouseY
          )
        }
      }

        // Jetzt zeichnet Metal garantiert die frischen Daten!
      for view in self.mtkViews {
        view.draw()
      }
    }
  }

  func stop() {
    displayLink?.invalidate()
    displayLink = nil

    if let enginePtr = engine {
      cbps_engine_destroy(enginePtr)
      engine = nil
    }
  }
}
