# [Migration Plan] bamboo-core (Go) -> lotus-core (C++)

## Primary Goal: Simplicity First

The objective is to achieve a functional C++ version as quickly as possible. Optimization, advanced memory safety, and performance tuning will be addressed in subsequent iterations.

## Technical Strategy (MVP)

- **C++ Version:** C++17 or C++20 (Standard STL).
- **Strings:** Use `std::string` or `std::wstring` for simplicity initially.
- **Memory:** Basic RAII. Focus on functional correctness over strict smart pointer usage if it slows down porting.
- **Errors:** Simple return values or basic exceptions.
- **Build:** Minimal CMakeLists.txt.

## Task Sequence (Bottom-Up Approach)

Thứ tự được thiết kế dựa trên cây phụ thuộc (dependency tree) để đảm bảo khi port một module, các thành phần nó cần đã sẵn sàng:

### 1. Project Setup

- Khởi tạo `CMakeLists.txt` và cấu trúc thư mục.

### 2. Foundational Data (Leaf Modules)

- **Charset:** Port `charset_def.go` (Hằng số ký tự Unicode). Đây là module cơ bản nhất, không phụ thuộc vào ai.
- **Definitions:** Port `input_method_def.go` (Các struct `Rule`, `InputMethod`).

### 3. Logic Support (Pure Utilities)

- **Utils:** Port `utils.go` (Kiểm tra nguyên âm, phụ âm, tìm chỉ mục).
- **Flattener:** Port `flattener.go` (Logic biến đổi từ danh sách `Transformation` thành chuỗi ký tự).

### 4. Logic Complex (Internal Logic)

- **Rules Parser:** Port `rules_parser.go` (Đọc cấu hình bộ gõ).
- **Engine Utils:** Port `bamboo_utils.go` (Logic xử lý tổ hợp ký tự, tìm mục tiêu bỏ dấu).

### 5. Core Orchestration

- **Engine:** Port `bamboo.go` (Class `LotusEngine` điều phối toàn bộ quá trình).

---

## Minimum Verification

- **Functional Check:** Does the C++ output match the Go output for 10 common Vietnamese test words (e.g., "tiếng", "việt")?
- **Unit Tests:** Mirror only the most critical tests from the Go suite.
