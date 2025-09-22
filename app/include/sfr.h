
#ifndef _SFR_
#define _SFR_


#ifndef __ASSEMBLER__
#define SFR_RO *(volatile unsigned long const *)
#define SFR_WO *(volatile unsigned long*)
#define SFR_RW *(volatile unsigned long*)
#define SWINT()      asm(".long 0xb0030057")
#define EEBREAKINT() asm(".long 0xb0040057")
#else
#define SFR_RO
#define SFR_WO
#define SFR_RW
#define SWINT      .long 0xb0030057
#define EEBREAKINT .long 0xb0040057
#endif

#define SFR_BASE   0x00000100    // address 0~255 is reserved
#define SFR0_BASE  (SFR_BASE + 0x0000)
#define SFR1_BASE  (SFR_BASE + 0x0100)
#define SFR2_BASE  (SFR_BASE + 0x0200)
#define SFR3_BASE  (SFR_BASE + 0x0300)
#define SFR4_BASE  (SFR_BASE + 0x0400)
#define SFR5_BASE  (SFR_BASE + 0x0500)
#define SFR6_BASE  (SFR_BASE + 0x0600)
#define SFR7_BASE  (SFR_BASE + 0x0700)
#define SFR8_BASE  (SFR_BASE + 0x0800)
#define SFR9_BASE  (SFR_BASE + 0x0900)
#define SFR10_BASE (SFR_BASE + 0x0a00)
#define SFR11_BASE (SFR_BASE + 0x0b00)
#define SFR12_BASE (SFR_BASE + 0x0c00)
#define SFR13_BASE (SFR_BASE + 0x0d00)
#define SFR14_BASE (SFR_BASE + 0x0e00)
#define SFR15_BASE (SFR_BASE + 0x0f00)
#define SFR16_BASE (SFR_BASE + 0x1000)
#define SFR17_BASE (SFR_BASE + 0x1100)
#define SFR30_BASE (SFR_BASE + 0x1e00)

//------------------------- SFR Group0 ---------------------------------------//
#define TMR0CON              SFR_RW (SFR0_BASE + 0x00*4)
#define TMR0CPND             SFR_RW (SFR0_BASE + 0x01*4)
#define TMR0CNT              SFR_RW (SFR0_BASE + 0x02*4)
#define TMR0PR               SFR_RW (SFR0_BASE + 0x03*4)
#define TMR1CON              SFR_RW (SFR0_BASE + 0x04*4)
#define TMR1CPND             SFR_RW (SFR0_BASE + 0x05*4)
#define TMR1CNT              SFR_RW (SFR0_BASE + 0x06*4)
#define TMR1PR               SFR_RW (SFR0_BASE + 0x07*4)
#define TMR2CON              SFR_RW (SFR0_BASE + 0x08*4)
#define TMR2CPND             SFR_RW (SFR0_BASE + 0x09*4)
#define TMR2CNT              SFR_RW (SFR0_BASE + 0x0a*4)
#define TMR2PR               SFR_RW (SFR0_BASE + 0x0b*4)
#define TMR3CON              SFR_RW (SFR0_BASE + 0x0c*4)
#define TMR3CPND             SFR_RW (SFR0_BASE + 0x0d*4)
#define TMR3CNT              SFR_RW (SFR0_BASE + 0x0e*4)
#define TMR3PR               SFR_RW (SFR0_BASE + 0x0f*4)
#define TMR3CPT              SFR_RW (SFR0_BASE + 0x10*4)
#define TMR3DUTY0            SFR_RW (SFR0_BASE + 0x11*4)
#define TMR3DUTY1            SFR_RW (SFR0_BASE + 0x12*4)
#define TMR3DUTY2            SFR_RW (SFR0_BASE + 0x13*4)
#define TMR4CON              SFR_RW (SFR0_BASE + 0x14*4)
#define TMR4CPND             SFR_RW (SFR0_BASE + 0x15*4)
#define TMR4CNT              SFR_RW (SFR0_BASE + 0x16*4)
#define TMR4PR               SFR_RW (SFR0_BASE + 0x17*4)
#define TMR4CPT              SFR_RW (SFR0_BASE + 0x18*4)
#define TMR4DUTY0            SFR_RW (SFR0_BASE + 0x19*4)
#define TMR4DUTY1            SFR_RW (SFR0_BASE + 0x1a*4)
#define TMR4DUTY2            SFR_RW (SFR0_BASE + 0x1b*4)
#define TMR5CON              SFR_RW (SFR0_BASE + 0x1c*4)
#define TMR5CPND             SFR_RW (SFR0_BASE + 0x1d*4)
#define TMR5CNT              SFR_RW (SFR0_BASE + 0x1e*4)
#define TMR5PR               SFR_RW (SFR0_BASE + 0x1f*4)
#define TMR5CPT              SFR_RW (SFR0_BASE + 0x20*4)
#define TMR5DUTY0            SFR_RW (SFR0_BASE + 0x21*4)
#define TMR5DUTY1            SFR_RW (SFR0_BASE + 0x22*4)
#define TMR5DUTY2            SFR_RW (SFR0_BASE + 0x23*4)
#define TMR5DUTY3            SFR_RW (SFR0_BASE + 0x24*4)
#define TMR5DUTY4            SFR_RW (SFR0_BASE + 0x25*4)
#define TMR5DUTY5            SFR_RW (SFR0_BASE + 0x26*4)
#define UART0CON             SFR_RW (SFR0_BASE + 0x27*4)
#define UART0CPND            SFR_RW (SFR0_BASE + 0x28*4)
#define UART0BAUD            SFR_RW (SFR0_BASE + 0x29*4)
#define UART0DATA            SFR_RW (SFR0_BASE + 0x2a*4)
#define UART1CON             SFR_RW (SFR0_BASE + 0x2b*4)
#define UART1CPND            SFR_RW (SFR0_BASE + 0x2c*4)
#define UART1BAUD            SFR_RW (SFR0_BASE + 0x2d*4)
#define UART1DATA            SFR_RW (SFR0_BASE + 0x2e*4)
#define UART2CON             SFR_RW (SFR0_BASE + 0x2f*4)
#define UART2CPND            SFR_RW (SFR0_BASE + 0x30*4)
#define UART2BAUD            SFR_RW (SFR0_BASE + 0x31*4)
#define UART2DATA            SFR_RW (SFR0_BASE + 0x32*4)
#define HSUT0CON             SFR_RW (SFR0_BASE + 0x33*4)
#define HSUT0CPND            SFR_RW (SFR0_BASE + 0x34*4)
#define HSUT0BAUD            SFR_RW (SFR0_BASE + 0x35*4)
#define HSUT0DATA            SFR_RW (SFR0_BASE + 0x36*4)
#define HSUT0TXCNT           SFR_RW (SFR0_BASE + 0x37*4)
#define HSUT0TXADR           SFR_RW (SFR0_BASE + 0x38*4)
#define HSUT0RXCNT           SFR_RW (SFR0_BASE + 0x39*4)
#define HSUT0RXADR           SFR_RW (SFR0_BASE + 0x3a*4)
#define HSUT0FIFOCNT         SFR_RW (SFR0_BASE + 0x3b*4)
#define HSUT0FIFO            SFR_RW (SFR0_BASE + 0x3c*4)
#define HSUT0FIFOADR         SFR_RW (SFR0_BASE + 0x3d*4)
#define HSUT0TMRCNT          SFR_RW (SFR0_BASE + 0x3e*4)
#define HSUT0FCCON           SFR_RW (SFR0_BASE + 0x3f*4)

