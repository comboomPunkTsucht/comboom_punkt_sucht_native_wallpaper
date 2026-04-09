import SwiftUI

// Screen-specific overlay that uses per-monitor dimensions
struct TextOverlayView: View {
    @ObservedObject var engineManager: EngineManager
    @State private var refreshId: UUID = UUID()

    var body: some View {
        GeometryReader { geometry in
            let screenWidth = UInt32(geometry.size.width)
            let screenHeight = UInt32(geometry.size.height)

            let titleFontSize = CGFloat(max(24, min(160, screenWidth / 10)))
            let subFontSize = CGFloat(max(12, min(48, screenWidth / 30)))
            let lineWidth = Double(screenWidth) * 0.85

            ZStack {
                VStack(spacing: 24) {
                    gradientLine(width: lineWidth)

                    Text(engineManager.h1)
                        .font(.custom("CaskaydiaCoveNFP-Regular", size: titleFontSize))
                        .foregroundStyle(foregroundColor)

                    gradientLine(width: lineWidth)

                    if engineManager.h2.lowercased() == "comboom.sucht".lowercased() {
                        Image("cbps_logo")
                            .resizable()
                            .scaledToFit()
                            .frame(height: max(48, min(192, Double(screenHeight) * 0.12)))
                            .colorMultiply(foregroundColor)
                    } else {
                        Text(engineManager.h2)
                            .font(.custom("CaskaydiaCoveNFP-Regular", size: subFontSize))
                            .foregroundStyle(foregroundColor)
                    }
                }
                .position(x: geometry.size.width / 2, y: geometry.size.height / 2)

                bottomRightLogo(
                    width: geometry.size.width,
                    height: geometry.size.height
                )
            }
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            .id(refreshId)  // Force redraw when refreshId changes
        }
    }

    func triggerRefresh() {
        refreshId = UUID()
    }

    private var backgroundColor: Color {
        guard let enginePtr = engineManager.engine else { return Color.black }
        return Color(
          red: Double(enginePtr.pointee.background_color.r) / 255.0,
          green: Double(enginePtr.pointee.background_color.g) / 255.0,
          blue: Double(enginePtr.pointee.background_color.b) / 255.0,
          opacity: Double(enginePtr.pointee.background_color.a) / 255.0
        )
    }

    private var foregroundColor: Color {
        guard let enginePtr = engineManager.engine else { return Color.white }
        return Color(
          red: Double(enginePtr.pointee.foreground_color.r) / 255.0,
          green: Double(enginePtr.pointee.foreground_color.g) / 255.0,
          blue: Double(enginePtr.pointee.foreground_color.b) / 255.0,
          opacity: Double(enginePtr.pointee.foreground_color.a) / 255.0
        )
    }

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

    @ViewBuilder
    func bottomRightLogo(width: CGFloat, height: CGFloat) -> some View {
        let h1Lower = engineManager.h1.lowercased()
        let targetH = max(24, min(128, height * 0.06))

        VStack {
            Spacer()
            HStack {
                Spacer()

                if h1Lower == "mcpeaps_hd".lowercased() {
                    Image("mahd_logo").resizable().scaledToFit().frame(height: targetH)
                } else if h1Lower == "blackdragon".lowercased() {
                    Image("bd_logo").resizable().scaledToFit().frame(height: targetH)
                } else if h1Lower == "knuddelzwerck".lowercased() {
                    Image("knuddelzwerck_logo").resizable().scaledToFit().frame(height: targetH)
                } else if h1Lower == "fabelke".lowercased() {
                    Image("fabelke_logo").resizable().scaledToFit().frame(height: targetH)
                }
            }
            .padding(16)
        }
    }
}
