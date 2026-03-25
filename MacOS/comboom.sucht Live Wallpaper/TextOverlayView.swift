import SwiftUI

struct TextOverlayView: View {
    // Beobachtet unseren EngineManager live
    @ObservedObject var engineManager: EngineManager

    // Safely computed colors from the C++ engine; provide sensible defaults if engine is unavailable
    private var backgroundColor: Color {
        guard let eginePtr = engineManager.engine else { return Color.black }
        return Color(
          red: Double(eginePtr.pointee.background_color.r) / 255.0,
          green: Double(eginePtr.pointee.background_color.g) / 255.0,
          blue: Double(eginePtr.pointee.background_color.b) / 255.0,
          opacity: Double(eginePtr.pointee.background_color.a) / 255.0
        )
    }

    private var foregroundColor: Color {
        guard let eginePtr = engineManager.engine else { return Color.white }
        return Color(
          red: Double(eginePtr.pointee.foreground_color.r) / 255.0,
          green: Double(eginePtr.pointee.foreground_color.g) / 255.0,
          blue: Double(eginePtr.pointee.foreground_color.b) / 255.0,
          opacity: Double(eginePtr.pointee.foreground_color.a) / 255.0
        )
    }

    var body: some View {
        GeometryReader { geo in
            let width = geo.size.width
            let height = geo.size.height

            // Exakt deine C++ Mathe für die Größen
            let titleFontSize = max(24, min(160, width / 10))
            let subFontSize = max(12, min(48, width / 30))
            let lineWidth = width * 0.85

            ZStack {

                // --- 1. Der zentrierte Text und die Linien ---
                VStack(spacing: 24) {

                    // Obere Gradient-Linie
                    gradientLine(width: lineWidth)

                    // H1 (Titel)
                    Text(engineManager.h1)
                        .font(.custom("CaskaydiaCoveNFP-Regular", size: titleFontSize))
                        .foregroundStyle(foregroundColor)

                    // Untere Gradient-Linie
                    gradientLine(width: lineWidth)

                    // H2 (Untertitel oder cbps Logo)
                    if engineManager.h2.lowercased() == "comboom.sucht" {
                        Image("cbps_logo")
                            .resizable()
                            .scaledToFit()
                            .frame(height: max(48, min(192, height * 0.12)))
                            .colorMultiply(foregroundColor)
                    } else {
                        Text(engineManager.h2)
                            .font(.custom("CaskaydiaCoveNFP-Regular", size: subFontSize))
                            .foregroundStyle(foregroundColor)
                    }

                }
                .position(x: width / 2, y: height / 2 - 20)

                // --- 2. Das kleine Logo unten rechts ---
                bottomRightLogo(width: width, height: height)
            }
        }
    }

    // Hilfsfunktion für den Verlauf: foregroundColor in der Mitte, mit Background an den Rändern
    @ViewBuilder
    func gradientLine(width: CGFloat) -> some View {
        Rectangle()
            .fill(
                LinearGradient(
                    gradient: Gradient(colors: [backgroundColor, foregroundColor, backgroundColor]),
                    startPoint: .leading,
                    endPoint: .trailing
                )
            )
            .frame(width: width, height: 3)
    }

    // Hilfsfunktion für das Eck-Logo
    @ViewBuilder
    func bottomRightLogo(width: CGFloat, height: CGFloat) -> some View {
        let h1Lower = engineManager.h1.lowercased()
        let targetH = max(24, min(128, height * 0.06))

        VStack {
            Spacer()
            HStack {
                Spacer()

                if h1Lower == "mcpeaps_hd" {
                    Image("mahd_logo").resizable().scaledToFit().frame(height: targetH)
                } else if h1Lower == "blackdragon" {
                    Image("bd_logo").resizable().scaledToFit().frame(height: targetH)
                } else if h1Lower == "knuddelzwerck" {
                    Image("knuddelzwerck_logo").resizable().scaledToFit().frame(height: targetH)
                } else if h1Lower == "fabelke" {
                    Image("fabelke_logo").resizable().scaledToFit().frame(height: targetH)
                }
            }
            .padding(16)
        }
    }
}
