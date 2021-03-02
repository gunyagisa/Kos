#include <stdint.h>
#include "efi.h"

#define NULL 0

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


  // load the kernel
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

  // read the kernel header

  // This structure is described by kernel.ld
  struct KernelHeader {
    uint64_t *bss_start;
    uint64_t bss_size;
  } header;

  uint64_t header_size = sizeof(struct KernelHeader);
  status = root->Read(kernel, &header_size, (void *)&header);

  kernel_size -= header_size;

  // read the kernel body
  status = root->Read(kernel, &kernel_size, (void *)0x120000);

  if (status == EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"kernel is loaded!\r\n");
  }
  status = root->Close(kernel);

  SystemTable->BootServices->SetMem(header.bss_start, header.bss_size, 0);

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
      "mov rdi, %0\n"
      "mov rsp, %1\n"
      "jmp %2\n"
      :: "r"(&fb), "r"(kernel_stack), "r"(kernel_start)
      );

  for(;;) ;
}
