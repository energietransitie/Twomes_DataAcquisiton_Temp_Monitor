@echo off

echo USING CUSTOM TWOMES UPLOADER

mkdir .\binaries
copy ".\.pio\build\esp32dev\partitions.bin" ".\binaries"
copy ".\.pio\build\esp32dev\firmware.bin" ".\binaries"

echo %1
echo %2

IF [%3]==[] goto autodetect
goto manual

:autodetect
    echo Uploading to device...
    %1 %2 --chip esp32 --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 .\binaries\bootloader_qio_80m.bin 0x8000 .\binaries\partitions.bin 0xe000 .\binaries\boot_app0.bin 0x10000 .\binaries\firmware.bin  
    goto end

:manual
    echo Uploading to device on %3
    %1 %2 --chip esp32 --port %3 --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 .\binaries\bootloader_qio_80m.bin 0x8000 .\binaries\partitions.bin 0xe000 .\binaries\boot_app0.bin 0x10000 .\binaries\firmware.bin
    goto end

:end
timeout 5