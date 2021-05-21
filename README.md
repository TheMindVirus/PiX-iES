# PiX-iES
rpi4-osdev working example of Raspberry Pi Zero Framebuffer being controlled by ARM core (first boot of PiX GPU)
https://github.com/isometimes/rpi4-osdev

```
    mmio_write(0x2010010C, 0x5A00107F); //PM_GRAFX
    mmio_write(0x2000A00C, 0x5A000000); //ASB_V3D_M_CTRL
    mmio_write(0x2000A008, 0x5A000000); //ASB_V3D_S_CTRL
```

![v3dpower](https://github.com/TheMindVirus/PiX-iES/blob/pi0-v3d/V3Dpower.png)
