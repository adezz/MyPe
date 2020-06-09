DWORD CopyFileBufferToImageBuffer(PVOID pFileBuffer,PVOID pImageBuffer);		
DWORD CopyImageBufferToNewBuffer(PVOID pImageBuffer,PVOID* pNewBuffer);
void FileBufferToAddShellcode(PVOID pFileBuffer);
void AddNewSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer);
void ExpandSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer);
void printfPE(PVOID pFileBuffer);
void PrintRelocation(PVOID pFileBuffer); //��ӡ�ض�λ��
DWORD FOA_TO_RVA(PVOID FileAddress, DWORD FOA,PDWORD pRVA);
DWORD RVA_TO_FOA(PVOID FileAddress, DWORD RVA, PDWORD pFOA);
void MyReadFile(PVOID *pFileBuffer,PDWORD BufferLenth);
void MyWriteFile(PVOID pMemBuffer,DWORD BufferLenth);
int GetBufferLength(PVOID Buffer);
void PrintfImportTable(PVOID pFileBuffer); //��ӡ�����
void MoveExportTable(PVOID pFileBuffer, PDWORD OldBufferSize,PVOID* pNewBuffer); //�ƶ�������
void MoveRelocationTable(PVOID pFileBuffer, PDWORD OldBufferSize,PVOID* pNewBuffer); //�ƶ��ض�λ��
void PrintBindImportTable(PVOID pFileBuffer); //��ӡ�󶨵����
void MoveAndInjectImportTable(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer); //�ƶ���������ҳ��Խ���ע��



//#define FILENAME "C:\\Documents and Settings\\Administrator\\����\\mydell.dll"
#define FILENAME "C:\\Documents and Settings\\Administrator\\����\\ipmsg.exe"
#define NEWFILENAME "C:\\Documents and Settings\\Administrator\\����\\ipmsg_new.exe"

int GetBufferLength(PVOID Buffer){
	int BufferLength;
	BufferLength = ftell(Buffer);
	return BufferLength;
}

//**************************************************************************
//ReadPEFile:���ļ���ȡ��������
//����˵��:			
//lpszFile �ļ�·��
//pFileBuffer ������ָ��
//����ֵ˵��:	
//��ȡʧ�ܷ���0  ���򷵻�ʵ�ʶ�ȡ�Ĵ�С
void MyReadFile(PVOID* pFileBuffer,PDWORD BufferLenth){
	FILE* File;
	File = fopen(FILENAME,"rb");

	if(File == NULL){
		printf("�ļ������ʧ��");
		return;
	}

	//��ȡ�ļ�
	fseek(File,0,SEEK_END);
	*BufferLenth = ftell(File);

	//���°�Fileָ��ָ���ļ��Ŀ�ͷ
	fseek(File,0,SEEK_SET);

	//�����¿ռ�
	*pFileBuffer = (PVOID)malloc(*BufferLenth);

	//�ڴ�����
	memset(*pFileBuffer,0,*BufferLenth);

	//��ȡ���ڴ滺����
	fread(*pFileBuffer,*BufferLenth,1,File);// һ�ζ���*bufferlenth���ֽڣ��ظ�1��

	//�ر��ļ����
	fclose(File);
}

//**************************************************************************								
//CopyFileBufferToImageBuffer:���ļ���FileBuffer���Ƶ�ImageBuffer								
//����˵����								
//pFileBuffer  FileBufferָ��								
//pImageBuffer ImageBufferָ��								
//����ֵ˵����								
//��ȡʧ�ܷ���0  ���򷵻ظ��ƵĴ�С								
						
DWORD CopyFileBufferToImageBuffer(PVOID pFileBuffer,PVOID* pImageBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_NT_HEADERS pImageNtHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	DWORD ImageBufferSize = 0;
	int i=0;
	
	// DOSͷ
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;

	// ��׼PE
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);

	// ��ѡPE
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + IMAGE_SIZEOF_FILE_HEADER);

	//�ڱ���
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	//��ȡImageBufffer���ڴ��С
	ImageBufferSize = pImageOptionalHeader->SizeOfImage;
	
	//ΪpImageBuffer�����ڴ�ռ�
	*pImageBuffer = (PVOID)malloc(ImageBufferSize);

	if (*pImageBuffer == NULL)
	{
		printf("malloc failed");
		return -1;
	}

	//����
	memset(*pImageBuffer, 0, ImageBufferSize);
	
	// ����ͷ+�ڱ�
	memcpy(*pImageBuffer, pFileBuffer, pImageOptionalHeader->SizeOfHeaders);


	//ѭ�������ڱ�
	for(i=0;i<pImageFileHeader->NumberOfSections;i++){
		memcpy(
			(PVOID)((DWORD)*pImageBuffer + pImageSectionHeaderGroup[i].VirtualAddress), // Ҫ������λ�� ImageBuffer�е�ÿ�������ݵ�ƫ��λ��
			(PVOID)((DWORD)pFileBuffer + pImageSectionHeaderGroup[i].PointerToRawData), // ��������λ���� Filebuffer�е�ÿ�������ݵ�ƫ��λ��
			pImageSectionHeaderGroup[i].SizeOfRawData // �������Ĵ�СΪ ÿ�������ݵ��ļ������С
		);
	}

	return 0;
}						


