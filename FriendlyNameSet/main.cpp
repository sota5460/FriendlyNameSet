#include <stdio.h>
#include<Windows.h>
#include <setupapi.h>
#include <iostream>
#include <combaseapi.h>

#pragma comment(lib,"setupapi.lib")

#define MATCH_PID_VID "PID_5740"

//TCHAR型配列のポインタ,配列長さ,char型配列ポインタを渡すと、16bitのTCHAR型配列を8bitのchar型配列に直したものがcharbufに格納される。
void convert16TCHAR_8CHAR(TCHAR* PTCHAR,long len, char* charbuf) {

	std::string tmpChar((char*)PTCHAR, len);
	int j = 0;

	for (int i = 0; i <len; i++) {
		if ((i % 2 == 0)) {
			charbuf[j] = tmpChar[i];
			j++;
		}
	}

}

int main() {

	HDEVINFO DeviceInfoSet;
	SP_DEVINFO_DATA DeviceInfoData;
	ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));

	DWORD Property;
	const BYTE* PropertyBuffer;
	DWORD PropertyBufferSize;

	Property = SPDRP_FRIENDLYNAME;

	DeviceInfoSet = SetupDiGetClassDevs(NULL,0,0,DIGCF_PRESENT | DIGCF_ALLCLASSES);

	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
		return -1;

	//ZeroMemory(*DeviceInfoData, sizeof(SP_DEVINFO_DATA));

	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	DWORD dwIndex = 0;
	DWORD dwRegType;
	DWORD dwSize;
	BOOL bRet;
	TCHAR* pid_vid_buf;
	char* pid_vid_char_buf = nullptr;

	TCHAR* currentFriendlyName_buf;
	DWORD currentFriendlyName_size;
	BOOL settingRet;

	const BYTE friendlyName[20]={};

	char DisplayDeviceName_COM[] = { 'H',NULL,'P',NULL,'K',NULL}; // <==ここにデバイスマネージャーに表示したいデバイス名記述。char(8byte)につき一つNULLを挿入すること。
	

	char suffix_com1[] = { 0x20,NULL,'(',NULL,'C',NULL,'O',NULL,'M',NULL,'x',NULL,')',NULL ,NULL,NULL};  //0x20:空白文字
	char suffix_com2[] = { 0x20,NULL,'(',NULL,'C',NULL,'O',NULL,'M',NULL,'x',NULL,'y',NULL,')',NULL,NULL,NULL};
	
	char* friendlyName_buf = nullptr;
	friendlyName_buf = new char[sizeof(DisplayDeviceName_COM) + sizeof(suffix_com1)];
	DWORD friendlyName_buf_size = sizeof(DisplayDeviceName_COM) + sizeof(suffix_com1);
	

	while (1) {

		bRet = SetupDiEnumDeviceInfo(DeviceInfoSet, dwIndex++, &DeviceInfoData);

		if (bRet == FALSE)
			break;

		//bRet = SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, &dwRegType,NULL, 0, &dwSize);
		bRet = SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID, &dwRegType, NULL, 0, &dwSize);
		pid_vid_buf = new TCHAR[dwSize];
		

		if (pid_vid_buf == NULL) //dwSizeが0のとき
			break;
		
		//bRet = SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, &dwRegType, (BYTE*)pid_vid_buf, dwSize, &dwSize);
		bRet = SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID, &dwRegType, (BYTE*)pid_vid_buf, dwSize, &dwSize);
		if (bRet == FALSE) {
			printf("error\n");
			//break; //ここで抜けるとおかしくなる。最後まで回らない。特定のPropertyが設定されていないとfalseを返す。deviceは結構存在する。
		}

		
		//とりあえず動くコード
		//std::string s((char*) pid_vid_buf, 41);
		//std::string pid_vid = {};
		//for (int i = 0; i < 21; i++) {
		//	pid_vid += s[i * 2];
		//}


		pid_vid_char_buf = new char[dwSize/2];

		for (int i = 0; i < (dwSize/2); i++) {
			pid_vid_char_buf[i] = (BYTE)pid_vid_buf[i];
			//printf("%c is inside \n", (BYTE)pid_vid_buf[i]);
		}

		std::string pid_vid_array(pid_vid_char_buf, dwSize);
		
		std::string pid_vid= {};

		for (int i = 0; i < (dwSize/2); i++) {
			pid_vid += pid_vid_array[i];
		}


		if (pid_vid.find(MATCH_PID_VID) != std::string::npos) {
			printf("Pid found\n");

			//SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, &dwRegType, NULL, 0, &currentFriendlyName_size);

			//currentFriendlyName_buf = new TCHAR[currentFriendlyName_size];

			//SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, &dwRegType, (BYTE*)currentFriendlyName_buf, dwSize, &dwSize);

			////(COM0) - (COM9)の場合　後ろから４文字目（終端コード含めて）がMかどうかで判断
			//if ((char)currentFriendlyName_buf[currentFriendlyName_size /2 - 4] == 'M') {
			//	//suffix_com1のxを実際のport番号に置き換え
			//	suffix_com1[10] = (char)currentFriendlyName_buf[currentFriendlyName_size/2 - 3];
			//	
			//	memcpy(friendlyName_buf, DisplayDeviceName_COM, sizeof(DisplayDeviceName_COM));
			//	memcpy(friendlyName_buf + sizeof(DisplayDeviceName_COM), suffix_com1, sizeof(suffix_com1));


			//	//printf("size of displaydevicename is %u \n", sizeof(DisplayDeviceName_COM));
			//	//printf("size of displaydevicename is %u \n", sizeof(suffix_com1));

			//	//バッファの中身確認用
			//	for (int i = 0; i < sizeof(DisplayDeviceName_COM) + sizeof(suffix_com1); i++) {
			//		printf("%d byte code  => %c \n",i, *(friendlyName_buf + i));
			//	}

			//	SetupDiSetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, (BYTE*)friendlyName_buf, friendlyName_buf_size);

			//}
			//else if ((char)currentFriendlyName_buf[currentFriendlyName_size / 2 - 5] == 'M') {
			//	suffix_com2[10] = (char)currentFriendlyName_buf[currentFriendlyName_size / 2 - 4];
			//	suffix_com2[12] = (char)currentFriendlyName_buf[currentFriendlyName_size / 2 - 3];

			//	memcpy(friendlyName_buf, DisplayDeviceName_COM, sizeof(DisplayDeviceName_COM));
			//	memcpy(friendlyName_buf + sizeof(DisplayDeviceName_COM), suffix_com2, sizeof(suffix_com2));
			//	
			//	SetupDiSetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, (BYTE*)friendlyName_buf, friendlyName_buf_size);

			//}
			//else {
			//	return -1;
			//}

			//SetupDiSetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, (BYTE*)f, friendlyName_buf_size);
			//settingRet=SetupDiSetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, (BYTE*)f, currentFriendlyName_size);

		}
		delete[] pid_vid_buf;
		delete[] pid_vid_char_buf;

	}
	printf("the number of devinfo is %d \n", dwIndex);

	printf("done");
	//SetupDiSetDeviceRegistryPropertyA(DeviceInfoSet,DeviceInfoData,Property,PropertyBuffer,PropertyBufferSize);

}