//------------------------- SFR Group1 ---------------------------------------//
#define WDTCON               SFR_RW (SFR1_BASE + 0x00*4)
#define RTCCON               SFR_RW (SFR1_BASE + 0x01*4)
#define RTCCPND              SFR_RW (SFR1_BASE + 0x02*4)
#define PWRCON0              SFR_RW (SFR1_BASE + 0x04*4)
#define PWRCON1              SFR_RW (SFR1_BASE + 0x05*4)
#define PWRCON2              SFR_RW (SFR1_BASE + 0x06*4)
#define PWRCON3              SFR_RW (SFR1_BASE + 0x07*4)
#define LPMCON               SFR_RW (SFR1_BASE + 0x08*4)
#define LVDCON               SFR_RW (SFR1_BASE + 0x09*4)
#define RSTCON0              SFR_RW (SFR1_BASE + 0x0a*4)
#define CRSTPND              SFR_RW (SFR1_BASE + 0x0b*4)
//efine                      SFR_RW (SFR1_BASE + 0x0c*4)
//efine                      SFR_RW (SFR1_BASE + 0x0d*4)
//efine                      SFR_RW (SFR1_BASE + 0x0e*4)
//efine                      SFR_RW (SFR1_BASE + 0x0f*4)
//efine                      SFR_RW (SFR1_BASE + 0x10*4)
//efine                      SFR_RW (SFR1_BASE + 0x11*4)
//efine                      SFR_RW (SFR1_BASE + 0x12*4)
#define XOSCCON              SFR_RW (SFR1_BASE + 0x13*4)
#define CLKCON0              SFR_RW (SFR1_BASE + 0x14*4)
#define CLKCON1              SFR_RW (SFR1_BASE + 0x15*4)
#define CLKCON2              SFR_RW (SFR1_BASE + 0x16*4)
#define CLKCON3              SFR_RW (SFR1_BASE + 0x17*4)
#define CLKCON4              SFR_RW (SFR1_BASE + 0x18*4)
#define CLKCON5              SFR_RW (SFR1_BASE + 0x19*4)
#define CLKGAT0              SFR_RW (SFR1_BASE + 0x1a*4)
#define CLKGAT1              SFR_RW (SFR1_BASE + 0x1b*4)
#define CLKGAT2              SFR_RW (SFR1_BASE + 0x1c*4)
#define CLKGAT3              SFR_RW (SFR1_BASE + 0x1d*4)
#define CLKGAT4              SFR_RW (SFR1_BASE + 0x1e*4)
#define CLKGAT5              SFR_RW (SFR1_BASE + 0x1f*4)
#define CLKDIVCON0           SFR_RW (SFR1_BASE + 0x20*4)
#define CLKDIVCON1           SFR_RW (SFR1_BASE + 0x21*4)
#define CLKDIVCON2           SFR_RW (SFR1_BASE + 0x22*4)
#define CLKDIVCON3           SFR_RW (SFR1_BASE + 0x23*4)
#define CLKDIVCON4           SFR_RW (SFR1_BASE + 0x24*4)
#define CLKDIVCON5           SFR_RW (SFR1_BASE + 0x25*4)
#define USERKEY           	 SFR_RW (SFR1_BASE + 0x26*4)
#define PROTCON1             SFR_RW (SFR1_BASE + 0x27*4)

