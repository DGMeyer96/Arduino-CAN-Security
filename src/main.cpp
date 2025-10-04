#include <Arduino.h>
#include "SHA256.h"
#include <string.h>
#include <Crypto.h>
#include <SPI.h>

#define CAN_2515
// Set SPI CS Pin according to your hardware
// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;
#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#define MAX_DATA_SIZE 8
#endif

#define HASH_SIZE 32
#define BLOCK_SIZE 64

SHA256 sha256;
byte buffer[128];
const char *key = "Flightcase2025";

const int LED = 8;
boolean ledON = 1;

const String hashMode = "HMAC";

void printHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);

  sprintf(tmp, format, num);
  Serial.print(tmp);
}

void print_hash(uint8_t *arr, uint8_t len) {
  Serial.print("Hash: ");
  for (int i = 0; i < len; i++) {

    if (arr[i] < 0x10) { // Check if the value is less than 16 (0x10)
      Serial.print('0'); // Print a leading '0' if needed
    }
    Serial.print(arr[i], HEX); // Print in hexadecimal format
    Serial.print(" "); // Add a space for separation
  }
  Serial.println(); // Newline at the end
}
void print_hash(const int *arr, uint8_t len) {
  Serial.print("Hash: ");
  for (int i = 0; i < len; i++) {

    if (arr[i] < 0x10) { // Check if the value is less than 16 (0x10)
      Serial.print('0'); // Print a leading '0' if needed
    }
    Serial.print(arr[i], HEX); // Print in hexadecimal format
    Serial.print(" "); // Add a space for separation
  }
  Serial.println(); // Newline at the end
}

void print_can(unsigned long canId, String data, uint8_t len) {
  Serial.print("CanId: ");
  Serial.println(canId, HEX);
  Serial.print("Data: ");
  Serial.print(data);
  Serial.println();
}
void print_can(unsigned long canId, uint8_t data[], uint8_t len) {
  Serial.print("CanId: ");
  Serial.println(canId, HEX);
  Serial.print("Data: ");
  for (int i = 0; i < len; i++) {
    // printHex(buf[i], 2);
    Serial.print(data[i]);
    // Serial.print(" ");
  } 
  Serial.println();
}

void print_array(uint8_t *arr, uint8_t len) {
  Serial.println("HS");
  for (int i = 0; i < len; i++) {
    Serial.print(arr[i]);
  }
  Serial.println(); // Newline at the end
  Serial.println("HE");
}

void sha256_HMAC(Hash *hash, const char *key, const unsigned char *data)
{
  uint8_t result[HASH_SIZE];

  hash->resetHMAC(key, strlen(key));
  hash->update(data, strlen((const char*)data));
  hash->finalizeHMAC(key, strlen(key), result, sizeof(result));

  print_hash(result, HASH_SIZE);
}
void my_sha256_HMAC(Hash *hash, const uint8_t *key, String data)
{
  uint8_t result[HASH_SIZE];
  hash->resetHMAC(key, 8);
  hash->update(data.c_str(), strlen(data.c_str()));
  hash->finalizeHMAC(key, 8, result, sizeof(result));
  print_hash(result, HASH_SIZE);
}
void my_sha256_HMAC(Hash *hash, const char *key, String data)
{
  uint8_t result[HASH_SIZE];
  hash->resetHMAC(key, strlen(key));
  hash->update(data.c_str(), strlen(data.c_str()));
  hash->finalizeHMAC(key, strlen(key), result, sizeof(result));
  print_hash(result, HASH_SIZE);
}

void my_sha256(Hash *hash, const uint8_t *data, uint8_t len)
{
  uint8_t result[HASH_SIZE];
  hash->reset();
  hash->update(data, len);
  hash->finalize(result, sizeof(result));
  print_hash(result, HASH_SIZE);
}
void my_sha256(Hash *hash, String data)
{
  uint8_t result[HASH_SIZE];
  hash->reset();
  hash->update(data.c_str(), strlen(data.c_str()));
  hash->finalize(result, sizeof(result));
  print_hash(result, HASH_SIZE);
}

void setup() {
  // Default Config is 8 data bits, No Parity bits, 1 Stop bit (8N1)
  Serial.begin(250000);
  pinMode(LED, OUTPUT);

  while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
    Serial.println("CAN init fail, retry...");
    delay(100);
  }
  Serial.println("CAN init ok!");
  
  // const unsigned char *data = "This is a test string";
  // sha256_HMAC(&sha256, key, data);
}

void loop() {
  uint8_t len = 0;
  uint8_t buf[8];


  if (CAN_MSGAVAIL == CAN.checkReceive()) { // check if data coming
    CAN.readMsgBuf(&len, buf);              // read data,  len: data length, buf: data buf
    unsigned long canId = CAN.getCanId();

    // Convert the buffer from unigned char to String (needed to match Android)
    String converted;
    for(int i = 0; i < len; i++) {
      converted += String(buf[i], HEX);
    }

    if(hashMode == "SHA256") {
      my_sha256(&sha256, converted);
    } else if (hashMode == "HMAC") {
      my_sha256_HMAC(&sha256, key, converted);
    } else {
      Serial.println("ARDUINO INVALID HASH MODE!!!");
    }
    print_can(canId, converted, len);
  }
  // Apply 1sec throttling to make it easier to read
  delay(1000);
}

//END FILE