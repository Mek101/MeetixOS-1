/**
 * @brief
 * This file is part of the MeetiX Operating System.
 * Copyright (c) 2017-2022, Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @developers
 * Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @license
 * GNU General Public License version 3
 */

#include "List.hh"

#include <Api/Info.h>
#include <iomanip>
#include <iostream>

#define WIDTH_BUS       6
#define WIDTH_SLOT      6
#define WIDTH_FUNC      6
#define WIDTH_VENDOR    5
#define WIDTH_DEV_ID    5
#define WIDTH_CLASS     28
#define WIDTH_SUB_CLASS 28

static constexpr struct PCIClassMapping {
    u8          m_class_code;
    u8          m_subclass_code;
    u8          m_program_if;
    const char* m_class_name;
    const char* m_subclass_name;
} PCI_CLASS_CODE_MAPPINGS[] = {
    { 0x00, 0x00, 0x00, "Other pre-2.0 device", "Other" },
    { 0x00, 0x01, 0x00, "Other pre-2.0 device", "VGA Compatible" },

    { 0x01, 0x00, 0x00, "Mass Storage Controller", "SCSI Bus Controller" },
    { 0x01, 0x01, 0x00, "Mass Storage Controller", "IDE Controller" },
    { 0x01, 0x02, 0x00, "Mass Storage Controller", "Floppy Disk Controller" },
    { 0x01, 0x03, 0x00, "Mass Storage Controller", "IPI Bus Controller" },
    { 0x01, 0x04, 0x00, "Mass Storage Controller", "RAID Controller" },
    { 0x01, 0x05, 0x20, "Mass Storage Controller", "ATA Controller (Single DMA)" },
    { 0x01, 0x05, 0x30, "Mass Storage Controller", "ATA Controller (Chained DMA)" },
    { 0x01, 0x06, 0x00, "Mass Storage Controller", "SATA (Vendor Spec. Interface)" },
    { 0x01, 0x06, 0x01, "Mass Storage Controller", "SATA (AHCI 1.0)" },
    { 0x01, 0x07, 0x00, "Mass Storage Controller", "Serial Attached SCSI (SAS)" },
    { 0x01, 0x80, 0x00, "Mass Storage Controller", "Other" },

    { 0x02, 0x00, 0x00, "Network Controller", "Ethernet Controller" },
    { 0x02, 0x01, 0x00, "Network Controller", "Token Ring Controller" },
    { 0x02, 0x02, 0x00, "Network Controller", "FDDI Controller" },
    { 0x02, 0x03, 0x00, "Network Controller", "ATM Controller" },
    { 0x02, 0x04, 0x00, "Network Controller", "ISDN Controller" },
    { 0x02, 0x05, 0x00, "Network Controller", "WorldFip Controller" },
    { 0x02, 0x06, 0x00, "Network Controller", "PICMG 2.14 Multi Computing" },
    { 0x02, 0x80, 0x00, "Network Controller", "Other" },

    { 0x03, 0x00, 0x00, "Display Controller", "VGA-Compatible Controller" },
    { 0x03, 0x00, 0x01, "Display Controller", "8512-Compatible Controller" },
    { 0x03, 0x01, 0x00, "Display Controller", "XGA Controller" },
    { 0x03, 0x02, 0x00, "Display Controller", "3D Controller (Not VGA-Compatible)" },
    { 0x03, 0x80, 0x00, "Display Controller", "Other" },

    { 0x04, 0x00, 0x00, "Multimedia Device", "Video" },
    { 0x04, 0x01, 0x00, "Multimedia Device", "Audio" },
    { 0x04, 0x02, 0x00, "Multimedia Device", "Computer Telephony Device" },
    { 0x04, 0x80, 0x00, "Multimedia Device", "Other" },

    { 0x05, 0x00, 0x00, "Memory Controller", "RAM Controller" },
    { 0x05, 0x01, 0x00, "Memory Controller", "Flash Controller" },
    { 0x05, 0x80, 0x00, "Memory Controller", "Other" },

    { 0x06, 0x00, 0x00, "Bridge Device", "Host-PCI" },
    { 0x06, 0x01, 0x00, "Bridge Device", "ISA Bridge" },
    { 0x06, 0x02, 0x00, "Bridge Device", "EISA Bridge" },
    { 0x06, 0x03, 0x00, "Bridge Device", "MCA Bridge" },
    { 0x06, 0x04, 0x00, "Bridge Device", "PCI-to-PCI Bridge" },
    { 0x06, 0x04, 0x01, "Bridge Device", "PCI-to-PCI Bridge (Subtractive Decode)" },
    { 0x06, 0x05, 0x00, "Bridge Device", "PCMCIA Bridge" },
    { 0x06, 0x06, 0x00, "Bridge Device", "NuBus Bridge" },
    { 0x06, 0x07, 0x00, "Bridge Device", "CardBus Bridge" },
    { 0x06, 0x08, 0x00, "Bridge Device", "RACEway Bridge" },
    { 0x06, 0x09, 0x00, "Bridge Device", "PCI-to-PCI Bridge (Primary, Semi-Transparent)" },
    { 0x06, 0x09, 0x01, "Bridge Device", "PCI-to-PCI Bridge (Secondary, Semi-Transparent)" },
    { 0x06, 0x0A, 0x00, "Bridge Device", "InfiniBrand-to-PCI Host Bridge" },
    { 0x06, 0x80, 0x00, "Bridge Device", "Other" },

    { 0x07, 0x00, 0x00, "Simple Communications Controller", "Generic XT-Compatible Serial Controller" },
    { 0x07, 0x00, 0x01, "Simple Communications Controller", "16450-Compatible Serial Controller" },
    { 0x07, 0x00, 0x02, "Simple Communications Controller", "16550-Compatible Serial Controller" },
    { 0x07, 0x00, 0x03, "Simple Communications Controller", "16650-Compatible Serial Controller" },
    { 0x07, 0x00, 0x04, "Simple Communications Controller", "16750-Compatible Serial Controller" },
    { 0x07, 0x00, 0x05, "Simple Communications Controller", "16850-Compatible Serial Controller" },
    { 0x07, 0x00, 0x06, "Simple Communications Controller", "16950-Compatible Serial Controller" },
    { 0x07, 0x01, 0x00, "Simple Communications Controller", "Parallel Port" },
    { 0x07, 0x01, 0x01, "Simple Communications Controller", "Bi-Directional Parallel Port" },
    { 0x07, 0x01, 0x02, "Simple Communications Controller", "ECP 1.X Compliant Parallel Port" },
    { 0x07, 0x01, 0x03, "Simple Communications Controller", "IEEE 1284 Controller" },
    { 0x07, 0x01, 0xFE, "Simple Communications Controller", "IEEE 1284 Target Device" },
    { 0x07, 0x02, 0x00, "Simple Communications Controller", "Multiport Serial Controller" },
    { 0x07, 0x03, 0x00, "Simple Communications Controller", "Generic Modem" },
    { 0x07, 0x03, 0x01, "Simple Communications Controller", "Hayes (16450-Compatible Interface)" },
    { 0x07, 0x03, 0x02, "Simple Communications Controller", "Hayes (16550-Compatible Interface)" },
    { 0x07, 0x03, 0x03, "Simple Communications Controller", "Hayes (16650-Compatible Interface)" },
    { 0x07, 0x03, 0x04, "Simple Communications Controller", "Hayes (16750-Compatible Interface)" },
    { 0x07, 0x04, 0x00, "Simple Communications Controller", "IEEE 488.1/2 (GPIB) Controller" },
    { 0x07, 0x05, 0x00, "Simple Communications Controller", "Smart Card" },
    { 0x07, 0x80, 0x02, "Simple Communications Controller", "Other" },

    { 0x08, 0x00, 0x00, "Base Systems Peripheral", "Generic 8259 PIC" },
    { 0x08, 0x00, 0x01, "Base Systems Peripheral", "ISA PIC" },
    { 0x08, 0x00, 0x02, "Base Systems Peripheral", "EISA PIC" },
    { 0x08, 0x00, 0x10, "Base Systems Peripheral", "I/O APIC Interrupt Controller" },
    { 0x08, 0x00, 0x20, "Base Systems Peripheral", "I/O(x) APIC Interrupt Controller" },
    { 0x08, 0x01, 0x00, "Base Systems Peripheral", "Generic 8237 DMA Controller" },
    { 0x08, 0x01, 0x01, "Base Systems Peripheral", "ISA DMA Controller" },
    { 0x08, 0x01, 0x02, "Base Systems Peripheral", "EISA DMA Controller" },
    { 0x08, 0x02, 0x00, "Base Systems Peripheral", "Generic 8254 System Timer" },
    { 0x08, 0x02, 0x01, "Base Systems Peripheral", "ISA System Timer" },
    { 0x08, 0x02, 0x02, "Base Systems Peripheral", "EISA System Timer" },
    { 0x08, 0x03, 0x00, "Base Systems Peripheral", "Generic RTC Controller" },
    { 0x08, 0x03, 0x01, "Base Systems Peripheral", "ISA RTC Controller" },
    { 0x08, 0x04, 0x00, "Base Systems Peripheral", "Generic PCI Hot-Plug Controller" },
    { 0x08, 0x80, 0x00, "Base Systems Peripheral", "Other" },

    { 0x09, 0x00, 0x00, "Input Device", "Keyboard" },
    { 0x09, 0x01, 0x00, "Input Device", "Digitizer (Pen)" },
    { 0x09, 0x02, 0x00, "Input Device", "Mouse" },
    { 0x09, 0x03, 0x00, "Input Device", "Scanner Controller" },
    { 0x09, 0x04, 0x00, "Input Device", "Gameport Controller (Generic)" },
    { 0x09, 0x04, 0x01, "Input Device", "Gameport Controller (Legacy)" },
    { 0x09, 0x80, 0x00, "Input Device", "Other" },

    { 0x0A, 0x00, 0x00, "Docking Station", "Generic" },
    { 0x0A, 0x80, 0x00, "Docking Station", "Other" },

    { 0x0B, 0x00, 0x00, "Processor", "i386 Processor" },
    { 0x0B, 0x01, 0x00, "Processor", "i486 Processor" },
    { 0x0B, 0x02, 0x00, "Processor", "Pentium Processor" },
    { 0x0B, 0x10, 0x00, "Processor", "Alpha Processor" },
    { 0x0B, 0x20, 0x00, "Processor", "Power PC Processor" },
    { 0x0B, 0x30, 0x00, "Processor", "MIPS Processor" },
    { 0x0B, 0x40, 0x00, "Processor", "Co-processor" },

    { 0x0C, 0x00, 0x00, "Serial Bus Controller", "IEEE 1394 Controller (FireWire)" },
    { 0x0C, 0x00, 0x10, "Serial Bus Controller", "IEEE 1394 Controller (1394 OpenHCI Spec.)" },
    { 0x0C, 0x01, 0x00, "Serial Bus Controller", "ACCESS.bus" },
    { 0x0C, 0x02, 0x00, "Serial Bus Controller", "SSA" },
    { 0x0C, 0x03, 0x00, "Serial Bus Controller", "USB (Universal Host Controller Spec.)" },
    { 0x0C, 0x03, 0x10, "Serial Bus Controller", "USB (s_open Host Controller Spec.)" },
    { 0x0C, 0x03, 0x20, "Serial Bus Controller", "USB2 Host Controller (Intel Enhanced Host Controller Interface)" },
    { 0x0C, 0x03, 0x30, "Serial Bus Controller", "USB3 XHCI Controller" },
    { 0x0C, 0x03, 0x80, "Serial Bus Controller", "Unspecified USB Controller" },
    { 0x0C, 0x03, 0xFE, "Serial Bus Controller", "USB (Not Host Controller)" },
    { 0x0C, 0x04, 0x00, "Serial Bus Controller", "Fibre Channel" },
    { 0x0C, 0x05, 0x00, "Serial Bus Controller", "SMBus" },
    { 0x0C, 0x06, 0x00, "Serial Bus Controller", "InfiniBand" },
    { 0x0C, 0x07, 0x00, "Serial Bus Controller", "IPMI SMIC Interface" },
    { 0x0C, 0x07, 0x01, "Serial Bus Controller", "IPMI Kybd Controller Style Interface" },
    { 0x0C, 0x07, 0x02, "Serial Bus Controller", "IPMI Block Transfer Interface" },
    { 0x0C, 0x08, 0x00, "Serial Bus Controller", "SERCOS Interface Standard (IEC 61491)" },
    { 0x0C, 0x09, 0x00, "Serial Bus Controller", "CANbus" },

    { 0x0D, 0x00, 0x00, "Wireless Controller", "iRDA Compatible Controller" },
    { 0x0D, 0x01, 0x00, "Wireless Controller", "Consumer IR Controller" },
    { 0x0D, 0x10, 0x00, "Wireless Controller", "RF Controller" },
    { 0x0D, 0x11, 0x00, "Wireless Controller", "Bluetooth Controller" },
    { 0x0D, 0x12, 0x00, "Wireless Controller", "Broadband Controller" },
    { 0x0D, 0x20, 0x00, "Wireless Controller", "Ethernet Controller (802.11a)" },
    { 0x0D, 0x21, 0x00, "Wireless Controller", "Ethernet Controller (802.11b)" },
    { 0x0D, 0x80, 0x00, "Wireless Controller", "Other" },

    { 0x0E, 0x00, 0x00, "Intelligent I/O Controller", "Message FIFO" },
    { 0x0E, 0x00, 0x01, "Intelligent I/O Controller", "I20 Architecture" },

    { 0x0F, 0x01, 0x00, "Satellite Communication Controller", "TV Controller" },
    { 0x0F, 0x02, 0x00, "Satellite Communication Controller", "Audio Controller" },
    { 0x0F, 0x03, 0x00, "Satellite Communication Controller", "Voice Controller" },
    { 0x0F, 0x04, 0x00, "Satellite Communication Controller", "Data Controller" },

    { 0x10, 0x00, 0x00, "Encryption/Decryption Controller", "Network and Computing" },
    { 0x10, 0x10, 0x00, "Encryption/Decryption Controller", "Entertainment" },
    { 0x10, 0x80, 0x00, "Encryption/Decryption Controller", "Other" },

    { 0x11, 0x00, 0x00, "Data Acq./Signal Proc. Controllers", "DPIO Modules" },
    { 0x11, 0x01, 0x00, "Data Acq./Signal Proc. Controllers", "Performance Counters" },
    { 0x11,
      0x10,
      0x00,
      "Data Acq./Signal Proc. Controllers",
      "Communications Sync. + Time and Frequency s_test/Measurement" },
    { 0x11, 0x20, 0x00, "Data Acq./Signal Proc. Controllers", "Management Card" },
    { 0x11, 0x80, 0x00, "Data Acq./Signal Proc. Controllers", "Other" },

    { 0xFF, 0x00, 0x00, "Unknown", "Unknown device" }
};