#define PRCLKCON0            SFR_RW (SFR1_BASE + 0x2d*4)
#define PRCLKDIVCON0         SFR_RW (SFR1_BASE + 0x2e*4)
#define PRCLKDIVCON1         SFR_RW (SFR1_BASE + 0x2f*4)
#define PLL0CON0             SFR_RW (SFR1_BASE + 0x30*4)
#define PLL0CON1             SFR_RW (SFR1_BASE + 0x31*4)
#define PLL0DIV              SFR_RW (SFR1_BASE + 0x32*4)
#define VERSIONID            SFR_RW (SFR1_BASE + 0x3f*4)

//------------------------- SFR Group2 ---------------------------------------//
#define BTCON2               SFR_RW (SFR2_BASE + 0x00*4)
#define FUNCINCON            SFR_RW (SFR2_BASE + 0x1d*4)
#define FUNCOUTCON           SFR_RW (SFR2_BASE + 0x1e*4)
#define FUNCOUTMCON          SFR_RW (SFR2_BASE + 0x1f*4)
#define FUNCMCON0            SFR_RW (SFR2_BASE + 0x20*4)
#define FUNCMCON1            SFR_RW (SFR2_BASE + 0x21*4)
#define FUNCMCON2            SFR_RW (SFR2_BASE + 0x22*4)
#define FUNCMCON3            SFR_RW (SFR2_BASE + 0x23*4)
#define FUNCMCON4            SFR_RW (SFR2_BASE + 0x24*4)
#define FUNCMCON5            SFR_RW (SFR2_BASE + 0x25*4)
#define WKUPCON              SFR_RW (SFR2_BASE + 0x26*4)
#define WKUPEDG              SFR_RW (SFR2_BASE + 0x27*4)
#define WKUPIE               SFR_RW (SFR2_BASE + 0x28*4)
#define WKUPCPND             SFR_RW (SFR2_BASE + 0x29*4)
#define WKPINMAP             SFR_RW (SFR2_BASE + 0x2a*4)
#define PORTINTEDG           SFR_RW (SFR2_BASE + 0x2b*4)
#define PORTINTEN            SFR_RW (SFR2_BASE + 0x2c*4)
#define PORTINTRISESRC       SFR_RW (SFR2_BASE + 0x2d*4)
#define PORTINTFALLSRC       SFR_RW (SFR2_BASE + 0x2e*4)

#define TICK0CON             SFR_RW (SFR2_BASE + 0x38*4)
#define TICK0CPND            SFR_RW (SFR2_BASE + 0x39*4)
#define TICK0CNT             SFR_RW (SFR2_BASE + 0x3a*4)
#define TICK0PR              SFR_RW (SFR2_BASE + 0x3b*4)
#define TICK1CON             SFR_RW (SFR2_BASE + 0x3c*4)
#define TICK1CPND            SFR_RW (SFR2_BASE + 0x3d*4)
#define TICK1CNT             SFR_RW (SFR2_BASE + 0x3e*4)
#define TICK1PR              SFR_RW (SFR2_BASE + 0x3f*4)

//------------------------- SFR Group3 ---------------------------------------//
#define USBCON0              SFR_RW (SFR3_BASE + 0x00*4)
#define USBCON1              SFR_RW (SFR3_BASE + 0x01*4)
#define USBCON2              SFR_RW (SFR3_BASE + 0x02*4)
#define USBCON3              SFR_RW (SFR3_BASE + 0x03*4)
#define USBCON4              SFR_RW (SFR3_BASE + 0x04*4)
#define SPI1CON              SFR_RW (SFR3_BASE + 0x26*4)
#define SPI1BUF              SFR_RW (SFR3_BASE + 0x27*4)
#define SPI1BAUD             SFR_RW (SFR3_BASE + 0x28*4)
#define SPI1CPND             SFR_RW (SFR3_BASE + 0x29*4)
#define SPI1DMACNT           SFR_RW (SFR3_BASE + 0x2a*4)
#define SPI1DMAADR           SFR_RW (SFR3_BASE + 0x2b*4)