//**************************************************************************								
//CopyImageBufferToNewBuffer:��ImageBuffer�е����ݸ��Ƶ��µĻ�����								
//����˵����								
//pImageBuffer ImageBufferָ��								
//pNewBuffer NewBufferָ��								
//����ֵ˵����								
//��ȡʧ�ܷ���0  ���򷵻ظ��ƵĴ�С															
DWORD CopyImageBufferToNewBuffer(PVOID pImageBuffer,PVOID* pNewBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_NT_HEADERS pImageNtHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	DWORD NewBufferSize = 0;
	int i;
	int j;
	
	// DOSͷ
	pImageDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	
	//pImageNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew);
	
	// ��׼PE
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	
	// ��ѡPE
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	
	//�ڱ���
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	//��ȡNewBufferSize���ڴ��С
	NewBufferSize = pImageOptionalHeader->SizeOfHeaders;


	//��ѭ�����Ͻ����ݵĴ�С
	for(j=0;j<pImageFileHeader->NumberOfSections;j++){
		NewBufferSize += pImageSectionHeaderGroup[j].SizeOfRawData;
	}


	//ΪNewBufferSize�����ڴ�ռ�
	*pNewBuffer = (PVOID)malloc(NewBufferSize);
		
	if (*pNewBuffer == NULL)
	{
		printf("malloc failed");
		return -1;
	}

	//����
	memset(*pNewBuffer, 0, NewBufferSize);

	// ����ͷ+�ڱ�
	memcpy(*pNewBuffer, pImageBuffer, pImageOptionalHeader->SizeOfHeaders);
	
	//ѭ�������ڱ�
	for(i=0;i<pImageFileHeader->NumberOfSections;i++){
		memcpy(
			(PVOID)((DWORD)*pNewBuffer + pImageSectionHeaderGroup[j].PointerToRawData),
			(PVOID)((DWORD)pImageBuffer + pImageSectionHeaderGroup[j].VirtualAddress),
			pImageSectionHeaderGroup[j].SizeOfRawData
		);
	}

	return NewBufferSize;
}	


								
//FOA_TO_RVA:FOA ת�� RVA							
DWORD FOA_TO_RVA(PVOID FileAddress, DWORD FOA,PDWORD pRVA)
{
	int ret = 0;
	int i;
	
	PIMAGE_DOS_HEADER pDosHeader				= (PIMAGE_DOS_HEADER)(FileAddress);
	PIMAGE_FILE_HEADER pFileHeader				= (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader	= (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionGroup			= (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	
	//RVA���ļ�ͷ�� �� SectionAlignment ���� FileAlignment ʱRVA����FOA
	if (FOA < pOptionalHeader->SizeOfHeaders || pOptionalHeader->SectionAlignment == pOptionalHeader->FileAlignment)
	{
		*pRVA = FOA;
		return ret;
	}
	
	//ѭ���ж�FOA�ڽ�����
	for (i=0;i < pFileHeader->NumberOfSections; i++)
	{
		if (FOA >= pSectionGroup[i].PointerToRawData && FOA < pSectionGroup[i].PointerToRawData + pSectionGroup[i].SizeOfRawData)
		{
			*pRVA = FOA - pSectionGroup[i].PointerToRawData + pSectionGroup[i].VirtualAddress;
			return *pRVA;
		}
	}
	
	//û���ҵ���ַ
	ret = -4;
	printf("func FOA_TO_RVA() Error: %d ��ַת��ʧ�ܣ�\n", ret);
	return ret;
}


//���ܣ�RVA ת�� FOA
// RVA_TO_FOA(pFileBuffer,pOptionHeader->DataDirectory[5].VirtualAddress,&FOA);
DWORD RVA_TO_FOA(PVOID FileAddress, DWORD RVA, PDWORD pFOA)
{
	int ret = 0;
	int i=0;
	PIMAGE_DOS_HEADER pDosHeader				= (PIMAGE_DOS_HEADER)(FileAddress);
	PIMAGE_FILE_HEADER pFileHeader				= (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader	= (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionGroup			= (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	
	
	//RVA���ļ�ͷ�� �� SectionAlignment(�ڴ����) ���� FileAlignment(�ļ�����) ʱ RVA����FOA
	if (RVA < pOptionalHeader->SizeOfHeaders || pOptionalHeader->SectionAlignment == pOptionalHeader->FileAlignment)
	{
		// 37000
		*pFOA = RVA;
		return ret;
	}
	
	/*
		��һ����ָ����.VirtualAddress <= RVA <= ָ����.VirtualAddress + Misc.VirtualSize(��ǰ���ڴ�ʵ�ʴ�С)
		�ڶ�������ֵ = RVA - ָ����.VirtualAddress
		��������FOA = ָ����.PointerToRawData + ��ֵ
	*/

	//ѭ���ж�RVA�ڽ�����
	for (i=0;i<pFileHeader->NumberOfSections; i++)
	{
		// RVA > ��ǰ�����ڴ��е�ƫ�Ƶ�ַ ���� RVA < ��ǰ�ڵ��ڴ�ƫ�Ƶ�ַ+�ļ�ƫ�Ƶ�ַ
		if (RVA >= pSectionGroup[i].VirtualAddress && RVA < pSectionGroup[i].VirtualAddress + pSectionGroup[i].Misc.VirtualSize)
		{
			*pFOA =  RVA - pSectionGroup[i].VirtualAddress + pSectionGroup[i].PointerToRawData;
			return ret;
		}
	}
	
	//û���ҵ���ַ
	ret = -4;
	printf("func RVA_TO_FOA() Error: %d ��ַת��ʧ�ܣ�\n", ret);
	return ret;
}
								

//���ܣ������ļ� 
void MyWriteFile(PVOID pNewBuffer,size_t size){
	
	FILE* File;
	File = fopen(NEWFILENAME,"wb");
	if(File == NULL){
		printf("�ļ������ʧ��");
		return;
	}
	fwrite(pNewBuffer,size,1,File);
	printf("�ļ�����ɹ�!");
	fclose(File);
	free(pNewBuffer);


}


//���ܣ����shellcode
void FileBufferToAddShellcode(PVOID pFileBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;

	DWORD CodeAddress = 0; //Ҫ���shellcode�ĵ�ַ
	DWORD FuncAddress; //MESSAGEBOX��ַ
	HMODULE hModule; //����User32

	DWORD FOA = 0;
	DWORD RVA = 0;

	BYTE SHELLCODE[] = {
		0X6A,0X00,0X6A,0X00,0X6A,0X00,0X6A,0X00,
		0XE8,0X00,0X00,0X00,0X00,
		0XE9,0X00,0X00,0X00,0X00
	};

	DWORD E8_Next_Address;
	DWORD E9_Next_Address;
	DWORD EntryOfAddress;



	// DOSͷ
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	
	// ��׼PE
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	
	// ��ѡPE
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	
	//�ڱ���
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	//��ȡMessagebox�ĵ�ַ
	hModule = LoadLibrary("User32.dll");
	FuncAddress = (DWORD)GetProcAddress(hModule, "MessageBoxA");


	// CodeAddressΪSHELLCODE���ļ��е���ʼ��ַ
	CodeAddress = (DWORD)pImageSectionHeaderGroup[0].PointerToRawData + (DWORD)pImageSectionHeaderGroup[0].Misc.VirtualSize;


	// ����E8����ָ�����һ�е�ַ��RVA
	E8_Next_Address = CodeAddress + 13;
	
	FOA_TO_RVA(pFileBuffer,E8_Next_Address,&RVA);


	// X = ����Ҫ��ת�ĵ�ַ - E8����ָ�����һ�е�ַ
	E8_Next_Address = FuncAddress - (RVA + pImageOptionalHeader->ImageBase);

	//���E8�հ׵Ŀհײ���
	memcpy(&SHELLCODE[9], &E8_Next_Address, 4);


	// ����E9����ָ�����һ�е�ַ��RVA
	E9_Next_Address = CodeAddress + 18;

	FOA_TO_RVA(pFileBuffer,E9_Next_Address,&RVA);

	//�ٻ�ȡԭ����ڵ�ַ��VA
	EntryOfAddress = pImageOptionalHeader->AddressOfEntryPoint;


	// X = ����Ҫ��ת�ĵ�ַ - E9����ָ�����һ�е�ַ
	E9_Next_Address = EntryOfAddress - RVA;

	memcpy(&SHELLCODE[14],&E9_Next_Address,4);

	//�������shellcode������ٰ�shellcode�Ž�ȥ
	memcpy((PVOID)((DWORD)pFileBuffer+CodeAddress),SHELLCODE,0x20);
	
	//����滻OEP��λ�ã��滻Ϊshellcode�ĵ�ַ

	FOA_TO_RVA(pFileBuffer,CodeAddress,&RVA);

	pImageOptionalHeader->AddressOfEntryPoint = RVA;

}


//���ܣ�����½�
void AddNewSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;

	DWORD isOk;
	DWORD NewLength=0;
	PVOID LastSection = NULL;
	PVOID CodeSection = NULL;

	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	//�ж��Ƿ����������Ӧ�Ľڱ�
	isOk = (DWORD)pImageOptionalHeader->SizeOfHeaders - ((DWORD)pImageDosHeader->e_lfanew + IMAGE_SIZEOF_FILE_HEADER + pImageFileHeader->SizeOfOptionalHeader + 40*pImageFileHeader->NumberOfSections);
	if(isOk < 80){
		printf("�ռ�̫С �޷��������!");
		return;
	}

	//���ɶ�Ӧ���ڴ��С�Ŀռ�
	NewLength += *OldBufferSize + 0x1000;
	*pNewBuffer = (PVOID)malloc(NewLength);
	ZeroMemory(*pNewBuffer,NewLength);

	//����֮ǰ�ڴ�ռ� �� ��ǰ�����ɵ��ڴ�ռ�
	memcpy(*pNewBuffer,pFileBuffer,*OldBufferSize);

	//��ȡ�µĽṹ��
	pImageDosHeader = (PIMAGE_DOS_HEADER)(*pNewBuffer);
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	// pImageFileHeader->NumberOfSections�޸�
	pImageFileHeader->NumberOfSections = pImageFileHeader->NumberOfSections + 1;
	
	// pImageOptionalHeader->SizeOfImage�޸�
	pImageOptionalHeader->SizeOfImage = (DWORD)pImageOptionalHeader->SizeOfImage + 0x1000;
	
	// ���ƴ���εĽ����ݵ� ��ǰ���һ�������ݺ���
	CodeSection = (PVOID)(&pImageSectionHeaderGroup[0]);

	LastSection = (PVOID)(DWORD)(&pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1]);
	memcpy(LastSection,CodeSection,40);
	
	//�����������
 	NewSec = (PIMAGE_SECTION_HEADER)LastSection;
	strcpy(NewSec,".NewSec");
	NewSec->Misc.VirtualSize = 0x1000;
	NewSec->SizeOfRawData = 0x1000;
	NewSec->VirtualAddress = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].VirtualAddress + pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].SizeOfRawData;
	NewSec->PointerToRawData = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].PointerToRawData + pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].SizeOfRawData;

	//�޸Ĵ�С����
	*OldBufferSize = NewLength;
}

