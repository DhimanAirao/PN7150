#include <T4T_NDEF_emu.h>
#include "Electroniccats_PN7150.h"
#define PN7150_IRQ   (23)
#define PN7150_VEN   (19)
#define PN7150_ADDR  (0x28)
#define ADD 0 // Enlarge NDEF message by adding dummy content

Electroniccats_PN7150 nfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR); // creates a global NFC device interface object, attached to pins 7 (IRQ) and 8 (VEN) and using the default I2C address 0x28
RfIntf_t RfInterface;

unsigned char STATUSOK[] = {0x90, 0x00}, Cmd[256], CmdSize;
uint8_t mode = 2;                                                  // modes: 1 = Reader/ Writer, 2 = Emulation

String str = "HELLO";

//char NDEF_MESSAGE[10 + ADD] = { 0xC1,   // MB/ME/CF/1/IL/TNF
//                                0x01,   // TYPE LENGTH
//                                (0x07 + ADD) >> 24,   // PAYLOAD LENTGH MSB
//                                (0x07 + ADD) >> 16,   // PAYLOAD LENTGH
//                                (0x07 + ADD) >> 8,    // PAYLOAD LENTGH
//                                (0x07 + ADD) & 0xFF,  // PAYLOAD LENTGH LSB
//                                'T',    // TYPE
//                                // PAYLOAD
//                                0x02,   // Status
//                                'e', 'n', // Language
//                              };

const char NDEF_MESSAGE[14 + ADD] = { 0xC1,   // MB/ME/CF/1/IL/TNF
        0x01,   // TYPE LENGTH
        (0x07 + ADD) >> 24,   // PAYLOAD LENTGH MSB
        (0x07 + ADD) >> 16,   // PAYLOAD LENTGH
        (0x07 + ADD) >> 8,    // PAYLOAD LENTGH
        (0x07 + ADD) & 0xFF,  // PAYLOAD LENTGH LSB
        'T',    // TYPE
    // PAYLOAD
        0x02,   // Status
        'e', 'n', // Language
        'T', 'e', 's', 't' };

void NdefPush_Cb(unsigned char *pNdefRecord, unsigned short NdefRecordSize) {
  Serial.println("--- NDEF Record sent ---\n\n");
}

//void append_str()
//{
//  int str_len = str.length() + 1;
//  char char_array[str_len];
//  str.toCharArray(char_array, str_len);
//  for (int i = 0; i < str_len; i++)
//  {
//    NDEF_MESSAGE[10 + i] = char_array[i];
//  }
//}

void setup()
{
 // pinMode(19, OUTPUT);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Detect NFC readers with PN7150");
  Serial.println("Initializing...");

  if (nfc.connectNCI()) { //Wake up the board
    Serial.println("Error while setting up the mode, check connections!");
    while (1);
  }

  if (nfc.ConfigureSettings()) {
    Serial.println("The Configure Settings is failed!");
    while (1);
  }

  if (nfc.ConfigMode(mode)) { //Set up the configuration mode
    Serial.println("The Configure Mode is failed!!");
    while (1);
  }
  nfc.StartDiscovery(mode); //NCI Discovery mode
  Serial.println("Waiting for an Reader Card ...");
}

void loop()
{
  //nfc.FlushReception();
  //append_str();
  T4T_NDEF_EMU_SetMessage((unsigned char *) NDEF_MESSAGE, sizeof(NDEF_MESSAGE), NdefPush_Cb);
  T4T_NDEF_EMU_PullCallback(NdefPull_Cb);
  //Serial.println(status);

  while (nfc.WaitForDiscoveryNotification(&RfInterface) != NFC_SUCCESS);
  if ((RfInterface.Interface == INTF_ISODEP) && ((RfInterface.ModeTech & MODE_LISTEN) == MODE_LISTEN))
  {
    Serial.println("LISTEN MODE: Activated from remote Reader");
    nfc.ProcessCardMode(RfInterface);
    Serial.println("READER DISCONNECTED\n");
  }
}
