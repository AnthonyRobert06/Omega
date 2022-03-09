#include <efi.h>
#include <efilib.h>
#include <stddef.h>
#include <elf.h>


int memcmp(const void* aptr, const void* bptr, size_t n) {
	const unsigned char* a = aptr, *b = bptr;
	for (size_t i = 0; i < n; i++) {
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}
	return 0;
}


typedef struct {
    void* baseAddr;
    size_t bufferSize;
    unsigned int width;
    unsigned int height;
    unsigned int ppsl;      // Pixels per scanline.
} framebuffer_t;


framebuffer_t* init_framebuffer(EFI_SYSTEM_TABLE* sysTable) {
    EFI_GUID gopGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;

    EFI_STATUS s = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGUID, NULL, (void**)&gop);

    if (EFI_ERROR(s)) {
        return NULL;
    }

    framebuffer_t* lfb;
    sysTable->BootServices->AllocatePool(EfiLoaderData, sizeof(framebuffer_t), (void**)&lfb);
    lfb->baseAddr = (void*)gop->Mode->FrameBufferBase;
    lfb->bufferSize = gop->Mode->FrameBufferSize;
    lfb->width = gop->Mode->Info->HorizontalResolution;
    lfb->height = gop->Mode->Info->VerticalResolution;
    lfb->ppsl = gop->Mode->Info->PixelsPerScanLine;
    return lfb;
}


EFI_FILE* loadFile(EFI_FILE* dir, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* sysTable) {
    EFI_FILE* fileRes;
    EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
    sysTable->BootServices->HandleProtocol(imageHandle, &gEfiLoadedImageProtocolGuid, (void**)&loadedImage);

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
    sysTable->BootServices->HandleProtocol(loadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&fs);

    if (!(dir)) {
        fs->OpenVolume(fs, &dir);
    }

    EFI_STATUS s = dir->Open(dir, &fileRes, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

    if (s != EFI_SUCCESS) {
        return NULL;
    }

    return fileRes;
}



EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* sysTable) {
    InitializeLib(imageHandle, sysTable);

    EFI_FILE* kernel = loadFile(NULL, L"kernel.elf", imageHandle, sysTable);

    if (!(kernel)) {
        Print(L"Failed to load kernel ELF!\n");
    } else {
        Elf64_Ehdr header;
        UINTN size = sizeof(header);
        kernel->Read(kernel, &size, &header);

        if (memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
                header.e_ident[EI_CLASS] != ELFCLASS64 || 
                header.e_type != ET_EXEC || 
                header.e_machine != EM_X86_64 || header.e_version != EV_CURRENT) {
            Print(L"Kernel may be corrupted, header bad.\n");
        } else {
            Elf64_Phdr* progHeaders;
            kernel->SetPosition(kernel, header.e_phoff);
            UINTN progHeaderSize = header.e_phnum * header.e_phentsize;
            sysTable->BootServices->AllocatePool(EfiLoaderData, progHeaderSize, (void**)&progHeaders);
            kernel->Read(kernel, &progHeaderSize, progHeaders);

            for (Elf64_Phdr* phdr = progHeaders; (char*)phdr < (char*)progHeaders + header.e_phnum * header.e_phentsize; phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)) {
                if (phdr->p_type == PT_LOAD) {
                    int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
                    Elf64_Addr segment = phdr->p_paddr;

                    sysTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
                    kernel->SetPosition(kernel, phdr->p_offset);

                    UINTN size = phdr->p_filesz;
                    kernel->Read(kernel, &size, (void*)segment);
                }
            }

            framebuffer_t* lfb = init_framebuffer(sysTable);
            void(*kernel_entry)(framebuffer_t*) = ((__attribute__((sysv_abi))void(*)(framebuffer_t*))header.e_entry);
            kernel_entry(lfb);
        }
    }

    return EFI_SUCCESS;
}