//���ܣ������
void ExpandSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer){

	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;

	DWORD TheBiggerOfSizeOfRawDataOrVirtualSize = 0;
	DWORD NewLength=0;
	
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	
	//���ɶ�Ӧ���ڴ��С�Ŀռ�
	NewLength += *OldBufferSize + 0x1000;
	*pNewBuffer = (PVOID)malloc(NewLength);
	ZeroMemory(*pNewBuffer,NewLength);
	
	//����֮ǰ�ڴ�ռ� �� ��ǰ�����ɵ��ڴ�ռ�
	memcpy(*pNewBuffer,pFileBuffer,*OldBufferSize);

	//�޸Ľ����ݵ�ƫ��
	if(pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].Misc.VirtualSize > pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].SizeOfRawData){
		TheBiggerOfSizeOfRawDataOrVirtualSize = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].Misc.VirtualSize;
	}else{
		TheBiggerOfSizeOfRawDataOrVirtualSize = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].SizeOfRawData;
	}

	pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].Misc.VirtualSize = TheBiggerOfSizeOfRawDataOrVirtualSize;
	pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].SizeOfRawData = TheBiggerOfSizeOfRawDataOrVirtualSize;

	// pImageOptionalHeader->SizeOfImage�޸�
	pImageOptionalHeader->SizeOfImage = (DWORD)pImageOptionalHeader->SizeOfImage + 0x1000;

	*OldBufferSize = NewLength;
}

// ���ܣ���ӡPE�ṹ
void printfPE(PVOID pFileBuffer){
    PIMAGE_DOS_HEADER pDosHeader = NULL;    
    PIMAGE_NT_HEADERS pNTHeader = NULL; 
    PIMAGE_FILE_HEADER pPEHeader = NULL;    
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;  
    PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
	PVOID AddressOfNamesTable = NULL;
	DWORD AddressOfNameOrdinalsNumber = NULL;
	PVOID FunctionOfAddress = NULL;
	char FunName[10] = {0};
	int i,j;

	DWORD FOA;
	char SectionName[9] = {0};

    pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER); 
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + IMAGE_SIZEOF_NT_OPTIONAL_HEADER);


    //�ж��Ƿ�����Ч��MZ��־��Ҳ����0x5A4D��ȡǰ�ĸ��ֽ�
    if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)    
    {   
        printf("������Ч��MZ��־\n");
        free(pFileBuffer);
        return ; 
    }   
	

    
    //��ӡDOSͷ    
    printf("********************DOSͷ********************\n\n"); 
    printf("_IMAGE_DOS_HEADERMZ->e_magic MZ��־��0x%x\n",pDosHeader->e_magic);
    printf("_IMAGE_DOS_HEADERMZ->e_lfanewָ��PE��־��0x%x\n",pDosHeader->e_lfanew);
    printf("\n");
	
    //�ж��Ƿ�����Ч��PE��־  
    if(*((PDWORD)((DWORD)pFileBuffer+pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)  
    {   
        printf("������Ч��PE��־\n");
        free(pFileBuffer);
        return ;
    }   
	
    
    //��ӡNTͷ 
    printf("********************NTͷ********************\n\n");  
    printf("_IMAGE_NT_HEADERS->Signature�ļ�PE��ʶ��0x%x\n",pNTHeader->Signature);
    printf("\n");
	

    printf("********************PEͷ********************\n\n");  
    printf("_IMAGE_FILE_HEADER->Machine֧�ֵ�CPU��0x%x\n",pPEHeader->Machine);
    printf("_IMAGE_FILE_HEADER->NumberOfSections�ڵ�������0x%x\n",pPEHeader->NumberOfSections);
    printf("_IMAGE_FILE_HEADER->SizeOfOptionalHeader��ѡPEͷ�Ĵ�С��0x%x\n",pPEHeader->SizeOfOptionalHeader);
    printf("\n");

	
    printf("********************OPTIOIN_PEͷ********************\n\n");  
    printf("_IMAGE_OPTIONAL_HEADER->Magic�ֱ�ϵͳλ��:0x%x\n",pOptionHeader->Magic);
    printf("_IMAGE_OPTIONAL_HEADER->AddressOfEntryPoint�������:0x%x\n",pOptionHeader->AddressOfEntryPoint);
    printf("_IMAGE_OPTIONAL_HEADER->ImageBase�ڴ澵���ַ:0x%x\n",pOptionHeader->ImageBase);
    printf("_IMAGE_OPTIONAL_HEADER->SectionAlignment�ڴ�����С:0x%x\n",pOptionHeader->SectionAlignment);
    printf("_IMAGE_OPTIONAL_HEADER->FileAlignment�ļ������С:0x%x\n",pOptionHeader->FileAlignment);
    printf("_IMAGE_OPTIONAL_HEADER->SizeOfImage�ڴ���PE�Ĵ�С(SectionAlignment������):0x%x\n",pOptionHeader->SizeOfImage);
    printf("_IMAGE_OPTIONAL_HEADER->SizeOfHeadersͷ+�ڱ����ļ�����Ĵ�С:0x%x\n",pOptionHeader->SizeOfImage);
    printf("_IMAGE_OPTIONAL_HEADER->NumberOfRvaAndSizesĿ¼����Ŀ:0x%x\n",pOptionHeader->NumberOfRvaAndSizes);
	
    printf("\n");
	
    //�ڱ�
    printf("********************�ڱ�********************\n\n");
    
    for(i=1;i<=pPEHeader->NumberOfSections;i++){
        char SectionName[9] ={0};
        strcpy(SectionName,(char *)pSectionHeader->Name);
        printf("_IMAGE_SECTION_HEADER->Name:%s\n",SectionName);
        printf("_IMAGE_SECTION_HEADER->VirtualSize:0x%x\n",pSectionHeader->Misc);
        printf("_IMAGE_SECTION_HEADER->VirtualAddress:0x%x\n",pSectionHeader->VirtualAddress);
        printf("_IMAGE_SECTION_HEADER->SizeOfRawData:0x%x\n",pSectionHeader->SizeOfRawData);
        printf("_IMAGE_SECTION_HEADER->PointerToRawData:0x%x\n",pSectionHeader->PointerToRawData);
        printf("_IMAGE_SECTION_HEADER->Characteristics:0x%x\n",pSectionHeader->Characteristics);
        pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pSectionHeader + IMAGE_SIZEOF_SECTION_HEADER);
        printf("\n");
    }


	RVA_TO_FOA(pFileBuffer,pOptionHeader->DataDirectory[0].VirtualAddress,&FOA);
	
	//������ĵ�ַ
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + FOA);
	
	//Ŀ¼���е�16�ű�Ľ���
	
	//�Ƚ�����һ�ű�������
	printf("********************������********************\n\n");
	printf("������������ַ:%x\n",pOptionHeader->DataDirectory[0].VirtualAddress);
	printf("������Ĵ�С:%x\n",pOptionHeader->DataDirectory[0].Size);
	printf("_IMAGE_EXPORT_DIRECTORY->Characteristics: 0x%x\n",pExportDirectory->Characteristics);
	printf("_IMAGE_EXPORT_DIRECTORY->TimeDateStampʱ���: 0x%x\n",pExportDirectory->TimeDateStamp);
	printf("_IMAGE_EXPORT_DIRECTORY->MajorVersion: 0x%x\n",pExportDirectory->MajorVersion);
	printf("_IMAGE_EXPORT_DIRECTORY->MinorVersion: 0x%x\n",pExportDirectory->MinorVersion);
	printf("_IMAGE_EXPORT_DIRECTORY->Nameָ��õ������ļ����ַ���: 0x%x\n",pExportDirectory->Name);
	printf("_IMAGE_EXPORT_DIRECTORY->Base����������ʼ���: 0x%x\n",pExportDirectory->Base);
	printf("_IMAGE_EXPORT_DIRECTORY->NumberOfFunctions���е��������ĸ���: 0x%x\n",pExportDirectory->NumberOfFunctions);
	printf("_IMAGE_EXPORT_DIRECTORY->NumberOfNames�Ժ������ֵ����ĺ�������: 0x%x\n",pExportDirectory->NumberOfNames);
	printf("_IMAGE_EXPORT_DIRECTORY->RVA_AddressOfFunctions����������ַ��: 0x%x\n",pExportDirectory->AddressOfFunctions);
	printf("_IMAGE_EXPORT_DIRECTORY->RAV_AddressOfNames�����������Ʊ�: 0x%x\n",pExportDirectory->AddressOfNames);
	printf("_IMAGE_EXPORT_DIRECTORY->RVA_AddressOfNameOrdinals����������ű�: 0x%x\n",pExportDirectory->AddressOfNameOrdinals);	

	printf("\n");




	//1�������������Ʊ���Ѱ�ҵ���������ַ��AddressOfNames��һ��ָ�������Ƶ�RVA��ַ����Ҫ��ת��Ϊ �ļ�ƫ�Ƶ�ַ
	RVA_TO_FOA(pFileBuffer,pExportDirectory->AddressOfNames,&FOA);

	//printf("pExportDirectory->AddressOfNames�����������Ʊ�: 0x%x\n",FOA);

	//2���ټ���pFileBuffer��ת��Ϊ�ļ���ַ���õ��������ƴ洢�ĵط����׵�ַ����ǰ���׵�ַ��RVA��Ҳ��Ҫ����RVA -> FOAת��
	AddressOfNamesTable = (PVOID)(*(PDWORD)((DWORD)pFileBuffer+(DWORD)FOA)); 
	RVA_TO_FOA(pFileBuffer,(DWORD)AddressOfNamesTable,&FOA); // // �����������Ʊ��к������Ƶ�FOA

	//AddressOfNamesTable = (PVOID)FOA;
	AddressOfNamesTable = (PVOID)((DWORD)pFileBuffer + (DWORD)FOA); // ����pFileBufferλ�þ͵��������ĺ������Ʊ�ĵ�ַ
	printf("\n");
	
	//3���õ��������Ʊ���ļ���ַ��ÿ������������ ռ�ĸ��ֽڣ�Ȼ����б����ж�	
	for(j=0;j<pExportDirectory->NumberOfNames;j++){
		//(PDWORD)((DWORD)AddressOfNamesTable + 4*j);
		//��ȡ��ǰ�������Ʊ��еĺ������ƣ�Ȼ��ѭ���ж�
		//printf("this is my test:%s \n", (PVOID)((DWORD)AddressOfNamesTable));
		strcpy(FunName,(PVOID)((DWORD)AddressOfNamesTable)); //����+1 �����һ���ֽ�Ϊ���ֽ� ��ô��Ϊ������
		if(0 == memcmp((PDWORD)((DWORD)AddressOfNamesTable),(PDWORD)FunName,strlen(FunName))){
			AddressOfNamesTable = (PVOID)((DWORD)AddressOfNamesTable + (DWORD)(strlen(AddressOfNamesTable)+1));			
			//4���ҵ���ű�AddressOfNameOrdinals�±�����Ӧ�ĵ�ֵ����ű���ÿ����Առ2�ֽ� word����
			RVA_TO_FOA(pFileBuffer,pExportDirectory->AddressOfNameOrdinals,&FOA);
			AddressOfNameOrdinalsNumber = *(PWORD)((DWORD)FOA + (DWORD)pFileBuffer + (DWORD)j*2);
			//5��ͨ����ű����±���õ�ֵȥ����������ַ��AddressOfFunctions��Ѱ�� ��ֵ�±��Ӧ��ֵ
			RVA_TO_FOA(pFileBuffer,pExportDirectory->AddressOfFunctions,&FOA);
			printf("�������: %d\t",AddressOfNameOrdinalsNumber);
			printf("��������Ϊ: %s\t",FunName);
			printf("����������ַ��ĵ�ַΪ��0x%.8x\n",*(PDWORD)(PVOID)((DWORD)FOA + (DWORD)pFileBuffer + AddressOfNameOrdinalsNumber*4));
		}
	}
	
	printf("\n");

	printf("********************�����********************\n\n");
	printf("�����������ַ:%x\n",pOptionHeader->DataDirectory[1].VirtualAddress);
	printf("�����Ĵ�С:%x\n",pOptionHeader->DataDirectory[1].Size);

	
	printf("\n");

	printf("********************��Դ��********************\n\n");
	printf("��Դ��������ַ:%x\n",pOptionHeader->DataDirectory[2].VirtualAddress);
	printf("��Դ��Ĵ�С:%x\n",pOptionHeader->DataDirectory[2].Size);
	printf("\n");

    //�ͷ��ڴ�  
    free(pFileBuffer);  
}