#define WPTDAT               SFR_RW (SFR3_BASE + 0x38*4)
#define WPTCON               SFR_RW (SFR3_BASE + 0x39*4)
#define WPTPND               SFR_RW (SFR3_BASE + 0x3a*4)
#define WPTADR               SFR_RW (SFR3_BASE + 0x3b*4)
#define PICCONCLR            SFR_RW (SFR4_BASE + 0x0c*4)
#define PICCONSET            SFR_RW (SFR4_BASE + 0x0d*4)
#define PICENCLR             SFR_RW (SFR4_BASE + 0x0e*4)
#define PICENSET             SFR_RW (SFR4_BASE + 0x0f*4)
#define PICCON               SFR_RW (SFR4_BASE + 0x10*4)
#define PICEN                SFR_RW (SFR4_BASE + 0x11*4)
#define PICPR                SFR_RW (SFR4_BASE + 0x12*4)
#define PICADR               SFR_RW (SFR4_BASE + 0x13*4)
#define PICPND               SFR_RW (SFR4_BASE + 0x14*4)

//------------------------- SFR Group5 ---------------------------------------//
#define IISCON0              SFR_RW (SFR5_BASE + 0x00*4)
#define IISBAUD              SFR_RW (SFR5_BASE + 0x01*4)
#define IISDMACNT            SFR_RW (SFR5_BASE + 0x02*4)
#define IISDMAOADR0          SFR_RW (SFR5_BASE + 0x03*4)
#define IISDMAOADR1          SFR_RW (SFR5_BASE + 0x04*4)
#define IISDMAIADR0          SFR_RW (SFR5_BASE + 0x05*4)
#define IISDMAIADR1          SFR_RW (SFR5_BASE + 0x06*4)
#define IICCON0              SFR_RW (SFR5_BASE + 0x07*4)
#define IICCON1              SFR_RW (SFR5_BASE + 0x08*4)
#define IICCMDA              SFR_RW (SFR5_BASE + 0x09*4)
#define IICDATA              SFR_RW (SFR5_BASE + 0x0a*4)
#define IICDMAADR            SFR_RW (SFR5_BASE + 0x0b*4)
#define IICDMACNT            SFR_RW (SFR5_BASE + 0x0c*4)
#define IICSSTS              SFR_RW (SFR5_BASE + 0x0d*4)

//------------------------- SFR Group6 ---------------------------------------//
#define GPIOASET        SFR_RW (SFR6_BASE + 0x00*4)
#define GPIOACLR        SFR_RW (SFR6_BASE + 0x01*4)
#define GPIOA           SFR_RW (SFR6_BASE + 0x02*4)
#define GPIOADIR        SFR_RW (SFR6_BASE + 0x03*4)
#define GPIOADE         SFR_RW (SFR6_BASE + 0x04*4)
#define GPIOAFEN        SFR_RW (SFR6_BASE + 0x05*4)
#define GPIOADRV        SFR_RW (SFR6_BASE + 0x06*4)
#define GPIOAPU         SFR_RW (SFR6_BASE + 0x07*4)
#define GPIOAPD         SFR_RW (SFR6_BASE + 0x08*4)
#define GPIOAPU200K     SFR_RW (SFR6_BASE + 0x09*4)
#define GPIOAPD200K     SFR_RW (SFR6_BASE + 0x0a*4)
#define GPIOAPU300      SFR_RW (SFR6_BASE + 0x0b*4)
#define GPIOAPD300      SFR_RW (SFR6_BASE + 0x0c*4)

#define GPIOBSET        SFR_RW (SFR6_BASE + 0x10*4)
#define GPIOBCLR        SFR_RW (SFR6_BASE + 0x11*4)
#define GPIOB           SFR_RW (SFR6_BASE + 0x12*4)
#define GPIOBDIR        SFR_RW (SFR6_BASE + 0x13*4)
#define GPIOBDE         SFR_RW (SFR6_BASE + 0x14*4)
#define GPIOBFEN        SFR_RW (SFR6_BASE + 0x15*4)
#define GPIOBDRV        SFR_RW (SFR6_BASE + 0x16*4)
#define GPIOBPU         SFR_RW (SFR6_BASE + 0x17*4)
#define GPIOBPD         SFR_RW (SFR6_BASE + 0x18*4)
#define GPIOBPU200K     SFR_RW (SFR6_BASE + 0x19*4)
#define GPIOBPD200K     SFR_RW (SFR6_BASE + 0x1a*4)
#define GPIOBPU300      SFR_RW (SFR6_BASE + 0x1b*4)
#define GPIOBPD300      SFR_RW (SFR6_BASE + 0x1c*4)

