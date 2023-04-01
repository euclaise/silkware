global call_user
call_user:
    mov rcx, 0x100000
    mov r11, 0x202
    o64 sysret