// ���ܣ���ӡ�ض�λ��
void PrintRelocation(PVOID pFileBuffer){

	PIMAGE_DOS_HEADER pDosHeader = NULL;    
    PIMAGE_NT_HEADERS pNTHeader = NULL; 
    PIMAGE_FILE_HEADER pPEHeader = NULL;    
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;  
    PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_BASE_RELOCATION pRelocationDirectory = NULL;
	DWORD FOA;
	DWORD RVA_Data;
	WORD reloData;
	int NumberOfRelocation = 0;
	PWORD Location = NULL;
	int i;

    pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER); 
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + IMAGE_SIZEOF_NT_OPTIONAL_HEADER);

	// _IMAGE_DATA_DIRECTORY�е�ָ���ض�λ��������ַת��ΪFOA��ַ
	//printf("%x\n",pOptionHeader->DataDirectory[5].VirtualAddress);

	printf("pRelocationDirectory_RVA:%x\n",pOptionHeader->DataDirectory[5].VirtualAddress);
	RVA_TO_FOA(pFileBuffer,pOptionHeader->DataDirectory[5].VirtualAddress,&FOA);
	printf("pRelocationDirectory_FOA:%x\n", FOA);

	pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer+(DWORD)FOA); //��λ��һ���ض�λ�� �ļ��еĵ�ַ

	while(pRelocationDirectory->SizeOfBlock && pRelocationDirectory->VirtualAddress){
		printf("VirtualAddress    :%08X\n", pRelocationDirectory->VirtualAddress);
		printf("SizeOfBlock       :%08X\n", pRelocationDirectory->SizeOfBlock);
		printf("================= BlockData Start ======================\n");

		
		NumberOfRelocation = (pRelocationDirectory->SizeOfBlock - 8)/2;// ÿ���ض�λ���е������������

		Location = (PWORD)((DWORD)pRelocationDirectory + 8); // ����8���ֽ�

		for(i=0;i<NumberOfRelocation;i++){
			if(Location[i] >> 12 != 0){ //�ж��Ƿ�����������
				// WORD���͵ı������н���
				reloData = (Location[i] & 0xFFF); //������������ ֻȡ4�ֽ� �����Ƶĺ�12λ
				RVA_Data = pRelocationDirectory->VirtualAddress + reloData; //�����RVA�ĵ�ַ
				RVA_TO_FOA(pFileBuffer,RVA_Data,&FOA);
				printf("��[%04X]��  �����������Ϊ:[%04X]  ��������Ϊ:[%X]  RVA�ĵ�ַΪ:[%08X]  �ض�λ������:[%08X]\n",i+1,reloData,(Location[i] >> 12),RVA_Data,*(PDWORD)((DWORD)pFileBuffer+FOA));
			}
		}
		pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocationDirectory + (DWORD)pRelocationDirectory->SizeOfBlock); //�����forѭ�����֮����ת���¸��ض�λ�� �������ϵĲ���
	}
}