#define GPIOESET        SFR_RW (SFR6_BASE + 0x20*4)
#define GPIOECLR        SFR_RW (SFR6_BASE + 0x21*4)
#define GPIOE           SFR_RW (SFR6_BASE + 0x22*4)
#define GPIOEDIR        SFR_RW (SFR6_BASE + 0x23*4)
#define GPIOEDE         SFR_RW (SFR6_BASE + 0x24*4)
#define GPIOEFEN        SFR_RW (SFR6_BASE + 0x25*4)
#define GPIOEDRV        SFR_RW (SFR6_BASE + 0x26*4)
#define GPIOEPU         SFR_RW (SFR6_BASE + 0x27*4)
#define GPIOEPD         SFR_RW (SFR6_BASE + 0x28*4)
#define GPIOEPU200K     SFR_RW (SFR6_BASE + 0x29*4)
#define GPIOEPD200K     SFR_RW (SFR6_BASE + 0x2a*4)
#define GPIOEPU300      SFR_RW (SFR6_BASE + 0x2b*4)
#define GPIOEPD300      SFR_RW (SFR6_BASE + 0x2c*4)

#define GPIOFSET        SFR_RW (SFR6_BASE + 0x30*4)
#define GPIOFCLR        SFR_RW (SFR6_BASE + 0x31*4)
#define GPIOF           SFR_RW (SFR6_BASE + 0x32*4)
#define GPIOFDIR        SFR_RW (SFR6_BASE + 0x33*4)
#define GPIOFDE         SFR_RW (SFR6_BASE + 0x34*4)
#define GPIOFFEN        SFR_RW (SFR6_BASE + 0x35*4)
#define GPIOFDRV        SFR_RW (SFR6_BASE + 0x36*4)
#define GPIOFPU         SFR_RW (SFR6_BASE + 0x37*4)
#define GPIOFPD         SFR_RW (SFR6_BASE + 0x38*4)
#define GPIOFPU200K     SFR_RW (SFR6_BASE + 0x39*4)
#define GPIOFPD200K     SFR_RW (SFR6_BASE + 0x3a*4)
#define GPIOFPU300      SFR_RW (SFR6_BASE + 0x3b*4)
#define GPIOFPD300      SFR_RW (SFR6_BASE + 0x3c*4)

//------------------------- SFR Group7 ---------------------------------------//
#define GPIOGSET        SFR_RW (SFR7_BASE + 0x00*4)
#define GPIOGCLR        SFR_RW (SFR7_BASE + 0x01*4)
#define GPIOG           SFR_RW (SFR7_BASE + 0x02*4)
#define GPIOGDIR        SFR_RW (SFR7_BASE + 0x03*4)
#define GPIOGDE         SFR_RW (SFR7_BASE + 0x04*4)
#define GPIOGFEN        SFR_RW (SFR7_BASE + 0x05*4)
#define GPIOGDRV        SFR_RW (SFR7_BASE + 0x06*4)
#define GPIOGPU         SFR_RW (SFR7_BASE + 0x07*4)
#define GPIOGPD         SFR_RW (SFR7_BASE + 0x08*4)
#define GPIOGPU200K     SFR_RW (SFR7_BASE + 0x09*4)
#define GPIOGPD200K     SFR_RW (SFR7_BASE + 0x0a*4)
#define GPIOGPU300      SFR_RW (SFR7_BASE + 0x0b*4)
#define GPIOGPD300      SFR_RW (SFR7_BASE + 0x0c*4)

//------------------------- SFR Group9 ---------------------------------------//
#define IRRXCON              SFR_RW (SFR9_BASE + 0x00*4)
#define IRRXDAT              SFR_RW (SFR9_BASE + 0x01*4)
#define IRRXCPND             SFR_RW (SFR9_BASE + 0x02*4)
#define IRRXERR0             SFR_RW (SFR9_BASE + 0x03*4)
#define IRRXERR1             SFR_RW (SFR9_BASE + 0x04*4)
#define IRRXPR0              SFR_RW (SFR9_BASE + 0x05*4)
#define IRRXPR1              SFR_RW (SFR9_BASE + 0x06*4)

