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
#endif

#define HASH_SIZE 32
#define BLOCK_SIZE 64

SHA256 sha256;
byte buffer[128];
// const char *key = "Flightcacse2025";
// const char *key = "b3BlbnNzaC1rZXktdjEAAAAABG5vbmUAAAAEbm9uZQAAAAAAAAABAAABFwAAAAdzc2gtcn
// NhAAAAAwEAAQAAAQEA4QDqpS9IX8PvWkNNHaWsFJ4ZFgCy/zt2FLR4Ld3sTaBCzEfg47Ej
// cG7tRJjGkb71tj1OPN0aDymSZ2KIjZ47eLBZihO1qezkfGRs7Xa0+nYABaZyDzvUwvyP4K
// qwSY0trDX/CVwS+DW7RlK4/l4ZXOyaDmQ4lWwf6XOUXtl7ruP/Lcw2ed/Oa0icdmBx4jCk
// lgk3EttnQHr5k2o8AwrowHQHriDemRrabrP4Ir+DS51lWHdMvPaDFOzMvV2jUmgtmTriFg
// wb9xkAWwYtux77JFlJgeAK65dfTUBnxcAtf+2i7AJ2QO2IqILdFSI2gRobyBVsfNX4gQwe
// ZE9s6iHQ1QAAA8i48bk2uPG5NgAAAAdzc2gtcnNhAAABAQDhAOqlL0hfw+9aQ00dpawUnh
// kWALL/O3YUtHgt3exNoELMR+DjsSNwbu1EmMaRvvW2PU483RoPKZJnYoiNnjt4sFmKE7Wp
// 7OR8ZGztdrT6dgAFpnIPO9TC/I/gqrBJjS2sNf8JXBL4NbtGUrj+Xhlc7JoOZDiVbB/pc5
// Re2Xuu4/8tzDZ5385rSJx2YHHiMKSWCTcS22dAevmTajwDCujAdAeuIN6ZGtpus/giv4NL
// nWVYd0y89oMU7My9XaNSaC2ZOuIWDBv3GQBbBi27HvskWUmB4Arrl19NQGfFwC1/7aLsAn
// ZA7Yiogt0VIjaBGhvIFWx81fiBDB5kT2zqIdDVAAAAAwEAAQAAAQBQGjbvCse9zC/0lvwn
// v1jlWCl/I02WPvF5Sox0a8raEeFl/3rpJKGEwAD5SFh8/5pTQD0Pvvkth7PG2ZXasQfkHg
// gqd3zXm232ePwsh/PDLOoyGMEHelny9t4z8iBjdv7ZDboXd2+LbPmYfg+9AvwNkHACCpz4
// WS99OXgLUmrNZlP+OXYw8ygM67hkDu0e2ZywyfObiBvcN165l0tjIeFusInPa+Fsw5zLv4
// eij2g0bGj4tUtcelvAysvl0KjN0keCKyLCKrzHK2LYTbQZfxARsK2yWXOhtjZOzaDpw5Rt
// xMR+gLn36J5T3TRRG0Bq132lwNHcT15gqIVuRtrWHC37AAAAgGrUp8goWUQFMaC3/Amdos
// nzV6hmEHWZTXxJyqHUVkr3lJHQliZg8xOrLOrSuc6N2dKpsxusy/bK7YlI/vj/jD5iFFUi
// NquVbDFGRqI0Rg37KeynowZjCPlkfrvQKX2ksXAO1DcViyhjEPokaGjYb9/32mgCgYF625
// voud1QSF9kAAAAgQDqg69JpQ2wNUpDjmxUVnm/SAqhGNqsxkTEQSleZ45BkHRJx6dUnG/U
// MiQNSmqN8RSIQO7KHZoJ8qMy8KX76xILwLrFVO6WFCN1dAQzPIU6TVd9M4brNMp6AtIVMZ
// 2DsuJIYCKUffnKQ3ZjtP0al3Tcxp+sEtWcfI2zJbkHiuANLwAAAIEA9Z4qJzRzkd0N/u+K
// /OlAK2svSDG2V8zNmsbqgiurvW10gkhlFp4D42as/V7q4uk9LYzpcbYSSt+D3I8x9ItoLZ
// H2pvreNqaz+NZQF18Utqpwuqrz4JVMVq8H9a7UwJ6XTAFQZF5j31ml7juHmJb8m4x+5rju
// /P3iVkzz21Gx6TsAAAASZGFuaWVsbWV5ZXJAcG9wLW9zAQ==";

