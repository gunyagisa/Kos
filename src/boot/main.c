#include <stdint.h>

#define NULL 0
#define EFI_SUCCESS 0
#define EFI_UNSUPPORTED 3
#define EFI_NO_MEDIA 12
#define EFI_DEVICE_ERROR 7
#define EFI_VOLUME_CORRUPTED 10
#define EFI_BUFFER_TOO_SMALL 5

typedef uint64_t UINTN;
typedef void* EFI_HANDLE;
typedef unsigned long long EFI_STATUS;

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
  uint64_t (*Close)(struct EFI_FILE_PROTOCOL *This);
  uint64_t buf;
  uint64_t (*Read)(struct EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
  uint64_t buf2[3];
  uint64_t (*GetInfo)(struct EFI_FILE_PROTOCOL *This, EFI_GUID *Type, UINTN *Size, void *Buffer);

} EFI_FILE_PROTOCOL;

struct EFI_TIME {
  uint16_t Year;
  uint8_t Month;
  uint8_t Day;
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Second;
  uint8_t Pad1;
  uint32_t Nanosecond;
  int16_t TimeZone;
  uint8_t Daylight;
  uint8_t Pad2;
} EFI_TIME;

typedef struct EFI_FILE_INFO {
  uint64_t Size;
  uint64_t FileSize;
  uint64_t PhysicalSize;
  struct EFI_TIME CreateTime;
  struct EFI_TIME LastAccessTime;
  struct EFI_TIME ModificationTime;
  uint64_t Attribute;
  uint16_t FileName[];
} EFI_FILE_INFO;

#define EFI_FILE_INFO_ID \
 {0x09576e92,0x6d3f,0x11d2,\
 {0x8e, 0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

// Open Modes
#define EFI_FILE_MODE_READ      0x0000000000000001
#define EFI_FILE_MODE_WRITE     0x0000000000000002
#define EFI_FILE_MODE_CREATE    0x8000000000000000

// EFI_GRAPHIC_OUTPUT_PROTOCOL
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
 {0x9042a9de,0x23dc,0x4a38,\
 {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}

typedef struct EFI_GRAPHIC_OUTPUT_PROTOCOL {
  uint64_t querymode;
  uint64_t (*SetMode)(struct EFI_GRAPHIC_OUTPUT_PROTOCOL *This, uint32_t ModeNumber);
  uint64_t blt;
  
  // this contains graphics information. defined below.
  struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
} EFI_GRAPHIC_OUTPUT_PROTOCOL;

typedef struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE {
  uint32_t MaxMode;
  uint32_t Mode;
  uint64_t buf;
  struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  uint64_t      FrameBufferBase;
  UINTN         FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION {
  uint32_t Version;
  uint32_t HorizontalResolution;
  uint32_t VericalResolution;
  uint32_t Format;
  uint32_t Info[4];
  uint32_t PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

void ExitBootServices ( EFI_HANDLE ImageHandle, UINTN MapKey);

struct framebuffer {
  uint64_t base;
  uint64_t size;
  uint64_t x_size;
  uint64_t y_size;
  uint32_t pps;
} fb;

unsigned char map[4800];

EFI_STATUS UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  unsigned long long status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;
  EFI_FILE_PROTOCOL *root;
  EFI_FILE_PROTOCOL *kernel;


  SystemTable->ConOut->clearScreen(SystemTable->ConOut);
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello UEFI\r\n");

  // Get EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
  EFI_GUID sfsp_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
  SystemTable->BootServices->LocateProtocol(&sfsp_guid, NULL, (void **)&SFSP);

  status = SFSP->OpenVolume(SFSP, &root);
  if (status == EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Success OpenVolume\r\n");
  }


  // load the kernel file
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Start Loading Kos Kernel.\r\n");

  status = root->Open(root, &kernel, L"kernel.bin", EFI_FILE_MODE_READ, 0);
  if (status == EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Success root->Open\r\n");
  }

  UINTN kernel_size;
  EFI_GUID fileinfo_guid = EFI_FILE_INFO_ID;


  EFI_FILE_INFO *finfo;
  uint64_t fi_size = 180;
  uint64_t fi_buf[180];
  status = kernel->GetInfo(kernel, &fileinfo_guid, &fi_size, fi_buf);

  if (status == EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Success kernel->GetInfo()\r\n");
  }

  finfo = (EFI_FILE_INFO *) fi_buf;

  kernel_size = finfo->FileSize;
  status = root->Read(kernel, &kernel_size, (void *)0x120000);

  if (status == EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"kernel is loaded!\r\n");
  }

  status = root->Close(kernel);

  // Get Graphics_Output_Protocol
  EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHIC_OUTPUT_PROTOCOL *GOP;
  status = SystemTable->BootServices->LocateProtocol(&gop_guid, NULL, (void **)&GOP);
  if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"failed Get GOP\r\n");
  }

  /*
  status = GOP->SetMode(GOP, 2);
  if (status == EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Success SetMode:2\r\n");
  }
  */

  fb.base = GOP->Mode->FrameBufferBase;
  fb.size = GOP->Mode->FrameBufferSize;
  fb.x_size = GOP->Mode->Info->HorizontalResolution;
  fb.y_size = GOP->Mode->Info->VericalResolution;
  fb.pps = GOP->Mode->Info->PixelsPerScanLine;

  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"finish preparation.\r\n");
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"See you again!\r\n");

  // ExitBootServices
  UINTN mapsize, mapkey, descriptorsize;
  uint32_t desc_ver;
  mapsize = 4800;

  do {
    status = SystemTable->BootServices->GetMemoryMap(&mapsize, map, &mapkey, &descriptorsize, &desc_ver);
  } while (status != EFI_SUCCESS);

  SystemTable->BootServices->ExitBootServices(ImageHandle, mapkey);


  uint64_t kernel_stack = 0x10000;

  unsigned long long kernel_start = 0x120000;
  asm (
      ".intel_syntax noprefix\n"
      "mov %%rdi, %0\n"
      "mov %%rsp, %1\n"
      "jmp %2\n"
      :: "r"(&fb), "r"(kernel_stack), "r"(kernel_start)
      );

  for(;;) ;
}
