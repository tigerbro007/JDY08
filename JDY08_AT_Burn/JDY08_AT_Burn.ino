// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       JDY08_AT_Burn.ino
    Created:	2018/8/24 9:14:48
    Author:     ZXD
	��;�����ڸ�JDY08 IBeaconģ��������ò�����һ�ο�����4ƬIBeacon��
	���Զ����ģ�����ޣ���LED��ָʾ��¼״̬��
	���ڼ�����������Ϊ115200����ͨ�����ڼ���������IBeacon������
*/

#include <EEPROM.h>

#define BAUD_RATE 115200	//������
#define SERIAL_WAIT_TIME 40000		//��������ȴ�ʱ��
#define Button_pin 9
#define S1_LED 4
#define S2_LED 3
#define S3_LED 2
#define Button_LED 8

char UUID[33] = "FDA50693A4E24FB1AFCFC6EB07647825";		//����Ϊ�ַ���
char Major[5] = "27CA";
char Minor[5] = "";
uint16_t Minor_hex;


/*
	��EEPROM�洢�ϴε����ò���
	��ַ0-31���ɸ�λ����λ�洢UUID
	��ַ32-35���ɸ�λ����λ�洢Major
	��ַ36-39���ɸ�λ����λ�洢Minor
*/
const uint16_t EEPROM_address = 0;	//EEPROM�ĳ�ʼ��ַ

void read_EEPROM_data(char *UUID_Rom, char *Major_Rom, char *Minor_Rom);
void write_EEPROM_data(char *UUID_Rom, char *Major_Rom, char *Minor_Rom);
//��ʮ�����Ƶ�ACSII�ַ���ת��������
uint16_t StrToHex(char *str_read);
//������תΪACSII�ַ���
void HexToStr(uint16_t number, char *str_out);

// The setup() function runs once each time the micro-controller starts
void setup()
{
	char UUID_Rom[33] = "01234567890123456789012345678901";
	char Major_Rom[5] = "0123";
	char Minor_Rom[5] = "0123";
	char string_read[10];

	//Ӳ����ʼ��
	Serial.begin(BAUD_RATE);
	Serial.setTimeout(SERIAL_WAIT_TIME);
	Serial1.begin(BAUD_RATE);
	Serial2.begin(BAUD_RATE);
	Serial3.begin(BAUD_RATE);
	pinMode(Button_pin, INPUT_PULLUP);
	pinMode(S1_LED, OUTPUT);
	pinMode(S2_LED, OUTPUT);
	pinMode(S3_LED, OUTPUT);
	pinMode(Button_LED, OUTPUT);
	digitalWrite(S1_LED, 1);
	digitalWrite(S2_LED, 1);
	digitalWrite(S3_LED, 1);
	digitalWrite(Button_LED, 1);

	//�ȴ����ھ���
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB
	}

	delay(100);
	Serial.print("Start system! Read the EEPROM.\n");
	//write_EEPROM_data(UUID, Major, Minor);
	read_EEPROM_data(UUID_Rom, Major_Rom, Minor_Rom);
	Serial.print("UUID_ROM = ");
	Serial.print(UUID_Rom);
	Serial.println();
	Serial.print("Major_ROM = ");
	Serial.print(Major_Rom);
	Serial.println();
	Serial.print("Minor_ROM = ");
	Serial.print(Minor_Rom);
	Serial.println();
	Serial.println("Is Minor data Right(Y/N)?");
	Serial.readBytes(string_read, 1);
	if ('N' == string_read[0] || 'n' == string_read[0])
	{
		memset(string_read, NULL, 33);
		Serial.println("Enter the new Minor in hex. Do not enter '0x'! ");
		Serial.readBytes(string_read, 1);
		Serial.setTimeout(100);
		Serial.readBytes(string_read+1, 3);
		Minor_hex = StrToHex(string_read);		//��������ַ���תΪ16���ƴ洢
		Serial.print("New Minor is: ");
		Serial.println(Minor_hex, HEX);
	}
	else
	{
		Minor_hex = StrToHex(Minor_Rom);		//��������ַ���תΪ16���ƴ洢
	}
	Serial.println("The parameter settings are complete. Ready to burn.");
	digitalWrite(S1_LED, 0);
	digitalWrite(S2_LED, 0);
	digitalWrite(S3_LED, 0);
	Serial.setTimeout(50);
}