int list_pci_devices() {
    /* obtain devices count */
    auto dev_count = s_get_pci_device_count();
    if ( !dev_count ) {
        std::cerr << "Failed to query the kernel for the number of PCI devices\n";
        return EXIT_FAILURE;
    }

    /* list the devices */
    std::cout << "Found " << dev_count << " PCI devices\n";
    std::cout << std::setw(WIDTH_BUS) << std::left << "B.";
    std::cout << std::setw(WIDTH_SLOT) << std::left << "S.";
    std::cout << std::setw(WIDTH_FUNC) << std::left << "F.";
    std::cout << std::setw(WIDTH_VENDOR) << std::left << "Ven.";
    std::cout << std::setw(WIDTH_DEV_ID) << std::left << "Dev.";
    std::cout << std::setw(WIDTH_CLASS) << std::left << "Class";
    std::cout << std::setw(WIDTH_SUB_CLASS) << std::left << "SubClass";
    std::cout << '\n';

    for ( auto pos = 0; pos < dev_count; ++pos ) {
        /* obtain the next PciDeviceHeader */
        PCIDeviceHeader pci_device_header{};
        auto            pci_device_status = s_get_pci_device(pos, &pci_device_header);
        if ( !pci_device_status ) {
            std::cerr << "Failed to query the kernel for PCI device n" << pos;
            return EXIT_FAILURE;
        }

        /* find the matching entry */
        PCIClassMapping* matching_entry           = nullptr;
        PCIClassMapping* matching_no_progif_entry = nullptr;
        for ( auto& class_mapping : PCI_CLASS_CODE_MAPPINGS ) {
            if ( class_mapping.m_class_code == pci_device_header.m_class_code
                 && class_mapping.m_subclass_code == pci_device_header.m_subclass_code
                 && class_mapping.m_program_if == pci_device_header.m_program ) {
                matching_entry = const_cast<PCIClassMapping*>(&class_mapping);
                break;
            } else if ( class_mapping.m_class_code == pci_device_header.m_class_code
                        && class_mapping.m_subclass_code == pci_device_header.m_subclass_code ) {
                matching_no_progif_entry = const_cast<PCIClassMapping*>(&class_mapping);
                break;
            }
        }

        std::string_view class_name{ "Unknown" };
        std::string_view subclass_name{ "Unknown" };
        if ( matching_entry ) {
            class_name    = matching_entry->m_class_name;
            subclass_name = matching_entry->m_subclass_name;
        } else if ( matching_no_progif_entry ) {
            class_name    = matching_no_progif_entry->m_class_name;
            subclass_name = matching_no_progif_entry->m_subclass_name;
        }

        std::cout << std::setfill(' ');
        std::cout << std::hex << std::setw(WIDTH_BUS) << std::left << pci_device_header.m_dev_bus;
        std::cout << std::hex << std::setw(WIDTH_SLOT) << std::left << pci_device_header.m_dev_slot;
        std::cout << std::hex << std::setw(WIDTH_FUNC) << std::left << pci_device_header.m_dev_function;
        std::cout << std::hex << std::setw(WIDTH_VENDOR) << std::left << pci_device_header.m_vendor_id;
        std::cout << std::hex << std::setw(WIDTH_DEV_ID) << std::left << pci_device_header.m_device_id;
        std::cout << std::setw(WIDTH_CLASS) << class_name;
        std::cout << std::setw(WIDTH_SUB_CLASS) << subclass_name << std::endl;
    }
    return EXIT_SUCCESS;
}