const int LED = 8;
boolean ledON = 1;

void print_hash(uint8_t *arr, uint8_t len) {
  // char buffer[len];

  Serial.println("+ Hash +");
  for (int i = 0; i < len; i++) {

    if (arr[i] < 0x10) { // Check if the value is less than 16 (0x10)
      Serial.print('0'); // Print a leading '0' if needed
    }
    Serial.print(arr[i], HEX); // Print in hexadecimal format
    // sprintf(format, "0x%%.%dX", 8);
    // sprintf(buffer, "0x%%.%dX", arr[i]); 
    // Serial.print(buffer);
    Serial.print(" "); // Add a space for separation

    if(i > 0 && i % 8 == 0) {
      Serial.println(); // Newline at the end
    }
  }
  Serial.println(); // Newline at the end
  Serial.println("- Hash -");
}

void print_array(uint8_t *arr, uint8_t len) {
  Serial.println("+ Hash +");
  for (int i = 0; i < len; i++) {
    Serial.print(arr[i]);
  }
  Serial.println(); // Newline at the end
  Serial.println("- Hash -");
}

void sha256_HMAC(Hash *hash, const char *key, const unsigned char *data)
{
  uint8_t result[HASH_SIZE];

  hash->resetHMAC(key, strlen(key));
  hash->update(data, strlen((const char*)data));
  hash->finalizeHMAC(key, strlen(key), result, sizeof(result));

  // If the first test passed, then try the all-in-one function too.
  if (!memcmp(result, hash, HASH_SIZE)) {
    memset(result, 0xAA, sizeof(result));
    hmac<SHA256>(result, HASH_SIZE, key, strlen(key), data, strlen((const char*)data));
  }

  print_hash(result, HASH_SIZE);
}

void my_sha256(Hash *hash, const unsigned char *data)
{
  uint8_t result[HASH_SIZE];
  // size_t size = strlen(test->data);
  // size_t posn, len;

  hash->reset();
  hash->update(data, strlen((const char*)data));
  hash->finalize(result, sizeof(result));
  // print_hash(result, HASH_SIZE);
  print_array(result, HASH_SIZE);
  // for (posn = 0; posn < size; posn += inc) {
  //     len = size - posn;
  //     if (len > inc)
  //         len = inc;
  //     hash->update(test->data + posn, len);
  // }
}

void setup() {
  SERIAL_PORT_MONITOR.begin(115200);
  pinMode(LED, OUTPUT);

  while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
    SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
    delay(100);
  }
  SERIAL_PORT_MONITOR.println("CAN init ok!");
  SERIAL_PORT_MONITOR.println("");
  SERIAL_PORT_MONITOR.println("");
  SERIAL_PORT_MONITOR.println("");
  SERIAL_PORT_MONITOR.println("");
  SERIAL_PORT_MONITOR.println("");
  SERIAL_PORT_MONITOR.println("");

  
  // const unsigned char *data = "This is a test string";
  // sha256_HMAC(&sha256, key, data);
}


void loop() {
  unsigned char len = 0;
  unsigned char buf[8];
  

  // if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
  //     CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
  //     unsigned long canId = CAN.getCanId();

  //     // SERIAL_PORT_MONITOR.println("-----------------------------");
  //     // SERIAL_PORT_MONITOR.print("get data from ID: 0x");
  //     SERIAL_PORT_MONITOR.println(canId, HEX);

  //     for (int i = 0; i < len; i++) { // print the data
  //         SERIAL_PORT_MONITOR.print(buf[i]);
  //         SERIAL_PORT_MONITOR.print("\t");
  //         if (ledON && i == 0) {

  //             digitalWrite(LED, buf[i]);
  //             ledON = 0;
  //             delay(500);
  //         } else if ((!(ledON)) && i == 4) {

  //             digitalWrite(LED, buf[i]);
  //             ledON = 1;
  //         }
  //     }
  //     SERIAL_PORT_MONITOR.println();
  //     // Serial.print("Original Data: ");
  //     // Serial.println(data);
  //     sha256_HMAC(&sha256, key, buf);
  // }

  Serial.println("+ Data +");
  // Serial.println("This is a test string");
  Serial.println("test");
  Serial.println("- Data -");
  
  // const unsigned char *data = "This is a test string";
  const unsigned char *data = "test";
  my_sha256(&sha256, data);
  // sha256_HMAC(&sha256, key, data);

  // Reduce loop rate, wait 1sec
  delay(1000);
}

//END FILE