// ���ܣ��ƶ������
void MoveExportTable(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;

	PIMAGE_EXPORT_DIRECTORY EXPORT_TABLE = NULL;
	PIMAGE_EXPORT_DIRECTORY EXPORT_TABLE_NewBuffer = NULL;

	PDWORD AddressFunctionName;
	DWORD RVA = 0;
	DWORD FOA = 0;
	PDWORD pTempAddress;

	int FunNameLen = 0;

	char FunName[10] = {0};

	int i = 0;
	int j = 0;
	int all_num = 0;


	DWORD isOk;
	DWORD NewLength=0;
	PVOID LastSection = NULL;

	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	/*
	��һ����������
	*/

	//�ж��Ƿ����������Ӧ�Ľڱ�
	isOk = (DWORD)pImageOptionalHeader->SizeOfHeaders - ((DWORD)pImageDosHeader->e_lfanew + IMAGE_SIZEOF_FILE_HEADER + pImageFileHeader->SizeOfOptionalHeader + 40*pImageFileHeader->NumberOfSections);
	if(isOk < 80){
		printf("�ռ�̫С �޷��������!");
		return;
	}

	//�����Ӧ���ڴ��С�Ŀռ�
	NewLength += *OldBufferSize + 0x1000;
	*pNewBuffer = (PVOID)malloc(NewLength);
	ZeroMemory(*pNewBuffer,NewLength);

	//����֮ǰ�ڴ�ռ� �� ��ǰ�����ɵ��ڴ�ռ�
	memcpy(*pNewBuffer,pFileBuffer,*OldBufferSize);

	//��ȡ�µĿռ��е�PE�ṹ��
	pImageDosHeader = (PIMAGE_DOS_HEADER)(*pNewBuffer);
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	// pImageFileHeader->NumberOfSections�޸�
	pImageFileHeader->NumberOfSections = pImageFileHeader->NumberOfSections + 1;
	
	// pImageOptionalHeader->SizeOfImage�޸�
	pImageOptionalHeader->SizeOfImage = (DWORD)pImageOptionalHeader->SizeOfImage + 0x1000;

	// �õ������ڵĵ�ַ,LastSection
	LastSection = (PVOID)(DWORD)(&pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1]);
	RVA_TO_FOA(*pNewBuffer,pImageOptionalHeader->DataDirectory[0].VirtualAddress,&FOA);
	EXPORT_TABLE = (PIMAGE_EXPORT_DIRECTORY)((DWORD)*pNewBuffer + (DWORD)FOA);

	/*
	�ڶ���������AddressOfFunctions
	���ȣ�4*NumberOfFunctions		
	*/
	printf("AddressOfFunctions����: %d ÿ��ռ4�ֽ�\n", EXPORT_TABLE->NumberOfFunctions);
	RVA_TO_FOA(*pNewBuffer,EXPORT_TABLE->AddressOfFunctions,&FOA);
	memcpy(LastSection,(PVOID)((DWORD)*pNewBuffer + FOA),((DWORD)EXPORT_TABLE->NumberOfFunctions)*4);

	/*
	������������AddressOfNameOrdinals				
	���ȣ�NumberOfNames*2			
	*/
	printf("AddressOfNameOrdinals����: %d ÿ��ռ2�ֽ�\n", EXPORT_TABLE->NumberOfNames);

	RVA_TO_FOA(*pNewBuffer, EXPORT_TABLE->AddressOfNameOrdinals,&FOA);
	memcpy((PVOID)((DWORD)LastSection + ((DWORD)EXPORT_TABLE->NumberOfFunctions)*4),(PVOID)((DWORD)*pNewBuffer + FOA),((DWORD)EXPORT_TABLE->NumberOfNames)*2);

	/*
	���Ĳ�������AddressOfNames
	���ȣ�NumberOfNames*4		
	*/
	printf("AddressOfNames����: %d ÿ��ռ4�ֽ�\n", EXPORT_TABLE->NumberOfNames);
	RVA_TO_FOA(*pNewBuffer, EXPORT_TABLE->AddressOfNames,&FOA);
	memcpy((PVOID)((DWORD)LastSection + ((DWORD)EXPORT_TABLE->NumberOfFunctions)*4 + ((DWORD)EXPORT_TABLE->NumberOfNames)*2),(PVOID)((DWORD)*pNewBuffer + FOA),(DWORD)EXPORT_TABLE->NumberOfNames*4);

	/*
	���岽���������еĺ�����
	���Ȳ�ȷ��������ʱֱ���޸�AddressOfNames				
	*/

	for(j=0;j<EXPORT_TABLE->NumberOfNames;j++){
		//��ú������Ʊ��RVA������ת��ΪFOA
		RVA_TO_FOA(*pNewBuffer, EXPORT_TABLE->AddressOfNames,&FOA);

		// ÿ��������RVAת��ΪFOA
		RVA_TO_FOA(*pNewBuffer, FOA, &FOA);
		
		//��ȡ��ǰ�������Ƶ�ƫ�Ƶ�ַ
		AddressFunctionName = (PDWORD)(*(PDWORD)((DWORD)*pNewBuffer + (DWORD)FOA + (DWORD)all_num));
		//printf("%x",AddressFunctionName);

		//����ǰ�ĺ������Ƶ�ƫ�Ƶ�ַ���� pNewBuffer �õ���Ӧ���ڴ��ַ ��ͨ��strcpy����ȡ��ǰ��ַ����ĺ�������
		strcpy(FunName,(PVOID)((DWORD)*pNewBuffer + (DWORD)AddressFunctionName));
		//printf("%s",FunName);

		//�õ���ǰ�������Ƶĳ���
		FunNameLen = strlen(FunName) + 1; //����β��Ҫ+1��ԭ��\0 ���ֽ�
		
		//�õ������ĳ��Ⱥ�����֮����Ҫ���и���
		memcpy(
			(PVOID)((DWORD)LastSection + ((DWORD)EXPORT_TABLE->NumberOfFunctions)*4 + ((DWORD)EXPORT_TABLE->NumberOfNames)*2 + ((DWORD)EXPORT_TABLE->NumberOfNames)*4 + (DWORD)all_num) //���ﵽʱ���ѭ��������ƫ�Ƹ���
			,(PVOID)((DWORD)*pNewBuffer + (DWORD)AddressFunctionName)
			,FunNameLen);
		
		//��������Ҫ�����޸�

		//���̣�ÿ�θ����� ����Ҫ�޸���֮ǰ�ո���AddressOfNames�еĶ�Ӧ�ĵ�ַ ���������ֵ ����Ϊ��ǰ���Ƶĺ�����ַ
		
		//ͨ��all_num������ƫ�� �Ӷ���õ�ǰ�ĵ�ַ��ָ���j�������ĵ�ַ
		pTempAddress = (PDWORD)((DWORD)LastSection + ((DWORD)EXPORT_TABLE->NumberOfFunctions)*4 + ((DWORD)EXPORT_TABLE->NumberOfNames)*2 + (DWORD)all_num);

		//�����õĵ�ַ��VA ����Ҫ��ȥpNewBuffer���FOA Ȼ����ת��ΪRVA ���洢���¸��Ƶĺ������Ʊ��Ӧ�ĵ�ַ����
		FOA_TO_RVA(*pNewBuffer
			,((DWORD)LastSection + ((DWORD)EXPORT_TABLE->NumberOfFunctions)*4 + ((DWORD)EXPORT_TABLE->NumberOfNames)*2 + ((DWORD)EXPORT_TABLE->NumberOfNames)*4 + (DWORD)all_num) -(DWORD)*pNewBuffer
			,&RVA);
		
		//�޸ĵ�ǰpTempAddressָ��ĵ�ַ�е�ֵ���޸�Ϊ֮ǰÿ���������Ƶĵĵ�ַ
		*pTempAddress = RVA;
		
		// all_num�������渴�ƺ������Ƶ�ʱ��һ�����˶��ٸ��ֽ�
		all_num += FunNameLen;
  }

	/*
	������������IMAGE_EXPORT_DIRECTORY�ṹ				
	*/
	memcpy((DWORD)LastSection + ((DWORD)EXPORT_TABLE->NumberOfFunctions)*4 + ((DWORD)EXPORT_TABLE->NumberOfNames)*2 + ((DWORD)EXPORT_TABLE->NumberOfNames)*4 + (DWORD)all_num
		,EXPORT_TABLE
		,40
		);

	
	/*
	���߲����޸�IMAGE_EXPORT_DIRECTORY�ṹ�е�

	AddressOfFunctions					
	AddressOfNameOrdinals										
	AddressOfNames					
	*/
	EXPORT_TABLE_NewBuffer = (PIMAGE_EXPORT_DIRECTORY)((DWORD)LastSection + ((DWORD)EXPORT_TABLE->NumberOfFunctions)*4 
		+ ((DWORD)EXPORT_TABLE->NumberOfNames)*2 
		+ ((DWORD)EXPORT_TABLE->NumberOfNames)*4 
		+ (DWORD)all_num);

	
	//���µĻ������е��������д洢�ĵ�ַ�������޸�Ϊ�����ƶ��õ�λ��
	EXPORT_TABLE_NewBuffer->AddressOfFunctions = (DWORD)LastSection;
	EXPORT_TABLE_NewBuffer->AddressOfNameOrdinals = ((DWORD)LastSection + ((DWORD)EXPORT_TABLE->NumberOfFunctions)*4);
	EXPORT_TABLE_NewBuffer->AddressOfNames = ((DWORD)LastSection + ((DWORD)EXPORT_TABLE->NumberOfFunctions)*4 + ((DWORD)EXPORT_TABLE->NumberOfNames)*2);
	
	/*
	�ڰ˲����޸�Ŀ¼���е�ֵ��ָ���µ�IMAGE_EXPORT_DIRECTORY						
	*/
	
	FOA_TO_RVA(*pNewBuffer,(DWORD)EXPORT_TABLE_NewBuffer - (DWORD)*pNewBuffer,&RVA);
	pImageOptionalHeader->DataDirectory[0].VirtualAddress = RVA;

	/*
	�ھŲ�����pNewBuffer�������ĵ�ַ����Ϊ�µ��ļ�
	*/
	MyWriteFile(*pNewBuffer, NewLength);
		
}

