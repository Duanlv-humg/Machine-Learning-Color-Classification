#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <LiquidCrystal.h>

// Nhúng "bộ não" AI đã train từ Python
// (Lưu ý: Tên file vẫn là 7mau do code Python cũ sinh ra, nhưng bên trong đã chứa 8 class)
#include "ColorModel_7mau.h"

// 1. Khởi tạo màn hình LCD Keypad Shield (Các chân mặc định)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// 2. Khởi tạo cảm biến TCS34725
// Tích hợp thời gian lấy mẫu 50ms và độ nhạy Gain 4X (Phù hợp băng chuyền)
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// 3. Khởi tạo Model Cây quyết định
Eloquent::ML::Port::DecisionTree clf;

// 4. Định nghĩa các trạng thái nút bấm trên Shield
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

bool isScanning = false;

// Hàm đọc tín hiệu nút bấm từ chân Analog A0
int read_LCD_buttons() {
    int adc_key_in = analogRead(A0);
    if (adc_key_in > 1000) return btnNONE;
    if (adc_key_in < 50)   return btnRIGHT;
    if (adc_key_in < 250)  return btnUP;
    if (adc_key_in < 450)  return btnDOWN;
    if (adc_key_in < 650)  return btnLEFT;
    if (adc_key_in < 850)  return btnSELECT;
    return btnNONE;
}

void setup() {
    Serial.begin(9600);
    
    // Setup màn hình
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("AI Color Vision ");
    lcd.setCursor(0, 1);
    lcd.print("Init TCS34725...");
    delay(1000);

    // Kiểm tra kết nối I2C với cảm biến
    if (tcs.begin()) {
        lcd.setCursor(0, 1);
        lcd.print("Sensor OK!      ");
        delay(1000);
    } else {
        lcd.setCursor(0, 1);
        lcd.print("Sensor Error!   ");
        while (1); // Treo máy nếu cắm sai dây I2C
    }

    lcd.clear();
    lcd.print("Press LEFT Start");
}

void loop() {
    int lcd_key = read_LCD_buttons();

    // Bấm nút LEFT để bắt đầu quét
    if (lcd_key == btnLEFT) {
        isScanning = true;
        lcd.clear();
        lcd.print("Scanning...");
        delay(300);
    }
    // Bấm nút RIGHT để dừng hệ thống
    else if (lcd_key == btnRIGHT) {
        isScanning = false;
        lcd.clear();
        lcd.print("Stopped.        ");
        delay(500);
        lcd.clear();
        lcd.print("Press LEFT Start");
    }

    // Nếu hệ thống đang trong trạng thái quét
    if (isScanning) {
        float r, g, b;
        
        // Thư viện tự động đọc và chuẩn hóa dữ liệu về thang 0-255
        tcs.getRGB(&r, &g, &b); 

        // TRÍCH XUẤT ĐẶC TRƯNG TOÁN HỌC (Feature Engineering)
        float sum_color = r + g + b + 0.000001; // Cộng thêm số cực nhỏ để chống lỗi chia cho 0
        float r_ratio = r / sum_color;
        float g_ratio = g / sum_color;
        float b_ratio = b / sum_color;
        float brightness = sum_color / (255.0 * 3.0); 

        // Đưa vào mảng dữ liệu để nhét vào não bộ AI
        float features[4] = {r_ratio, g_ratio, b_ratio, brightness};

// 1. AI suy luận ra số thứ tự (0 đến 7)
        int classIdx = clf.predict(features); 
        
        // 2. Từ điển 8 nhãn mới (ĐÃ SẮP XẾP LẠI CHUẨN A-Z THEO PYTHON)
        String classNames[] = {"Background", "Blue", "Green", "Pink", "Purple", "Red", "White", "Yellow"};
        
        // Lấy tên màu ban đầu do AI đoán
        String colorName = classNames[classIdx]; 

        // ========================================================
        // HÀNG RÀO BẢO VỆ: BỘ LỌC CHỐNG "MÀU LẠ" (UNKNOWN)
        // ========================================================
        if (colorName == "Red" && (r < g || r < b)) {
            colorName = "Unknown";
        }
        else if (colorName == "Blue" && (b < r || b < g)) {
            colorName = "Unknown";
        }
        // Thêm bộ lọc cho Green: Nếu đoán là Xanh lá mà kênh G lại lùn hơn R hoặc B thì là sai
        else if (colorName == "Green" && (g < r || g < b)) {
            colorName = "Unknown";
        }
        else if (colorName == "Yellow" && (r < 100 || g < 100)) {
            colorName = "Unknown";
        }
        else if (colorName == "Pink" && (r < b)) {
            colorName = "Unknown";
        }
        
        // Lọc nhiễu tối: Vì bạn không còn học màu Đen nữa, nên mọi thứ quá tối
        // đập vào cảm biến đều sẽ tự động bị gán thành Unknown
        if (r < 15 && g < 15 && b < 15) {
            colorName = "Unknown";
        }
        // ========================================================

        // In dữ liệu ra Serial Monitor để kiểm tra
        Serial.print("R: "); Serial.print((int)r);
        Serial.print("\tG: "); Serial.print((int)g);
        Serial.print("\tB: "); Serial.print((int)b);
        Serial.print("\t=> Predict: "); Serial.println(colorName);

        // Hiển thị trực tiếp lên LCD
        lcd.setCursor(0, 0);
        lcd.print("AI: ");
        lcd.print(colorName);
        lcd.print("        "); // Xóa các khoảng trắng thừa của từ trước đó

        lcd.setCursor(0, 1);
        lcd.print("R:"); lcd.print((int)r);
        lcd.print(" G:"); lcd.print((int)g);
        lcd.print(" B:"); lcd.print((int)b);
        lcd.print("   ");

        delay(400); // Chu kỳ quét màu (400ms/lần)
    }
}