//------------------------- SFR Group11 ---------------------------------------//
#define RTCCON0              SFR_RW (SFR11_BASE + 0x00*4)
#define RTCCON1              SFR_RW (SFR11_BASE + 0x01*4)
#define RTCCON2              SFR_RW (SFR11_BASE + 0x02*4)
#define RTCCON3              SFR_RW (SFR11_BASE + 0x03*4)
#define RTCCON4              SFR_RW (SFR11_BASE + 0x04*4)
#define RTCCON5              SFR_RW (SFR11_BASE + 0x05*4)
#define RTCCON6              SFR_RW (SFR11_BASE + 0x06*4)
#define RTCCON7              SFR_RW (SFR11_BASE + 0x07*4)
#define RTCCON8              SFR_RW (SFR11_BASE + 0x08*4)
#define RTCCON9              SFR_RW (SFR11_BASE + 0x09*4)
#define RTCCON10             SFR_RW (SFR11_BASE + 0x0a*4)
#define RTCCON11             SFR_RW (SFR11_BASE + 0x0b*4)
#define RTCCON12             SFR_RW (SFR11_BASE + 0x0c*4)
#define RTCCON13             SFR_RW (SFR11_BASE + 0x0d*4)
#define RTCCON14             SFR_RW (SFR11_BASE + 0x0e*4)
#define RTCCON15             SFR_RW (SFR11_BASE + 0x0f*4)
#define RTCRAMADR            SFR_RW (SFR11_BASE + 0x10*4)
#define RTCRAMDAT            SFR_RW (SFR11_BASE + 0x11*4)
#define RTCALM               SFR_RW (SFR11_BASE + 0x12*4)
#define RTCCNT               SFR_RW (SFR11_BASE + 0x13*4)

#define TKRBCNT              SFR_RW (SFR11_BASE + 0x20*4)
#define TK1CNT               SFR_RW (SFR11_BASE + 0x21*4)
#define TK2CNT               SFR_RW (SFR11_BASE + 0x22*4)
#define TK3CNT               SFR_RW (SFR11_BASE + 0x23*4)
#define TK4CNT               SFR_RW (SFR11_BASE + 0x24*4)
#define TK5CNT               SFR_RW (SFR11_BASE + 0x25*4)
#define TKCDPR2              SFR_RW (SFR11_BASE + 0x26*4)
#define TKVARI               SFR_RW (SFR11_BASE + 0x27*4)
#define TKVARITHD            SFR_RW (SFR11_BASE + 0x28*4)
#define TKACON0              SFR_RW (SFR11_BASE + 0x29*4)
#define TKACON1              SFR_RW (SFR11_BASE + 0x2a*4)
#define TKIE                 SFR_RW (SFR11_BASE + 0x2b*4)
#define TKCPND               SFR_RW (SFR11_BASE + 0x2c*4)
#define TKCON                SFR_RW (SFR11_BASE + 0x2d*4)
#define TKCON1               SFR_RW (SFR11_BASE + 0x2e*4)
#define TK0CNT               SFR_RW (SFR11_BASE + 0x2f*4)
#define TKCDPR0              SFR_RW (SFR11_BASE + 0x30*4)
#define TKCDPR1              SFR_RW (SFR11_BASE + 0x31*4)
#define TKTMR                SFR_RW (SFR11_BASE + 0x32*4)
#define TETMR                SFR_RW (SFR11_BASE + 0x33*4)
#define TKBCNT               SFR_RW (SFR11_BASE + 0x34*4)
#define TKPTHD               SFR_RW (SFR11_BASE + 0x35*4)
#define TKETHD               SFR_RW (SFR11_BASE + 0x36*4)
#define TEBCNT               SFR_RW (SFR11_BASE + 0x37*4)
#define TEPTHD               SFR_RW (SFR11_BASE + 0x38*4)
#define TEETHD               SFR_RW (SFR11_BASE + 0x39*4)
#define TKCON2               SFR_RW (SFR11_BASE + 0x3a*4)
#define TKVARIRP             SFR_RW (SFR11_BASE + 0x3b*4)
#define TKDLYPR              SFR_RW (SFR11_BASE + 0x3c*4)

//------------------------- SFR Group12 ---------------------------------------//
#define AUBUF1CON            SFR_RW (SFR12_BASE + 0x0f*4)
#define AUBUF1DATA           SFR_RW (SFR12_BASE + 0x10*4)

//------------------------- SFR Group13 ---------------------------------------//
#define DACDIGCON0           SFR_RW (SFR13_BASE + 0x00*4)
#define DACDIGCON1           SFR_RW (SFR13_BASE + 0x01*4)
#define DACDIGCON2           SFR_RW (SFR13_BASE + 0x02*4)
#define DACDIGCON3           SFR_RW (SFR13_BASE + 0x03*4)
#define DACVOLCON            SFR_RW (SFR13_BASE + 0x04*4)
#define AU0LMIXCOEF          SFR_RW (SFR13_BASE + 0x05*4)
#define AU0RMIXCOEF          SFR_RW (SFR13_BASE + 0x06*4)
#define AU1LMIXCOEF          SFR_RW (SFR13_BASE + 0x07*4)
#define AU1RMIXCOEF          SFR_RW (SFR13_BASE + 0x08*4)
#define SRC0VOLCON           SFR_RW (SFR13_BASE + 0x0b*4)
#define SRC1VOLCON           SFR_RW (SFR13_BASE + 0x0c*4)

