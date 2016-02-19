import Foundation
import ScreenSaver
import Cocoa

class NixieTubeClock: ScreenSaverView {
    
    var nixieTubeNumbers: NSImage?
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
        self.animationTimeInterval = 0.008
        if let path = NSBundle(forClass: self.dynamicType).pathForResource("nixie_tube_numbers", ofType: "png") {
            nixieTubeNumbers = NSImage(contentsOfFile: path)
        }
    }
    
    override init?(frame: NSRect, isPreview: Bool) {
        super.init(frame: frame, isPreview: isPreview)
        self.animationTimeInterval = 0.008
        if let path = NSBundle(forClass: self.dynamicType).pathForResource("nixie_tube_numbers", ofType: "png") {
            nixieTubeNumbers = NSImage(contentsOfFile: path)
        }
    }
    
    override func startAnimation() {
        super.startAnimation()
    }
    
    override func stopAnimation() {
        super.stopAnimation()
    }
    
    override func drawRect(rect: NSRect) {
        super.drawRect(rect)
        NSColor.blackColor().set()
        NSBezierPath(rect: rect).fill()
    }
    
    override func animateOneFrame() {
        if let img: NSImage = nixieTubeNumbers {
            let rate: CGFloat = frame.width / 1440.0
            let width: CGFloat = img.size.width / 10 * rate
            let height: CGFloat = img.size.height * rate
            var rect: CGRect = NSMakeRect(0, frame.height / 2, width, height)
            var fromRect: CGRect = NSMakeRect(0, 0, img.size.width / 10, img.size.height)
            let fmt: NSDateFormatter = NSDateFormatter()
            fmt.dateFormat = "yyyyMMddHHmmssSSS"
            let str: String = fmt.stringFromDate(NSDate())
            for i in 0...7 {
                rect.origin.x = (CGFloat(i) * width) + (frame.width / 2) - (width * 4)
                let number = Int((str as NSString).substringWithRange(NSRange(location: i, length: 1)))
                fromRect.origin.x = (img.size.width / 10) * CGFloat(number!)
                img.drawInRect(rect, fromRect: fromRect, operation: .CompositeCopy, fraction: 1.0)
            }
            rect.origin.y -= height
            for i in 8...16 {
                rect.origin.x = (CGFloat(i - 8) * width) + (frame.width / 2) - (width * 4.5)
                let number = Int((str as NSString).substringWithRange(NSRange(location: i, length: 1)))
                fromRect.origin.x = (img.size.width / 10) * CGFloat(number!)
                img.drawInRect(rect, fromRect: fromRect, operation: .CompositeCopy, fraction: 1.0)
            }
        }
    }
    
    override func hasConfigureSheet() -> Bool {
        return false
    }
    
    override func configureSheet() -> NSWindow? {
        return nil
    }
}