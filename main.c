#include <stdint.h>

typedef uint64_t UINTN;
typedef void* EFI_HANDLE;

typedef void EFI_STATUS;

typedef struct {
  uint64_t sig;
  uint32_t rev;
  uint32_t size;
  uint32_t CRC32;
  uint32_t reserved;
} EFI_TABLE_HEADER;

typedef struct {
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

void ExitBootServices ( EFI_HANDLE ImageHandle, UINTN MapKey);


EFI_STATUS UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  SystemTable->ConOut->clearScreen(SystemTable->ConOut);
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello UEFI\n");

  for(;;) ;
}