void MoveRelocationTable(PVOID pFileBuffer, PDWORD OldBufferSize,PVOID* pNewBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;
	PIMAGE_BASE_RELOCATION pRelocationDirectory = NULL;
	
	DWORD isOk;
	DWORD NewLength=0;
	PVOID LastSection = NULL;
	PVOID CodeSection = NULL;
	PVOID AddressOfSectionTable = NULL;
	PVOID pTemp;

	DWORD AllSizeOfBlock = 0;
	DWORD RVA = 0;
	DWORD FOA = 0;

	int NumberOfRelocation=0;
	PWORD Location = NULL;
	int i = 0;
	DWORD RVA_Data;
	WORD reloData;



	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	//�ж��Ƿ����������Ӧ�Ľڱ�
	isOk = (DWORD)pImageOptionalHeader->SizeOfHeaders - ((DWORD)pImageDosHeader->e_lfanew + IMAGE_SIZEOF_FILE_HEADER + pImageFileHeader->SizeOfOptionalHeader + 40*pImageFileHeader->NumberOfSections);
	if(isOk < 80){
		printf("�ռ�̫С �޷��������!");
		return;
	}
	
	//���ɶ�Ӧ���ڴ��С�Ŀռ�
	NewLength += *OldBufferSize + 0x1000;
	*pNewBuffer = (PVOID)malloc(NewLength);
	ZeroMemory(*pNewBuffer,NewLength);
	
	//����֮ǰ�ڴ�ռ� �� ��ǰ�����ɵ��ڴ�ռ�
	memcpy(*pNewBuffer,pFileBuffer,*OldBufferSize);
	
	//��ȡ�µĽṹ��
	pImageDosHeader = (PIMAGE_DOS_HEADER)(*pNewBuffer);
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	// pImageFileHeader->NumberOfSections�޸�
	pImageFileHeader->NumberOfSections = pImageFileHeader->NumberOfSections + 1;
	
	// pImageOptionalHeader->SizeOfImage�޸�
	pImageOptionalHeader->SizeOfImage = (DWORD)pImageOptionalHeader->SizeOfImage + 0x1000;
	
	// ���ƴ���εĽ����ݵ� ��ǰ���һ�������ݺ���
	CodeSection = (PVOID)(&pImageSectionHeaderGroup[0]);
	
	
	LastSection = (PVOID)(DWORD)(&pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1]);
	memcpy(LastSection,CodeSection,40);
	
	//�����������
	NewSec = (PIMAGE_SECTION_HEADER)LastSection;
	strcpy(NewSec,".NewSec");
	NewSec->Misc.VirtualSize = 0x1000;
	NewSec->SizeOfRawData = 0x1000;
	NewSec->VirtualAddress = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].VirtualAddress + pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].SizeOfRawData;
	NewSec->PointerToRawData = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].PointerToRawData + pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].SizeOfRawData;
	*OldBufferSize = NewLength;

	//�������������Ѿ������
	AddressOfSectionTable = (PVOID)((DWORD)*pNewBuffer + (DWORD)NewSec->PointerToRawData);
	
	//printf("%x",AddressOfSectionTable);

	//�ض�λ���FOA
	RVA_TO_FOA(*pNewBuffer,pImageOptionalHeader->DataDirectory[5].VirtualAddress,&FOA);
	
	//��ȡ�ṹ
	pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)*pNewBuffer + FOA);

	pTemp = pRelocationDirectory;
	
	//printf("%x",pRelocationDirectory->VirtualAddress);
	
	//��ȡ�ض�λ���С
	while(pRelocationDirectory->SizeOfBlock && pRelocationDirectory->VirtualAddress){
		AllSizeOfBlock = pRelocationDirectory->SizeOfBlock;
		pRelocationDirectory = ((DWORD)pRelocationDirectory + (DWORD)pRelocationDirectory->SizeOfBlock);
	}
	
	//�����ض�λ�������Ľ�������
	memcpy(AddressOfSectionTable,pTemp,AllSizeOfBlock);

	//��PE��ѡͷ�е��ض�λ�ĵ�ַָ�����������ݵ���ʼ��ַ
	pImageOptionalHeader->DataDirectory[5].VirtualAddress = (DWORD)AddressOfSectionTable;

	
	//�޸�DLL��ImageBase	
	//pImageOptionalHeader->ImageBase += 1000;

	//=============================================================
	//=============================================================
	//=============================================================
	//=============================================================

		
	printf("pRelocationDirectory_RVA:%x\n",pImageOptionalHeader->DataDirectory[5].VirtualAddress);
	RVA_TO_FOA(pFileBuffer,pImageOptionalHeader->DataDirectory[5].VirtualAddress,&FOA);
	printf("pRelocationDirectory_FOA:%x\n", FOA);
	
	pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer+(DWORD)FOA); //��λ��һ���ض�λ�� �ļ��еĵ�ַ
	
	while(pRelocationDirectory->SizeOfBlock && pRelocationDirectory->VirtualAddress){
		printf("VirtualAddress    :%08X\n", pRelocationDirectory->VirtualAddress);
		printf("SizeOfBlock       :%08X\n", pRelocationDirectory->SizeOfBlock);
		printf("================= BlockData Start ======================\n");
		
		
		NumberOfRelocation = (pRelocationDirectory->SizeOfBlock - 8)/2;// ÿ���ض�λ���е������������
		
		Location = (PWORD)((DWORD)pRelocationDirectory + 8); // ����8���ֽ�
		
		for(i=0;i<NumberOfRelocation;i++){
			if(Location[i] >> 12 != 0){ //�ж��Ƿ�����������
				// WORD���͵ı������н���
				reloData = (Location[i] & 0xFFF); //������������ ֻȡ4�ֽ� �����Ƶĺ�12λ
				RVA_Data = pRelocationDirectory->VirtualAddress + reloData; //�����RVA�ĵ�ַ
				RVA_TO_FOA(pFileBuffer,RVA_Data,&FOA);
				printf("��[%04X]��  �����������Ϊ:[%04X]  ��������Ϊ:[%X]  RVA�ĵ�ַΪ:[%08X]  �ض�λ������:[%08X]\n"
					,i+1
					,reloData
					,(Location[i] >> 12)
					,RVA_Data
					,*(PDWORD)((DWORD)pFileBuffer+(DWORD)FOA));

				//������������ �����޸��ض�λ�������Imagebase����������1000����ôҪ�޸��ĵ�ַ����Ҫ����1000
				*(PDWORD)((DWORD)pFileBuffer+(DWORD)FOA) = *(PDWORD)((DWORD)pFileBuffer+(DWORD)FOA) + 1000;				
			}
		}
		pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocationDirectory + (DWORD)pRelocationDirectory->SizeOfBlock); //�����forѭ�����֮����ת���¸��ض�λ�� �������ϵĲ���
	}

	//=============================================================
	//=============================================================
	//=============================================================
	//=============================================================
	//�����ļ�
	MyWriteFile(*pNewBuffer,NewLength);
}

