//
//  Renderer.swift
//  comboom.sucht Live Wallpaper
//
//  Created by mcpeaps_HD on 25/03/2026.
//

import MetalKit

class Renderer: NSObject, MTKViewDelegate {

  private let device: MTLDevice
  private let commandQueue: MTLCommandQueue
  private let pipelineState: MTLRenderPipelineState

  init(device: MTLDevice, metalKitView: MTKView) {
    self.device = device
    self.commandQueue = device.makeCommandQueue()!

      // 1. Shader laden
    let library = device.makeDefaultLibrary()!
    let vertexFunction = library.makeFunction(name: "vertex_main")
    let fragmentFunction = library.makeFunction(name: "fragment_main")

      // 2. Render Pipeline Pipeline Konfigurieren
    let pipelineDescriptor = MTLRenderPipelineDescriptor()
    pipelineDescriptor.label = "Particles Pipeline"
    pipelineDescriptor.vertexFunction = vertexFunction
    pipelineDescriptor.fragmentFunction = fragmentFunction
    pipelineDescriptor.colorAttachments[0].pixelFormat = metalKitView.colorPixelFormat

      // Wichtig: Alpha-Blending aktivieren, damit transparente Partikel korrekt übereinander liegen
    let attachment = pipelineDescriptor.colorAttachments[0]
    attachment?.isBlendingEnabled = true
    attachment?.rgbBlendOperation = .add
    attachment?.alphaBlendOperation = .add
    attachment?.sourceRGBBlendFactor = .sourceAlpha
    attachment?.sourceAlphaBlendFactor = .sourceAlpha
    attachment?.destinationRGBBlendFactor = .oneMinusSourceAlpha
    attachment?.destinationAlphaBlendFactor = .oneMinusSourceAlpha

    pipelineState = try! device.makeRenderPipelineState(descriptor: pipelineDescriptor)

    super.init()
  }

    // --- MTKViewDelegate Methoden ---

  func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
      // Hier könntest du auf Größenänderungen reagieren
  }

  func draw(in view: MTKView) {
      // Diese Methode wird vom DisplayLink getriggert
    guard let engineManager = EngineManager.shared,
          let enginePtr = engineManager.engine,
          let drawable = view.currentDrawable,
          let renderPassDescriptor = view.currentRenderPassDescriptor else {
      return
    }

      // 3. Partikel-Daten von der C-Engine holen
    var particleCount: UInt32 = 0
    guard let particlesC = cbps_engine_get_particles(enginePtr, &particleCount) else {
      return
    }

      // 4. Metal Buffer erstellen (und Partikel-Daten von C in den Buffer kopieren)
      // Dieser Schritt ist performant, da der Buffer direkt auf dem CPU/GPU shared Memory liegt.
    let bufferSize = MemoryLayout<CBPSParticle>.stride * Int(particleCount)
    guard let particleBuffer = device.makeBuffer(
      bytes: particlesC,
      length: bufferSize,
      options: .storageModeShared
    ) else { return }

      // 5. Bildschirmgröße an Shader übergeben
    var screenSize = simd_float2(
      Float(enginePtr.pointee.screen_width),
      Float(enginePtr.pointee.screen_height)
    )

      // 6. GPU-Befehle schreiben
    let commandBuffer = commandQueue.makeCommandBuffer()!
    let renderEncoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor)!

    //renderEncoder.setLabel("Particle Render Encoder")
    renderEncoder.setRenderPipelineState(pipelineState)

      // Partikel-Daten an Shader übergeben (Buffer 0)
    renderEncoder.setVertexBuffer(particleBuffer, offset: 0, index: 0)
      // Bildschirmgröße an Shader übergeben (Buffer 1)
    renderEncoder.setVertexBytes(&screenSize, length: MemoryLayout<simd_float2>.size, index: 1)

      // 7. DAS ZEICHNEN BEFEHLEN: Zeichne 'particleCount' Punkte
    renderEncoder.drawPrimitives(type: .point, vertexStart: 0, vertexCount: Int(particleCount))

    renderEncoder.endEncoding()

      // 8. Auf dem Bildschirm anzeigen
    commandBuffer.present(drawable)
    commandBuffer.commit()
  }
}
