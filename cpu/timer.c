void pic_remap() {
    unsigned char a1, a2;

    // Save existing masks
    a1 = port_byte_in(0x21);
    a2 = port_byte_in(0xA1);

    // ICW1: Start initialization sequence (in cascade mode)
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);

    // ICW2: Set vector offsets in IDT
    port_byte_out(0x21, 0x20); // Master PIC starts at 32 (0x20)
    port_byte_out(0xA1, 0x28); // Slave PIC starts at 40 (0x28)

    // ICW3: Tell Master about Slave, and Slave about Master
    port_byte_out(0x21, 0x04); // Master: IRQ2 connected to Slave
    port_byte_out(0xA1, 0x02); // Slave's cascade identity

    // ICW4: Set 8086/88 mode
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);

    // Restore saved masks (or send 0x00 to enable all hardware interrupts)
    port_byte_out(0x21, 0x00);
    port_byte_out(0xA1, 0x00);
}