void PrintfImportTable(PVOID pFileBuffer){
    PIMAGE_DOS_HEADER pDosHeader = NULL;    
    PIMAGE_NT_HEADERS pNTHeader = NULL; 
    PIMAGE_FILE_HEADER pPEHeader = NULL;    
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;  
    PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pIMPORT_DESCRIPTOR;
	PIMAGE_IMPORT_BY_NAME pImage_IMPORT_BY_NAME;


	char ImportTableDllName[10] = {0};
	char FunctionName[20] = {0};

	PDWORD OriginalFirstThunk_INT = NULL;
	PDWORD FirstThunk_IAT = NULL;

	DWORD RVA = 0;
	DWORD FOA = 0;
	DWORD Original = 0;
	
    pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER); 
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + IMAGE_SIZEOF_NT_OPTIONAL_HEADER);

	//��ȡ������λ��
	RVA_TO_FOA(pFileBuffer,pOptionHeader->DataDirectory[1].VirtualAddress,&FOA);


	//ÿ�������������Ϣռ20���ֽ�
	pIMPORT_DESCRIPTOR = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + (DWORD)FOA);
	
	//������Խ���while����������while���ж�����Ϊ pIMPORT_DESCRIPTOR����

	printf("=========================================");
	
	while(pIMPORT_DESCRIPTOR->FirstThunk && pIMPORT_DESCRIPTOR->OriginalFirstThunk){
		//�����ӡ����INT��
		//��ȡ��ǰ�����DLL������
		strcpy(ImportTableDllName,(PVOID)((DWORD)pFileBuffer + (DWORD)pIMPORT_DESCRIPTOR->Name));
		
		printf("��ǰ��ӡ�ĵ������DLLΪ: %s \n", ImportTableDllName);
		printf("\n");

		//printf("TimeDateStamp: %x\n",pIMPORT_DESCRIPTOR->TimeDateStamp);
		

		printf("INT���ӡ\n");
		//OriginalFirstThunkת��FOA
		RVA_TO_FOA(pFileBuffer,pIMPORT_DESCRIPTOR->OriginalFirstThunk,&FOA);
		
		OriginalFirstThunk_INT = (PDWORD)((DWORD)pFileBuffer + (DWORD)FOA);
		
		//printf("%x",*OriginalFirstThunk_INT);
		printf("\n");
		while(*OriginalFirstThunk_INT){
			//printf("%x\n ",*OriginalFirstThunk_INT);
			if((*OriginalFirstThunk_INT) & 0X80000000){
				//��λΪ1 �� ��ȥ���λ��ֵ���Ǻ����ĵ������
				Original = *OriginalFirstThunk_INT & 0xFFF;	//ȥ����߱�־λ��
				printf("����ŵ���: %08Xh -- %08dd\n", Original, Original);	//16���� -- 10 ����
			}else{
				//��λ��Ϊ1 ��ָ��IMAGE_IMPORT_BY_NAME
				RVA_TO_FOA(pFileBuffer,*OriginalFirstThunk_INT,&FOA);
				pImage_IMPORT_BY_NAME = (PIMAGE_IMPORT_BY_NAME)FOA;
				strcpy(FunctionName,(PVOID)((DWORD)pFileBuffer + (DWORD)&(pImage_IMPORT_BY_NAME->Name)));
				printf("������������ ������Ϊ: %s \n",FunctionName);
			}
			OriginalFirstThunk_INT++;
		}

		printf("\n");
		

		//�������ϲ������д�ӡ����
		//�����ӡ����iat��

		printf("IAT���ӡ\n");
		//FirstThunkת��FOA
		RVA_TO_FOA(pFileBuffer,pIMPORT_DESCRIPTOR->FirstThunk,&FOA);

		FirstThunk_IAT = (PDWORD)((DWORD)pFileBuffer + (DWORD)FOA);
		
		//printf("%x",*OriginalFirstThunk_INT);
		printf("\n");
		while(*FirstThunk_IAT){
			printf("%x\n ",*FirstThunk_IAT);

			
			if((*FirstThunk_IAT) & 0X80000000){
				//��λΪ1 �� ��ȥ���λ��ֵ���Ǻ����ĵ������
				Original = *FirstThunk_IAT & 0xFFF;	//ȥ����߱�־λ��
				printf("����ŵ���: %08Xh -- %08dd\n", Original, Original);	//16���� -- 10 ����
			}else{
				//��λ��Ϊ1 ��ָ��IMAGE_IMPORT_BY_NAME
				RVA_TO_FOA(pFileBuffer,*FirstThunk_IAT,&FOA);
				pImage_IMPORT_BY_NAME = (PIMAGE_IMPORT_BY_NAME)FOA;
				strcpy(FunctionName,(PVOID)((DWORD)pFileBuffer + (DWORD)&(pImage_IMPORT_BY_NAME->Name)));
				printf("������������ ������Ϊ: %s \n",FunctionName);
			}
			
			FirstThunk_IAT++;
		}
		
		printf("=========================================");
		printf("\n");
		
		pIMPORT_DESCRIPTOR++;		
	}
}

void PrintBindImportTable(PVOID pFileBuffer){
	PIMAGE_DOS_HEADER pDosHeader = NULL;    
    PIMAGE_NT_HEADERS pNTHeader = NULL; 
    PIMAGE_FILE_HEADER pPEHeader = NULL;    
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;  
    PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_BOUND_IMPORT_DESCRIPTOR pIMAGE_BOUND_IMPORT_DESCRIPTOR = NULL;
	PIMAGE_BOUND_FORWARDER_REF pIMAGE_BOUND_FORWARDER_REF = NULL;

	char ModuleName[20] = {0};
	DWORD BOUNG_IMPORT_DESCRIPTOR_TEMP = NULL;
	int i = 0;
	DWORD RVA = 0;
	DWORD FOA = 0;
	
    pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER); 
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + IMAGE_SIZEOF_NT_OPTIONAL_HEADER);
	
	RVA_TO_FOA(pFileBuffer, pOptionHeader->DataDirectory[11].VirtualAddress,&FOA);

	//�����һ��DESCRIPTOR�ĵ�ַ �����OffsetModuleName������ʹ��
	BOUNG_IMPORT_DESCRIPTOR_TEMP = (DWORD)pFileBuffer+(DWORD)FOA;

	
	//��ʼ���д�ӡ����
	pIMAGE_BOUND_IMPORT_DESCRIPTOR = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer+(DWORD)FOA);
	
	while (*(PDWORD)pIMAGE_BOUND_IMPORT_DESCRIPTOR)
	{
		printf("\n");
		strcpy(ModuleName, (PVOID)((DWORD)BOUNG_IMPORT_DESCRIPTOR_TEMP + (DWORD)pIMAGE_BOUND_IMPORT_DESCRIPTOR->OffsetModuleName));
		printf("ģ������: %s \n",ModuleName);
		printf("ģ���ʱ���Ϊ: %x \n", pIMAGE_BOUND_IMPORT_DESCRIPTOR->TimeDateStamp);
		printf("��ǰģ�����õ�dll������Ϊ: %x\n",pIMAGE_BOUND_IMPORT_DESCRIPTOR->NumberOfModuleForwarderRefs);

		for(i=0;i<pIMAGE_BOUND_IMPORT_DESCRIPTOR->NumberOfModuleForwarderRefs;i++){
			pIMAGE_BOUND_IMPORT_DESCRIPTOR++;
			pIMAGE_BOUND_FORWARDER_REF = (PIMAGE_BOUND_FORWARDER_REF)pIMAGE_BOUND_IMPORT_DESCRIPTOR;
			strcpy(ModuleName, (PVOID)((DWORD)BOUNG_IMPORT_DESCRIPTOR_TEMP + (DWORD)pIMAGE_BOUND_FORWARDER_REF->OffsetModuleName));
			printf("\t���õ�ģ������: %s \n",ModuleName);
			printf("\t���õ�ģ���ʱ���: %x\n", pIMAGE_BOUND_FORWARDER_REF->TimeDateStamp);
		}
	
		pIMAGE_BOUND_IMPORT_DESCRIPTOR++;
	}
}

