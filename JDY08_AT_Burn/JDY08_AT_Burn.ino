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
char Minor[5] = "12AF";
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
//NO.1 ��¼
bool S1_Burn(uint16_t num);		//����1Ϊ�ɹ���0Ϊʧ��
//bool S2_Burn(uint16_t num);		//����1Ϊ�ɹ���0Ϊʧ��
//bool S3_Burn(uint16_t num);		//����1Ϊ�ɹ���0Ϊʧ��


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
	//write_EEPROM_data(UUID, Major, Minor);		//��EEPROMд�����
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
	HexToStr(Minor_hex, Minor);
	write_EEPROM_data(UUID, Major, Minor);		//��EEPROMд�����
	digitalWrite(S1_LED, 0);
	digitalWrite(S2_LED, 0);
	digitalWrite(S3_LED, 0);
	Serial.setTimeout(150);
}

// Add the main program code into the continuous loop() function
void loop()
{
	if (0 == digitalRead(Button_pin))
	{
		digitalWrite(Button_LED, 0);
		digitalWrite(S1_LED, 0);
		digitalWrite(S2_LED, 0);
		digitalWrite(S3_LED, 0);
		Serial.println("Start burning!");
		if (S1_Burn(Minor_hex))
		{
			Minor_hex++;
			HexToStr(Minor_hex, Minor);
			write_EEPROM_data(UUID, Major, Minor);		//��EEPROMд�����
			digitalWrite(S1_LED, 1);
		}
		if (S2_Burn(Minor_hex))
		{
			Minor_hex++;
			HexToStr(Minor_hex, Minor);
			write_EEPROM_data(UUID, Major, Minor);		//��EEPROMд�����
			digitalWrite(S2_LED, 1);
		}
		if (S3_Burn(Minor_hex))
		{
			Minor_hex++;
			HexToStr(Minor_hex, Minor);
			write_EEPROM_data(UUID, Major, Minor);		//��EEPROMд�����
			digitalWrite(S3_LED, 1);
		}
		while (!digitalRead(Button_pin))
			;
		digitalWrite(Button_LED, 1);
	}


	if (Serial.available()) {      // If anything comes in Serial (USB),
		Serial1.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
	}

	if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)
		Serial.write(Serial1.read());   // read it and send it out Serial (USB)
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

bool S1_Burn(uint16_t num)	//����1Ϊ�ɹ���0Ϊʧ��
{
	char str_Serial[50];
	memset(str_Serial, NULL, 50);
	Serial1.write("AT+HOSTEN3");
	Serial1.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.1 Mode ERROR!!!");
		Serial.write(str_Serial);
		Serial.println(" ");
		return 0;
	}
	else
	{
		Serial.println("NO.1 Mode OK!");
	}

	Serial1.write("AT+RST");
	delay(200);	//�����Բ��ܸ�С

	memset(str_Serial, NULL, 50);
	Serial1.write("AT+STRUUID");
	Serial1.write(UUID,32);
	Serial1.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.1 STRUUID ERROR!!!");
		Serial.write(str_Serial);
		Serial.println(" ");
		return 0;
	}
	else
	{
		Serial.println("NO.1 STRUUID OK!");
	}

	memset(str_Serial, NULL, 50);
	Serial1.write("AT+MAJOR");
	Serial1.write(Major,4);
	Serial1.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.1 Major ERROR!!!");
		Serial.write(str_Serial);
		Serial.println();
		return 0;
	}
	else
	{
		Serial.println("NO.1 Major OK!");
	}

	delay(300);
	memset(str_Serial, NULL, 50);
	Serial1.write("AT+MINOR");
	Serial1.write(Minor,4);
	delay(20);
	Serial1.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.1 Minor ERROR!!!");
		Serial.write(str_Serial);
		Serial.println();
		return 0;
	}
	else
	{
		Serial.print("NO.1 Minor is:");
		Serial.write(Minor, 4);
		Serial.println(" ");
	}
	return 1;
}



bool S2_Burn(uint16_t num)	//����1Ϊ�ɹ���0Ϊʧ��
{
	char str_Serial[50];
	memset(str_Serial, NULL, 50);
	Serial2.write("AT+HOSTEN3");
	Serial2.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.2 Mode ERROR!!!");
		Serial.write(str_Serial);
		Serial.println();
		return 0;
	}
	else
	{
		Serial.println("NO.2 Mode OK!");
	}

	Serial1.write("AT+RST");
	delay(200);	//�����Բ��ܸ�С

	memset(str_Serial, NULL, 50);
	Serial2.write("AT+STRUUID");
	Serial2.write(UUID, 32);
	Serial2.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.2 STRUUID ERROR!!!");
		Serial.write(str_Serial);
		Serial.println();
		return 0;
	}
	else
	{
		Serial.println("NO.2 STRUUID OK!");
	}

	memset(str_Serial, NULL, 50);
	Serial2.write("AT+MAJOR");
	Serial2.write(Major, 4);
	Serial2.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.2 Major ERROR!!!");
		Serial.write(str_Serial);
		Serial.println();
		return 0;
	}
	else
	{
		Serial.println("NO.2 Major OK!");
	}

	delay(300);
	memset(str_Serial, NULL, 50);
	Serial2.write("AT+MINOR");
	Serial2.write(Minor, 4);
	delay(20);
	Serial2.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.2 Minor ERROR!!!");
		Serial.write(str_Serial);
		Serial.println();
		return 0;
	}
	else
	{
		Serial.print("NO.2 Minor is:");
		Serial.write(Minor, 4);
		Serial.println(" ");
	}
	return 1;
}

bool S3_Burn(uint16_t num)	//����1Ϊ�ɹ���0Ϊʧ��
{
	char str_Serial[50];
	memset(str_Serial, NULL, 50);
	Serial3.write("AT+HOSTEN3");
	Serial3.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.3 Mode ERROR!!!");
		Serial.write(str_Serial);
		Serial.println();
		return 0;
	}
	else
	{
		Serial.println("NO.3 Mode OK!");
	}

	Serial1.write("AT+RST");
	delay(200);	//�����Բ��ܸ�С

	memset(str_Serial, NULL, 50);
	Serial3.write("AT+STRUUID");
	Serial3.write(UUID, 32);
	Serial3.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.3 STRUUID ERROR!!!");
		Serial.write(str_Serial);
		Serial.println();
		return 0;
	}
	else
	{
		Serial.println("NO.3 STRUUID OK!");
	}

	memset(str_Serial, NULL, 50);
	Serial3.write("AT+MAJOR");
	Serial3.write(Major, 4);
	Serial3.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.3 Major ERROR!!!");
		Serial.write(str_Serial);
		Serial.println();
		return 0;
	}
	else
	{
		Serial.println("NO.3 Major OK!");
	}

	delay(300);
	memset(str_Serial, NULL, 50);
	Serial3.write("AT+MINOR");
	Serial3.write(Minor, 4);
	delay(20);
	Serial3.readBytesUntil('\n', str_Serial, 10);
	if (str_Serial[1] != 'O' && str_Serial[2] != 'K')
	{
		Serial.println("NO.3 Minor ERROR!!!");
		Serial.write(str_Serial);
		Serial.println();
		return 0;
	}
	else
	{
		Serial.print("NO.3 Minor is:");
		Serial.write(Minor, 4);
		Serial.println(" ");
	}
	return 1;
}

