# CAN Bus Simulator Speedometer Tester

Project Arduino untuk mensimulasikan sinyal kecepatan kendaraan pada bus CAN dan sekaligus menjadi tester yang bisa menerima frame CAN dari perangkat lain.

Fitur utama:
- Mengirim frame CAN dengan nilai kecepatan kendaraan (0–250 km/h)
- Menangkap dan menampilkan frame CAN yang masuk
- Dapat diatur via Serial Monitor untuk kecepatan tertentu
- Cocok untuk testing speedometer, dashboard, atau ECU simulator

Komponen utama:
- Arduino Uno / Nano / Mega
- Shield CAN MCP2515 atau modul CAN bus
- Kabel CAN-H / CAN-L
- 120 ohm terminator bila diperlukan

## Wiring (MCP2515)

- VCC -> 5V Arduino
- GND -> GND Arduino
- CS -> D10
- SI (MOSI) -> D11
- SO (MISO) -> D12
- SCK -> D13
- INT -> D2
- CANH -> bus CAN high
- CANL -> bus CAN low

Catatan:
- Jika menggunakan board CAN shield yang sudah didesain khusus, pin CS/INT bisa berbeda.
- Pastikan bus CAN mempunyai terminator 120 ohm di ujung jaringan.

## Instalasi library

Di Arduino IDE, install library:
- mcp_can
- SPI

## Penggunaan

1. Upload sketch ke Arduino.
2. Hubungkan modul CAN ke bus CAN.
3. Buka Serial Monitor dengan baud 115200.
4. Ketik salah satu perintah berikut lalu tekan Enter:
   - `speed 0`
   - `speed 60`
   - `speed 120`
   - `speed 200`
5. Program akan mengirim frame CAN secara periodik dengan nilai kecepatan baru.

## Format frame CAN

ID yang digunakan: `0x100`
- Byte 0: high byte kecepatan
- Byte 1: low byte kecepatan
- Byte 2–7: 0

Contoh:
- Kecepatan 60 km/h -> `0x00 0x3C`
- Kecepatan 120 km/h -> `0x00 0x78`

## Catatan

Sketch ini dimaksudkan sebagai simulator dasar dengan pendekatan yang mudah dipahami, serta dapat dikembangkan untuk protokol CAN yang lebih kompleks seperti J1939, OEM speedometer message, atau komunikasi dengan dashboard instrument cluster.
