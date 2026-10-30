#include <SPI.h>
#include <mcp_can.h>

// --- Pin konfigurasi CAN Shield ---
const int SPI_CS_PIN = 10;
const int CAN_INT_PIN = 2;

MCP_CAN CAN(SPI_CS_PIN); // Pin CS MCP2515

unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL_MS = 100;

uint16_t speedKmh = 0;
bool increaseSpeed = true;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  Serial.println("CAN Bus Speedometer Simulator Tester");
  Serial.println("Initializing CAN...");

  // Gunakan MCP_8MHZ jika crystal 8MHz.
  // Jika modul Anda menggunakan 16MHz, ganti MCP_8MHZ menjadi MCP_16MHZ.
  if (CAN_OK != CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ)) {
    Serial.println("CAN BUS Shield failed to initialize.");
    Serial.println("Check wiring and MCP2515 module.");
    while (1) {
      delay(1000);
    }
  }

  CAN.setMode(MCP_NORMAL);
  Serial.println("CAN BUS Shield initialized successfully.");
  Serial.println("Type command: speed <value> (example: speed 80)");
}

void loop() {
  readSerialCommand();
  readIncomingCanMessage();

  if (millis() - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = millis();

    // Contoh efek speed ramp agar simulator terasa seperti kendaraan yang bergerak.
    if (speedKmh >= 220) {
      increaseSpeed = false;
    } else if (speedKmh <= 0) {
      increaseSpeed = true;
    }

    if (increaseSpeed) {
      speedKmh += 2;
    } else {
      speedKmh -= 2;
    }

    sendSpeedFrame(speedKmh);
  }
}

void readIncomingCanMessage() {
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    long unsigned int rxId = 0;
    unsigned char len = 0;
    unsigned char rxBuf[8];

    CAN.readMsgBuf(&rxId, &len, rxBuf);

    Serial.print("RX ID: 0x");
    Serial.print(rxId, HEX);
    Serial.print(" | Len: ");
    Serial.print(len);
    Serial.print(" | Data: ");

    for (int i = 0; i < len; i++) {
      if (rxBuf[i] < 0x10) {
        Serial.print("0");
      }
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void readSerialCommand() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.length() == 0) {
      return;
    }

    if (command.startsWith("speed")) {
      String value = command.substring(5);
      value.trim();

      if (value.length() > 0) {
        int newSpeed = value.toInt();
        if (newSpeed >= 0 && newSpeed <= 250) {
          speedKmh = (uint16_t)newSpeed;
          Serial.print("Speed set to: ");
          Serial.print(speedKmh);
          Serial.println(" km/h");
          sendSpeedFrame(speedKmh);
        } else {
          Serial.println("Invalid speed. Use range 0-250 km/h.");
        }
      }
    } else if (command == "stop") {
      speedKmh = 0;
      sendSpeedFrame(speedKmh);
      Serial.println("Speed set to 0 km/h");
    } else {
      Serial.println("Unknown command. Use: speed <value> or stop");
    }
  }
}

void sendSpeedFrame(uint16_t speedValue) {
  unsigned char payload[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  // Format data: [highByte, lowByte, ...]
  payload[0] = (speedValue >> 8) & 0xFF;
  payload[1] = speedValue & 0xFF;

  // ID CAN untuk simulasi speedometer
  const unsigned int speedMsgId = 0x100;

  byte result = CAN.sendMsgBuf(speedMsgId, 0, 8, payload);

  if (result == CAN_OK) {
    Serial.print("TX ID: 0x");
    Serial.print(speedMsgId, HEX);
    Serial.print(" | Speed: ");
    Serial.print(speedValue);
    Serial.println(" km/h");
  } else {
    Serial.println("Error: CAN send failed");
  }
}