void MoveAndInjectImportTable(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer){

	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pIMPORT_DESCRIPTOR = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pIMPORT_DESCRIPTOR_Temp = NULL;
	PIMAGE_IMPORT_BY_NAME IMPORT_BY_NAME = NULL;

	
	DWORD RVA = 0;
	DWORD FOA = 0;
	DWORD isOk;
	DWORD NewLength=0;
	PVOID LastSection = NULL;
	PVOID CodeSection = NULL;
	PVOID SectionOfNew= NULL;
	PVOID SectionOfNewTemp = NULL;

	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	//�ж��Ƿ����������Ӧ�Ľڱ�
	isOk = (DWORD)pImageOptionalHeader->SizeOfHeaders - ((DWORD)pImageDosHeader->e_lfanew + IMAGE_SIZEOF_FILE_HEADER + pImageFileHeader->SizeOfOptionalHeader + 40*pImageFileHeader->NumberOfSections);
	if(isOk < 80){
		printf("�ռ�̫С �޷��������!");
		return;
	}
	
	//���ɶ�Ӧ���ڴ��С�Ŀռ�
	NewLength += *OldBufferSize + 0x1000;
	*pNewBuffer = (PVOID)malloc(NewLength);
	ZeroMemory(*pNewBuffer,NewLength);
	
	//����֮ǰ�ڴ�ռ� �� ��ǰ�����ɵ��ڴ�ռ�
	memcpy(*pNewBuffer,pFileBuffer,*OldBufferSize);
	
	//��ȡ�µĽṹ��
	pImageDosHeader = (PIMAGE_DOS_HEADER)(*pNewBuffer);
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	// pImageFileHeader->NumberOfSections�޸�
	pImageFileHeader->NumberOfSections = pImageFileHeader->NumberOfSections + 1;
	
	// pImageOptionalHeader->SizeOfImage�޸�
	pImageOptionalHeader->SizeOfImage = (DWORD)pImageOptionalHeader->SizeOfImage + 0x1000;
	
	// ���ƴ���εĽ����ݵ� ��ǰ���һ�������ݺ���
	CodeSection = (PVOID)(&pImageSectionHeaderGroup[0]);
	
	//�����ڵ�λ��
	LastSection = (PVOID)(DWORD)(&pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1]);
	memcpy(LastSection,CodeSection,40);
	
	//�����������
	NewSec = (PIMAGE_SECTION_HEADER)LastSection;
	strcpy(NewSec,".NewSec");
	NewSec->Misc.VirtualSize = 0x1000;
	NewSec->SizeOfRawData = 0x1000;
	NewSec->VirtualAddress = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].VirtualAddress + pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].SizeOfRawData;
	NewSec->PointerToRawData = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].PointerToRawData + pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].SizeOfRawData;
	
	//�޸Ĵ�С����
	*OldBufferSize = NewLength;

	//����õ��½�λ�õ�ָ��
	SectionOfNew = (PVOID)((DWORD)*pNewBuffer + (DWORD)NewSec->PointerToRawData);

	//�Ȼ�ȡ�����ĵ�ַ
	RVA_TO_FOA(*pNewBuffer,pImageOptionalHeader->DataDirectory[1].VirtualAddress,&FOA);
	pIMPORT_DESCRIPTOR = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)*pNewBuffer + (DWORD)FOA);
	//printf("start:%x\n", pIMPORT_DESCRIPTOR);

	/*
	��������			
	��ԭ�����ȫ��Copy���հ���			
	*/

	SectionOfNewTemp = SectionOfNew;

	while (pIMPORT_DESCRIPTOR->OriginalFirstThunk && pIMPORT_DESCRIPTOR->FirstThunk)
	{
		//printf("%x\n", (DWORD)SectionOfNewTemp - (DWORD)*pNewBuffer);
		memcpy(SectionOfNewTemp,pIMPORT_DESCRIPTOR,20);
		pIMPORT_DESCRIPTOR++;
		SectionOfNewTemp = (PVOID)((DWORD)SectionOfNewTemp + 20);
	}
	
	//���渴���굼���֮��ĵ�ַ
	pIMPORT_DESCRIPTOR_Temp = SectionOfNewTemp;
	printf("��ʼ����Լ��ĵ����ĵ�ַ:%x\n",(DWORD)SectionOfNewTemp-(DWORD)*pNewBuffer);


	/*
	���Ĳ���				
	���µĵ������棬׷��һ�������.
	  typedef struct _IMAGE_IMPORT_DESCRIPTOR {							
	  union {							
	  DWORD   Characteristics;           							
	  DWORD   OriginalFirstThunk;         							
	  };							
	  DWORD   TimeDateStamp;               							
	  DWORD   ForwarderChain;              							
	  DWORD   Name;							
	  DWORD   FirstThunk;                 							
	  } IMAGE_IMPORT_DESCRIPTOR;							
	  typedef IMAGE_IMPORT_DESCRIPTOR UNALIGNED *PIMAGE_IMPORT_DESCRIPTOR;							
	*/

	pIMPORT_DESCRIPTOR->TimeDateStamp = 0;
	
	pIMPORT_DESCRIPTOR->ForwarderChain = -1;

	FOA_TO_RVA(*pNewBuffer,(DWORD)pIMPORT_DESCRIPTOR_Temp + 40 - (DWORD)*pNewBuffer,&RVA); // INT��ռ8���ֽ�
	pIMPORT_DESCRIPTOR_Temp->OriginalFirstThunk = RVA;  //�����ָ��������INT�� �����RVA������ǰ�滹��Ҫת����

	FOA_TO_RVA(*pNewBuffer,(DWORD)pIMPORT_DESCRIPTOR_Temp + 40 + 8 - (DWORD)*pNewBuffer,&RVA); // IAT��ռ8���ֽ�
	pIMPORT_DESCRIPTOR_Temp->FirstThunk = RVA;// �����ָ�������ص�IAT �����RVA������ǰ�滹��Ҫת����

	FOA_TO_RVA(*pNewBuffer,(DWORD)pIMPORT_DESCRIPTOR_Temp + 40 + 16 - (DWORD)*pNewBuffer,&RVA); // dll������ռ8���ֽڣ������Լ���ģ�� dll����Ϊabc.dll ����Ϊ7���ֽ� ���һ���ֽ�Ϊ\0
	pIMPORT_DESCRIPTOR_Temp->Name = RVA; // �����ָ�������ص�DLL���� �����RVA ����ǰ�滹��Ҫת����

	strcpy((PVOID)((DWORD)pIMPORT_DESCRIPTOR_Temp + 40 + 16),"abc.dll");

	
	/*
	���岽��			
	׷��8���ֽڵ�INT��  8���ֽڵ�IAT��	��һ��_IMAGE_THUNK_DATA32�ṹ��4���ֽ� ���ǻ���Ҫ4���ֽ�����Ϊ�����ı�ʶ��	���������ܹ���ռ16���ֽ�	
	*/
	
	FOA_TO_RVA(*pNewBuffer, ((DWORD)pIMPORT_DESCRIPTOR_Temp + 40 + 24 - (DWORD)*pNewBuffer),&RVA);

	*(PDWORD)((DWORD)pIMPORT_DESCRIPTOR_Temp + 40) = RVA; //_IMAGE_THUNK_DATA32�ṹ�е�����ָ��PIMAGE_IMPORT_BY_NAME �����RVA ����ǰ����Ҫת����

	FOA_TO_RVA(*pNewBuffer, ((DWORD)pIMPORT_DESCRIPTOR_Temp + 40 + 24 - (DWORD)*pNewBuffer),&RVA);
	
	*(PDWORD)((DWORD)pIMPORT_DESCRIPTOR_Temp + 40 + 8) = RVA; //ָ��PIMAGE_IMPORT_BY_NAME �����RVA ����ǰ����Ҫת����

	/*
	��������							
	  ׷��һ��IMAGE_IMPORT_BY_NAME �ṹ��ǰ2���ֽ���0 �����Ǻ��������ַ���							
	*/

	//IMPORT_BY_NAME = (PIMAGE_IMPORT_BY_NAME)((DWORD)pIMPORT_DESCRIPTOR_Temp + 40 + 26);

	//IMPORT_BY_NAME->Hint = 0;
	//IMPORT_BY_NAME->Name = "myFun";
	//strcpy(&IMPORT_BY_NAME->Name,"myFun");

	*(PWORD)((DWORD)pIMPORT_DESCRIPTOR_Temp + 40 + 26) = 0;
	strcpy((PVOID)((DWORD)pIMPORT_DESCRIPTOR_Temp + 40 + 26),"myFun");//����д���ˣ�����������ΪmyFun

	/*
	���߲���								
	  ����IMAGE_DATA_DIRECTORY�ṹ��VirtualAddress��Size
	*/

	
	FOA_TO_RVA(*pNewBuffer,(DWORD)SectionOfNew - (DWORD)*pNewBuffer,&RVA);
	pImageOptionalHeader->DataDirectory[1].VirtualAddress = RVA;
	pImageOptionalHeader->DataDirectory[1].Size = (DWORD)pImageOptionalHeader->DataDirectory[1].Size + 20;

	//�����д��̲���
	MyWriteFile(*pNewBuffer, NewLength);
}