#define SDADCDMACON          SFR_RW (SFR14_BASE + 0x00*4)
#define SDADCDMAFLAG         SFR_RW (SFR14_BASE + 0x01*4)
#define SDADCDMACLR          SFR_RW (SFR14_BASE + 0x02*4)
#define VADDMACON            SFR_RW (SFR14_BASE + 0x03*4)
#define SDADC0DMACON         SFR_RW (SFR14_BASE + 0x04*4)
#define VADCON0              SFR_RW (SFR14_BASE + 0x2e*4)
#define VADCON1              SFR_RW (SFR14_BASE + 0x2f*4)
#define VADCON2              SFR_RW (SFR14_BASE + 0x30*4)

#ifndef __ASSEMBLER__
enum funo_select_tbl {
    FO_T5PWM0              = 0,
    FO_T5PWM1,
    FO_T5PWM2,
    FO_T5PWM3,
    FO_UR0TX,
    FO_HUR0TX,
    FO_UR1TX,
    FO_I2CSCL,
    FO_I2CSDA,
    FO_CLKOUT,
    FO_SPI1D0,
    FO_SPI1D1,
    FO_SPI1CLK,
    FO_UR2TX,
};

enum funo_io_tbl {
    FO_PA4              = 1,
    FO_PA5,
    FO_PA6,
    FO_PA7,
    FO_PB0              = 5,
    FO_PB1,
    FO_PB2,
    FO_PB3,
    FO_PB4,
    FO_PB5,
    FO_PE0              = 11,
    FO_PE4,
    FO_PE5,
    FO_PE6,
    FO_PE7,
    FO_PF0              = 16,
    FO_PF1,
};

enum funi_io_tbl {
    FI_PA4              = 0,
    FI_PA5,
    FI_PA6,
    FI_PA7,
    FI_PB0              = 4,
    FI_PB1,
    FI_PB2,
    FI_PB3,
    FI_PB4,
    FI_PB5,
    FI_PE0              = 10,
    FI_PE4,
    FI_PE5,
    FI_PE6,
    FI_PE7,
    FI_PF0              = 15,
    FI_PF1,
    FI_PG0              = 17,
    FI_PG1,
    FI_PG2,
    FI_PG3,
    FI_PG4,
    FI_PG5,
};
#endif

//channel output function select
#define CH0_FUNO_SEL(ch0_funo_sel) FUNCOUTCON = (ch0_funo_sel << 0)
#define CH1_FUNO_SEL(ch1_funo_sel) FUNCOUTCON = (ch1_funo_sel << 8)
#define CH2_FUNO_SEL(ch2_funo_sel) FUNCOUTCON = (ch2_funo_sel <<16)
#define CH3_FUNO_SEL(ch3_funo_sel) FUNCOUTCON = (ch3_funo_sel <<24)

//channel 0 output mapping
#define CH0_FUNO_PA4MAP            FUNCOUTMCON = ( 1 << 0)
#define CH0_FUNO_PA5MAP            FUNCOUTMCON = ( 2 << 0)
#define CH0_FUNO_PA6MAP            FUNCOUTMCON = ( 3 << 0)
#define CH0_FUNO_PA7MAP            FUNCOUTMCON = ( 4 << 0)

#define CH0_FUNO_PB0MAP            FUNCOUTMCON = ( 5 << 0)
#define CH0_FUNO_PB1MAP            FUNCOUTMCON = ( 6 << 0)
#define CH0_FUNO_PB2MAP            FUNCOUTMCON = ( 7 << 0)
#define CH0_FUNO_PB3MAP            FUNCOUTMCON = ( 8 << 0)
#define CH0_FUNO_PB4MAP            FUNCOUTMCON = ( 9 << 0)
#define CH0_FUNO_PB5MAP            FUNCOUTMCON = (10 << 0)

#define CH0_FUNO_PE0MAP            FUNCOUTMCON = (11 << 0)
#define CH0_FUNO_PE4MAP            FUNCOUTMCON = (12 << 0)
#define CH0_FUNO_PE5MAP            FUNCOUTMCON = (13 << 0)
#define CH0_FUNO_PE6MAP            FUNCOUTMCON = (14 << 0)
#define CH0_FUNO_PE7MAP            FUNCOUTMCON = (15 << 0)

#define CH0_FUNO_PF0MAP            FUNCOUTMCON = (16 << 0)
#define CH0_FUNO_PF1MAP            FUNCOUTMCON = (17 << 0)

//channel 1 output mapping
#define CH1_FUNO_PA4MAP            FUNCOUTMCON = ( 1 << 8)
#define CH1_FUNO_PA5MAP            FUNCOUTMCON = ( 2 << 8)
#define CH1_FUNO_PA6MAP            FUNCOUTMCON = ( 3 << 8)
#define CH1_FUNO_PA7MAP            FUNCOUTMCON = ( 4 << 8)

