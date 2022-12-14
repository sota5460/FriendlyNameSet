#include <stdio.h>
#include<Windows.h>
#include <setupapi.h>
#include <iostream>
#include <combaseapi.h>

#pragma comment(lib,"setupapi.lib")

#define MATCH_PID_VID "PID_5740"

void print_deviceNameChange(TCHAR* currentBuf, long currentBuf_size, char* modifiedBuf, long modifiedBuf_size) {

	//currentBuf��16bit�^�|�C���^�Ȃ̂�,2�Ŋ���B
	for (int i = 0; i < (currentBuf_size / 2); i++) {
		printf("%c", currentBuf[i]);
	}

	printf(" ===> ");

	for (int j = 0; j < modifiedBuf_size; j++) {
		printf("%c", modifiedBuf[j]);
	}

	printf("\n");
	printf("\n");
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
	BOOL SetUpSuccessFlag;

	const BYTE friendlyName[20]={};

	char DisplayDeviceName_COM[] = { 'H',NULL,'P',NULL,'K',NULL, '_',NULL, 'P',NULL, 'M',NULL,'T',NULL}; // <==�����Ƀf�o�C�X�}�l�[�W���[�ɕ\���������f�o�C�X���L�q�Bchar(8byte)�ɂ����NULL��}�����邱�ƁB
	

	char suffix_com1[] = { 0x20,NULL,'(',NULL,'C',NULL,'O',NULL,'M',NULL,'x',NULL,')',NULL ,NULL,NULL};  //0x20:�󔒕���
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
		

		if (pid_vid_buf == NULL) //dwSize��0�̂Ƃ�
			break;
		
		//bRet = SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, &dwRegType, (BYTE*)pid_vid_buf, dwSize, &dwSize);
		bRet = SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID, &dwRegType, (BYTE*)pid_vid_buf, dwSize, &dwSize);
		if (bRet == FALSE) {
			//printf("this device doesn't have SPDPR_HARDWAREID property. \n");
			//break; //�����Ŕ�����Ƃ��������Ȃ�B�Ō�܂ŉ��Ȃ��B�����Property���ݒ肳��Ă��Ȃ���false��Ԃ��Bdevice�͌��\���݂���B
		}

		
		//�Ƃ肠���������R�[�h
		//std::string s((char*) pid_vid_buf, 41);
		//std::string pid_vid = {};
		//for (int i = 0; i < 21; i++) {
		//	pid_vid += s[i * 2];
		//}


		//���ۂɎ󂯎����property�̕�����16bit������ŁA�o�C�g���Z�����char�^��2�{���݂���̂ŁA2�Ŋ���B
		pid_vid_char_buf = new char[dwSize/2];

		for (int i = 0; i < (dwSize/2); i++) {
			pid_vid_char_buf[i] = (BYTE)pid_vid_buf[i];
			//printf("%c is inside \n", (BYTE)pid_vid_buf[i]);
		}

		std::string pid_vid_array(pid_vid_char_buf, dwSize);
		//std::string pid_vid_array(pid_vid_char_buf);
		
		std::string pid_vid= {};

		for (int i = 0; i < (dwSize/2); i++) {
			pid_vid += pid_vid_array[i];
		}


		if (pid_vid.find(MATCH_PID_VID) != std::string::npos) {
			printf("Target Device is Found\n");
			printf("\n");
			printf("Device Name in COM port is changing ......\n");
			printf("\n");

			//���݂�friendlyName�̃T�C�Y�ƒl���m��
			SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, &dwRegType, NULL, 0, &currentFriendlyName_size);

			currentFriendlyName_buf = new TCHAR[currentFriendlyName_size];

			SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, &dwRegType, (BYTE*)currentFriendlyName_buf, dwSize, &dwSize);

			//(COM0) - (COM9)�̏ꍇ�@��납��S�����ځi�I�[�R�[�h�܂߂āj��M���ǂ����Ŕ��f
			if ((char)currentFriendlyName_buf[currentFriendlyName_size /2 - 4] == 'M') {
				//suffix_com1��x�����ۂ�port�ԍ��ɒu������
				suffix_com1[10] = (char)currentFriendlyName_buf[currentFriendlyName_size/2 - 3];
				
				//friendlyName�ɕ\�����閼�O����������B
				memcpy(friendlyName_buf, DisplayDeviceName_COM, sizeof(DisplayDeviceName_COM));
				memcpy(friendlyName_buf + sizeof(DisplayDeviceName_COM), suffix_com1, sizeof(suffix_com1));


				//�o�b�t�@�̒��g�m�F�p
				// 
				//printf("size of displaydevicename is %u \n", sizeof(DisplayDeviceName_COM));
				//printf("size of displaydevicename is %u \n", sizeof(suffix_com1));
				//for (int i = 0; i < sizeof(DisplayDeviceName_COM) + sizeof(suffix_com1); i++) {
				//	printf("%d byte code  => %c \n",i, *(friendlyName_buf + i));
				//}

				SetUpSuccessFlag=SetupDiSetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, (BYTE*)friendlyName_buf, friendlyName_buf_size);

				if (SetUpSuccessFlag == FALSE) {
					print_deviceNameChange(currentFriendlyName_buf, currentFriendlyName_size, friendlyName_buf, friendlyName_buf_size);
					printf("Property Setup is failed \nExecute with administrator priviledges. \n");
					
				}
				else {
					print_deviceNameChange(currentFriendlyName_buf, currentFriendlyName_size, friendlyName_buf, friendlyName_buf_size);
					printf("device FriendlyName property is modified \n ");
				}
			}
			//(COM10)-(COM99)�̂Ƃ�
			else if ((char)currentFriendlyName_buf[currentFriendlyName_size / 2 - 5] == 'M') {
				suffix_com2[10] = (char)currentFriendlyName_buf[currentFriendlyName_size / 2 - 4];
				suffix_com2[12] = (char)currentFriendlyName_buf[currentFriendlyName_size / 2 - 3];

				memcpy(friendlyName_buf, DisplayDeviceName_COM, sizeof(DisplayDeviceName_COM));
				memcpy(friendlyName_buf + sizeof(DisplayDeviceName_COM), suffix_com2, sizeof(suffix_com2));
				
				SetUpSuccessFlag=SetupDiSetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, (BYTE*)friendlyName_buf, friendlyName_buf_size);

				if (SetUpSuccessFlag == FALSE) {
					print_deviceNameChange(currentFriendlyName_buf, currentFriendlyName_size, friendlyName_buf, friendlyName_buf_size);
					printf("Property Setup is failed \nExecute with administrator priviledges. \n");
				}
				else {
					print_deviceNameChange(currentFriendlyName_buf, currentFriendlyName_size, friendlyName_buf, friendlyName_buf_size);
					printf("device FriendlyName property is modified \n ");
				}

			}
			//(COM100�j�ȍ~���邢��,�\������(COMxx)�̕\�L���Ȃ��Ƃ��B
			else {
				printf("device name end with (COMxx) is not found \n");
				return -1;
			}

		}
		delete[] pid_vid_buf;
		delete[] pid_vid_char_buf;

	}
	printf("\n");
	printf("the number of devinfo is %d \n", dwIndex);
	printf("\n");
	printf("done");

	Sleep(3);
}