// Add the main program code into the continuous loop() function
void loop()
{
	if (1 == digitalRead(Button_pin))
	{
		Serial.println("Start burning!");

		Serial.print("");
	}
	//HexToStr(Minor_hex, Minor);
}




/*
	void read_EEPROM_data(char *UUID_Rom, char *Major_Rom, char *Minor_Rom);
	���ڶ�ȡEEPROM�д洢�Ĳ�����
	���룺�����������ַ������ᱻָ������ġ�
	�������
*/
void read_EEPROM_data(char *UUID_Rom, char *Major_Rom, char *Minor_Rom)
{
	for (int i = 0; i < 32; i++)
	{
		*(UUID_Rom + i) = EEPROM.read(EEPROM_address + i);
	}
	for (int i = 0; i < 4; i++)
	{
		*(Major_Rom + i) = EEPROM.read(EEPROM_address + i + 32);
	}
	for (int i = 0; i < 4; i++)
	{
		*(Minor_Rom + i) = EEPROM.read(EEPROM_address + i + 36);
	}
}

/*
	void write_EEPROM_data(char *UUID_Rom, char *Major_Rom, char *Minor_Rom)��
	���ڰѲ���д��EEPROM�С�
	���룺�����������ַ���
*/
void write_EEPROM_data(char *UUID_Rom, char *Major_Rom, char *Minor_Rom)
{
	for (int i = 0; i < 32; i++)
	{
		EEPROM.write(EEPROM_address + i, *(UUID_Rom + i));
	}
	for (int i = 0; i < 4; i++)
	{
		EEPROM.write(EEPROM_address + i + 32, *(Major_Rom + i));
	}
	for (int i = 0; i < 4; i++)
	{
		EEPROM.write(EEPROM_address + i + 36, *(Minor_Rom + i));
	}
}

//����дתΪСд
int tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
	{
		return c + 'a' - 'A';
	}
	else
	{
		return c;
	}
}

/*
	���ܣ���ʮ�����Ƶ�ACSII�ַ���ת������������֧��16λ�����µġ�
	���룺��Ҫ��ת�����ַ���
	�������Ӧ������
*/
uint16_t StrToHex(char *str_read)
{
	uint8_t i = 0;
	uint16_t n = 0;
	for (; (*(str_read+i) >= '0' && *(str_read + i) <= '9') || (*(str_read + i) >= 'a' && *(str_read + i) <= 'f')\
			|| (*(str_read + i) >= 'A' && *(str_read + i) <= 'F'); ++i)
	{
		if (tolower(*(str_read + i)) > '9')
		{
			n = 16 * n + (10 + tolower(*(str_read + i)) - 'a');
		}
		else
		{
			n = 16 * n + (tolower(*(str_read + i)) - '0');
		}
	}
	if (i != 4)
		return 0;
	else
		return n;
}

/*
	void HexToStr(uint16_t number, char *str_out)��
	���ܣ�������תΪ��Ӧ���ַ���
	���룺number����������֡�
		  *str_out����ָ����ʽ���ת������ַ���
	�������
*/
void HexToStr(uint16_t number, char *str_out)
{
	char num_str[17] = "0123456789ABCDEF";
	char n;
	uint16_t i;
	i = number;
	i = (i & 0xf000) >> 12;
	*str_out = num_str[i];
	i = number;
	i = (i & 0x0f00) >> 8;
	*(str_out+1) = num_str[i];
	i = number;
	i = (i & 0x00f0) >> 4;
	*(str_out+2) = num_str[i]; 
	i = number;
	i = i & 0x000f;
	*(str_out+3) = num_str[i];
}

/*


*/
bool S1_Burn(uint16_t num)
{
	char minor_Serial[5];
	char str_Serial[50];
	memset(str_Serial, NULL, 50);
	HexToStr(num, minor_Serial);
	Serial1.write("AT+HOSTEN3");
	Serial1.readBytesUntil()

}