#define CH1_FUNO_PB0MAP            FUNCOUTMCON = ( 5 << 8)
#define CH1_FUNO_PB1MAP            FUNCOUTMCON = ( 6 << 8)
#define CH1_FUNO_PB2MAP            FUNCOUTMCON = ( 7 << 8)
#define CH1_FUNO_PB3MAP            FUNCOUTMCON = ( 8 << 8)
#define CH1_FUNO_PB4MAP            FUNCOUTMCON = ( 9 << 8)
#define CH1_FUNO_PB5MAP            FUNCOUTMCON = (10 << 8)

#define CH1_FUNO_PE0MAP            FUNCOUTMCON = (11 << 8)
#define CH1_FUNO_PE4MAP            FUNCOUTMCON = (12 << 8)
#define CH1_FUNO_PE5MAP            FUNCOUTMCON = (13 << 8)
#define CH1_FUNO_PE6MAP            FUNCOUTMCON = (14 << 8)
#define CH1_FUNO_PE7MAP            FUNCOUTMCON = (15 << 8)

#define CH1_FUNO_PF0MAP            FUNCOUTMCON = (16 << 8)
#define CH1_FUNO_PF1MAP            FUNCOUTMCON = (17 << 8)

//channel 2 output mapping
#define CH2_FUNO_PA4MAP            FUNCOUTMCON = ( 1 <<16)
#define CH2_FUNO_PA5MAP            FUNCOUTMCON = ( 2 <<16)
#define CH2_FUNO_PA6MAP            FUNCOUTMCON = ( 3 <<16)
#define CH2_FUNO_PA7MAP            FUNCOUTMCON = ( 4 <<16)

#define CH2_FUNO_PB0MAP            FUNCOUTMCON = ( 5 <<16)
#define CH2_FUNO_PB1MAP            FUNCOUTMCON = ( 6 <<16)
#define CH2_FUNO_PB2MAP            FUNCOUTMCON = ( 7 <<16)
#define CH2_FUNO_PB3MAP            FUNCOUTMCON = ( 8 <<16)
#define CH2_FUNO_PB4MAP            FUNCOUTMCON = ( 9 <<16)
#define CH2_FUNO_PB5MAP            FUNCOUTMCON = (10 <<16)

#define CH2_FUNO_PE0MAP            FUNCOUTMCON = (11 <<16)
#define CH2_FUNO_PE4MAP            FUNCOUTMCON = (12 <<16)
#define CH2_FUNO_PE5MAP            FUNCOUTMCON = (13 <<16)
#define CH2_FUNO_PE6MAP            FUNCOUTMCON = (14 <<16)
#define CH2_FUNO_PE7MAP            FUNCOUTMCON = (15 <<16)

#define CH2_FUNO_PF0MAP            FUNCOUTMCON = (16 <<16)
#define CH2_FUNO_PF1MAP            FUNCOUTMCON = (17 <<16)

//channel 3 output mapping
#define CH3_FUNO_PA4MAP            FUNCOUTMCON = ( 1 <<24)
#define CH3_FUNO_PA5MAP            FUNCOUTMCON = ( 2 <<24)
#define CH3_FUNO_PA6MAP            FUNCOUTMCON = ( 3 <<24)
#define CH3_FUNO_PA7MAP            FUNCOUTMCON = ( 4 <<24)

#define CH3_FUNO_PB0MAP            FUNCOUTMCON = ( 5 <<24)
#define CH3_FUNO_PB1MAP            FUNCOUTMCON = ( 6 <<24)
#define CH3_FUNO_PB2MAP            FUNCOUTMCON = ( 7 <<24)
#define CH3_FUNO_PB3MAP            FUNCOUTMCON = ( 8 <<24)
#define CH3_FUNO_PB4MAP            FUNCOUTMCON = ( 9 <<24)
#define CH3_FUNO_PB5MAP            FUNCOUTMCON = (10 <<24)

#define CH3_FUNO_PE0MAP            FUNCOUTMCON = (11 <<24)
#define CH3_FUNO_PE4MAP            FUNCOUTMCON = (12 <<24)
#define CH3_FUNO_PE5MAP            FUNCOUTMCON = (13 <<24)
#define CH3_FUNO_PE6MAP            FUNCOUTMCON = (14 <<24)
#define CH3_FUNO_PE7MAP            FUNCOUTMCON = (15 <<24)

#define CH3_FUNO_PF0MAP            FUNCOUTMCON = (16 <<24)
#define CH3_FUNO_PF1MAP            FUNCOUTMCON = (17 <<24)

//channel input function select
#define CH0_FUNI_SEL(ch0_funi_sel) FUNCINCON  = (ch0_funi_sel << 0)
#define CH1_FUNI_SEL(ch1_funi_sel) FUNCINCON  = (ch1_funi_sel << 8)
#define CH2_FUNI_SEL(ch2_funi_sel) FUNCINCON  = (ch2_funi_sel <<16)
#define CH3_FUNI_SEL(ch3_funi_sel) FUNCINCON  = (ch3_funi_sel <<24)

#endif
