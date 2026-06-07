# Hệ thống phân loại màu sắc bằng Edge AI

Dự án ứng dụng Machine Learning (Decision Tree) để phân loại 8 trạng thái màu sắc trên băng chuyền, sử dụng cảm biến quang học TCS34725 và vi điều khiển Arduino Uno.
Sinh viên thực hiện: Lê Văn Duẩn - HUMG

##  Cấu trúc kho lưu trữ
* `Color_dataset_8mau.csv`: Tập dữ liệu quang phổ thô được thu thập thực tế và nội suy chống nhiễu sáng.
* `Huanluyenmohinhpart3.py`: Mã nguồn Python trích xuất đặc trưng và huấn luyện mô hình.
* `ColorModel_8mau.h`: Mô hình AI đã được dịch sang C++ tĩnh để nạp thẳng vào Arduino.
* `ColorModel_8mau.pkl`: File mô hình lưu trữ cục bộ.
* `Part3_Arduino_AI_Vision.ino`: Mã nguồn thực thi logic phân loại và lọc nhiễu trên mạch vật lý.

##  Phần cứng sử dụng
* Khối xử lý trung tâm: Arduino Uno R3
* Cảm biến thị giác: TCS34725 (Giao tiếp I2C)
* Hiển thị: Màn hình LCD Keypad Shield
