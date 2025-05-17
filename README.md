change G++ path in Makefile

```shell
# build with cmake
C:\msys64\usr\bin\make.exe # change to your path

# run
.\build\run.exe frame1.jpg frame2.jpg

```



# GPT: Giải thích thuật toán [Lucas-Kanade](https://gist.github.com/TheVaffel/991ed8f43d8e526ea70935f05ebf1c04) 

## Mục đích

Code này thực hiện tính toán **optical flow** giữa hai khung hình ảnh (frame) bằng thuật toán Lucas-Kanade mở rộng sử dụng **kim tự tháp ảnh (image pyramid)**. Thuật toán giúp theo dõi sự dịch chuyển của điểm ảnh trong các khung hình liên tiếp.

---

## Tổng quan thuật toán

1. **Chuyển ảnh RGB sang ảnh xám (grayscale)**
   Ảnh màu RGB được chuyển sang ảnh xám để đơn giản hóa tính toán.

2. **Tạo kim tự tháp ảnh (Image pyramid)**
   Ảnh gốc được giảm kích thước theo cấp số nhân (mỗi mức giảm một nửa kích thước) với bộ lọc Gaussian để giảm nhiễu và tăng hiệu quả xử lý.

3. **Tính gradient ảnh (gradient computation)**
   Tính đạo hàm theo x và y tại mỗi pixel, dùng để xác định sự thay đổi cường độ ảnh trong vùng ảnh nhỏ.

4. **Tìm điểm đặc trưng (strong feature)**
   Tìm điểm có độ biến thiên (gradient) lớn nhất quanh tâm ảnh để theo dõi.

5. **Thuật toán Lucas-Kanade tại từng mức kim tự tháp**
   Tại mỗi mức của kim tự tháp, sử dụng cửa sổ 5x5 quanh điểm quan tâm để tính toán dịch chuyển bằng cách giải hệ phương trình tuyến tính. Thuật toán thực hiện lặp lại nhiều lần để tìm dịch chuyển hội tụ.

6. **Kết hợp các mức kim tự tháp**
   Bắt đầu từ mức thấp nhất (ảnh nhỏ nhất), tính luồng quang học rồi nhân lên để cập nhật điểm dịch chuyển ở mức lớn hơn (ảnh lớn hơn), tăng độ chính xác dịch chuyển tại ảnh gốc.

---

## Cấu trúc chính của chương trình

* **rgb\_to\_grayscale**: chuyển ảnh RGB sang grayscale.
* **build\_image\_pyramid**: tạo kim tự tháp ảnh với bộ lọc Gaussian.
* **compute\_gradient**: tính gradient ảnh tại mỗi mức kim tự tháp.
* **find\_strong\_feature**: tìm điểm có gradient mạnh nhất quanh tâm ảnh.
* **lucas\_kanade\_at\_level**: thực hiện thuật toán Lucas-Kanade trong cửa sổ 5x5 tại một mức kim tự tháp, lặp 5 lần để hội tụ dịch chuyển.
* **lucas\_kanade\_pyramid**: tính toán dịch chuyển điểm từ mức thấp nhất tới mức cao nhất (ảnh gốc) theo kim tự tháp.

---

## Ý tưởng chính của thuật toán Lucas-Kanade

* **Giả thiết**: điểm ảnh trong vùng nhỏ có cùng dịch chuyển (u,v).
* Tính đạo hàm cường độ ảnh theo x, y và sự thay đổi theo thời gian.
* Giải hệ phương trình để tìm vector dịch chuyển (u,v) sao cho giảm sai số sai lệch cường độ ảnh giữa hai khung.
* Dùng lặp để tìm nghiệm tốt hơn.
* Thuật toán kim tự tháp giúp theo dõi dịch chuyển lớn bằng cách bắt đầu từ ảnh nhỏ hơn, rồi dần đến ảnh lớn hơn.

---

## Các thông số quan trọng

* `WIDTH`, `HEIGHT`: kích thước ảnh (320x180).
* `PYR_LEVELS`: số mức kim tự tháp (3).
* `WINDOW_SIZE`: kích thước cửa sổ tính gradient (5x5).
* `NUM_ITER`: số lần lặp hội tụ tại mỗi mức (5).

---

## Lưu ý

* Ảnh đầu vào phải đúng kích thước và định dạng RGB 3 kênh.
* Bộ nhớ cấp phát cho kim tự tháp và gradient cần được giải phóng sau khi sử dụng (code gốc chưa có phần giải phóng bộ nhớ).
* Thuật toán Lucas-Kanade không hiệu quả với vùng ảnh có ít texture hoặc gradient yếu.



