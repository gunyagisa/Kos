#include <stdint.h>

#define NULL 0
#define EFI_SUCCESS 0

typedef uint64_t UINTN;
typedef void* EFI_HANDLE;
typedef uint64_t EFI_STATUS;

typedef struct {
  uint32_t guid1;
  uint16_t guid2;
  uint16_t guid3;
  uint8_t  guid4[8];
} EFI_GUID;

typedef struct {
  uint64_t sig;
  uint32_t rev;
  uint32_t size;
  uint32_t CRC32;
  uint32_t reserved;
} EFI_TABLE_HEADER;

typedef struct {
  uint32_t Type;
  uint64_t              PhysicalStart;
  uint64_t              VirtualStart;
  uint64_t              PageNum;
  uint64_t              Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef struct {
  EFI_TABLE_HEADER Hdr;
  uint64_t              buf[4];
  uint64_t      (*GetMemoryMap)(UINTN *MemoryMapSize, 
                                EFI_MEMORY_DESCRIPTOR *MemoryMap, 
                                UINTN *MapKey, UINTN *DescriptroSize, uint32_t *DescriptroVersion);
  uint64_t      buf2[21];
  uint64_t      (*ExitBootServices)(EFI_HANDLE ImageHandle, UINTN MapKey);
  uint64_t      buf3[10];
  uint64_t      (*LocateProtocol)(EFI_GUID *Protocol, void *Registration, void **Interface);
} EFI_BOOT_SERVICES;

typedef struct {
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
  unsigned long long Reset;
  unsigned long long (*OutputString)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, unsigned short *String);
  unsigned long long buf[4];
  unsigned long long (*clearScreen) (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *Thsi);
  unsigned long long buf2[3];
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct {
} EFI_RUNTIME_SERVICES;

typedef struct {
} EFI_CONFIGURATION_TABLE;

typedef struct {
  EFI_TABLE_HEADER                      Hdr;
  uint64_t                              buf1;
  uint32_t                              rev;
  EFI_HANDLE                            ConsoleInHandle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL        *ConIn;
  EFI_HANDLE                            ConsoleOutHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *ConOut;
  EFI_HANDLE                            StandardErrorHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *StdErr;
  EFI_RUNTIME_SERVICES                  *RuntimeServices;
  EFI_BOOT_SERVICES                     *BootServices;
  UINTN                                  num;
  EFI_CONFIGURATION_TABLE               *ConfigTable;
} EFI_SYSTEM_TABLE;

// EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
//
#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
 {0x0964e5b22,0x6459,0x11d2,\
 {0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}
struct EFI_FILE_PROTOCOL;

typedef struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
  uint64_t Rev;
  uint64_t (*OpenVolume)(struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This, struct EFI_FILE_PROTOCOL **Root);
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

// EFI_FILE_PROTOCOL
// structure
typedef struct EFI_FILE_PROTOCOL {
  uint64_t rev;
  uint64_t (*Open)(struct EFI_FILE_PROTOCOL *This, 
                   struct EFI_FILE_PROTOCOL **NewHandle, 
                   uint16_t *FileName, 
                   uint64_t OpenMode, uint64_t Attributes);
  uint64_t buf[2];
  uint64_t (*Read)(struct EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
  uint64_t buf2[3];
  uint64_t (*GetInfo)(struct EFI_FILE_PROTOCOL *This, EFI_GUID *Type, UINTN *Size, void *Buffer);

} EFI_FILE_PROTOCOL;

#define EFI_FILE_INFO_ID \
 {0x09576e92,0x6d3f,0x11d2,\
 {0x8e, 0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

// Open Modes
#define EFI_FILE_MODE_READ      0x0000000000000001
#define EFI_FILE_MODE_WRITE     0x0000000000000002
#define EFI_FILE_MODE_CREATE    0x8000000000000000


void ExitBootServices ( EFI_HANDLE ImageHandle, UINTN MapKey);


EFI_STATUS UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN mapsize, mapkey, descriptorsize;
  EFI_MEMORY_DESCRIPTOR map;
  uint32_t desc_ver;
  unsigned long long status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;
  EFI_FILE_PROTOCOL *root;
  EFI_FILE_PROTOCOL *kernel;


  SystemTable->ConOut->clearScreen(SystemTable->ConOut);
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello UEFI\n\r");

  // Get EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
  EFI_GUID sfsp_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
  SystemTable->BootServices->LocateProtocol(&sfsp_guid, NULL, (void **)&SFSP);

  status = SFSP->OpenVolume(SFSP, &root);
  if (status == EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Success OpenVolume\n\r");
  }


  // load the kernel file
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Start Loading Kos Kernel.\n\r");

  status = root->Open(root, &kernel, L"kernel.bin", EFI_FILE_MODE_READ, 0);
  if (status == EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Success root->Open\n\r");
  }

  UINTN kernel_size;
  EFI_GUID fileinfo_guid = EFI_FILE_INFO_ID;
  UINTN FInfo_size = 64;
  struct {
    uint64_t size;
  } FInfo;
  kernel->GetInfo(kernel, &fileinfo_guid, &FInfo_size, &FInfo);
  kernel_size = FInfo.size;
  status = root->Read(kernel, &kernel_size, (void *)0x1200);

  if (status == EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Success root->Read\n\r");
  }

  // ExitBootServices
  SystemTable->BootServices->GetMemoryMap(&mapsize, &map, &mapkey, &descriptorsize, &desc_ver);
  SystemTable->BootServices->ExitBootServices(ImageHandle, mapkey);

  for(;;) ;
}
