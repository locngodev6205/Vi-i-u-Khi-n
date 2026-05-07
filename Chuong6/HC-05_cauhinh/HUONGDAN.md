# Huong dan dung 2 sketch de tranh ket AT mode

## 1) Day noi phan cung

- Pin 10 Arduino noi TX cua HC-05.
- Pin 11 Arduino noi RX cua HC-05 (nen qua chia ap cho RX 3.3V).
- Pin 9 Arduino noi KEY/EN cua HC-05.
- GND phai chung.

## 2) Co 2 file sketch va cach dung

- HC-05_cauhinh.ino: dung de cau hinh AT (KEY se duoc keo HIGH).
- HC-05_run.ino: dung de van hanh data mode (KEY duoc giu LOW, tranh vao AT).

## 3) Cau hinh module SLAVE

1. Nap HC-05_cauhinh.ino.
2. Dam bao KEY/EN HIGH truoc khi cap nguon module.
3. Mo Serial Monitor 9600 baud, line ending Newline hoac Both NL & CR.
4. Goi lenh A de lay dia chi.
5. Goi lenh S de auto cau hinh SLAVE.
6. Kiem tra lai:
	- AT+ROLE?
	- AT+PSWD?
	- AT+UART?
	- AT+ADDR?

Luu y: neu AT+CMODE=0 bi FAIL tren mot so firmware, SLAVE van co the hoat dong binh thuong.

## 4) Cau hinh module MASTER

1. Thao module SLAVE, gan module thu hai.
2. Van dung HC-05_cauhinh.ino va che do AT nhu tren.
3. Lay dia chi cua SLAVE theo dang BIND NAP,UAP,LAP.

Vi du neu ADDR cua SLAVE la 25:1D:E8:07:CE:A3 thi nhap:

M 7D6E,4D,4668C0 

4. Kiem tra lai:
	- AT+ROLE? phai la 1
	- AT+BIND? phai trung dia chi SLAVE
	- AT+PSWD? va AT+UART? phai khop voi SLAVE

## 5) Chay ket noi thuong (thoat AT mode)

1. Nap HC-05_run.ino cho Arduino dung de chay thuong.
2. Power off module.
3. Dam bao KEY/EN LOW (hoac thao day KEY).
4. Power on lai module.
5. LED se nhay nhanh (data mode, chua ket noi), sau do doi kieu nhay khi da ket noi.

## 6) Neu van bi ket AT mode

1. Tat nguon ca Arduino va HC-05.
2. Thao day KEY/EN khoi Arduino.
3. Thu keo KEY/EN xuong GND qua dien tro 10k.
4. Cap nguon lai va kiem tra LED.
5. Test nhanh:
	- Neu gui AT o 38400 ma tra OK on dinh -> van dang AT mode.
	- Neu khong tra OK -> da ve data mode.

## 7) Dong bo baud data mode

- Hien tai sketch cau hinh dat UART la 38400,0,0.
- Vi vay HC-05_run.ino cung dat HC05_DATA_BAUD = 38400.
- Neu muon data mode 9600:
  1. Doi DEFAULT_UART trong HC-05_cauhinh.ino thanh 9600,0,0 va cau hinh lai ca 2 module.
  2. Doi HC05_DATA_BAUD trong HC-05_run.ino thanh 9600.