#ifndef __IO_DEF_H
#define __IO_DEF_H


#include "global.h"
#include "config.h"
#include "sfr.h"


#if (SD0_MAPPING == SD0MAP_G1)
    //SDCMD(PA5),  SDCLK(PA6),  SDDAT0(PA7)
    #define SD0CMD_GP               A
    #define SD0CLK_GP               A
    #define SD0DAT_GP               A
    #define SD0CMD_BIT              BIT(5)
    #define SD0CLK_BIT              BIT(6)
    #define SD0DAT_BIT              BIT(7)
    #define SD0_FCON_INIT()         FUNCMCON0 = SD0MAP_G1

    #define SDCMD_IO                IO_PA5
    #define SDCLK_IO                IO_PA6
    #define SDDAT_IO                IO_PA7

#elif (SD0_MAPPING == SD0MAP_G2)
    //SDCMD(PB0), SDCLK(PB1), SDDAT0(PB2)
    #define SD0CMD_GP               B
    #define SD0CLK_GP               B
    #define SD0DAT_GP               B
    #define SD0CMD_BIT              BIT(0)
    #define SD0CLK_BIT              BIT(1)
    #define SD0DAT_BIT              BIT(2)
    #define SD0_FCON_INIT()         FUNCMCON0 = SD0MAP_G2

    #define SDCMD_IO                IO_PB0
    #define SDCLK_IO                IO_PB1
    #define SDDAT_IO                IO_PB2

#elif (SD0_MAPPING == SD0MAP_G3)
    //SDCMD(PE5),  SDCLK(PE6),  SDDAT0(PE7)
    #define SD0CMD_GP               E
    #define SD0CLK_GP               E
    #define SD0DAT_GP               E
    #define SD0CMD_BIT              BIT(5)
    #define SD0CLK_BIT              BIT(6)
    #define SD0DAT_BIT              BIT(7)
    #define SD0_FCON_INIT()         FUNCMCON0 = SD0MAP_G3

    #define SDCMD_IO                IO_PB13
    #define SDCLK_IO                IO_PB14
    #define SDDAT_IO                IO_PB15

#elif (SD0_MAPPING == SD0MAP_G4)
    //SDCMD(PA5),  SDCLK(PA6),  SDDAT0(PB4)
    #define SD0CMD_GP               A
    #define SD0CLK_GP               A
    #define SD0DAT_GP               B
    #define SD0CMD_BIT              BIT(5)
    #define SD0CLK_BIT              BIT(6)
    #define SD0DAT_BIT              BIT(4)
    #define SD0_FCON_INIT()         FUNCMCON0 = SD0MAP_G4

    #define SDCMD_IO                IO_PE11
    #define SDCLK_IO                IO_PE12
    #define SDDAT_IO                IO_PE13

#elif (SD0_MAPPING == SD0MAP_G5)
    //SDCMD(PA5),  SDCLK(PA6),  SDDAT0(PB3)
    #define SD0CMD_GP               A
    #define SD0CLK_GP               A
    #define SD0DAT_GP               B
    #define SD0CMD_BIT              BIT(5)
    #define SD0CLK_BIT              BIT(6)
    #define SD0DAT_BIT              BIT(3)
    #define SD0_FCON_INIT()         FUNCMCON0 = SD0MAP_G5

    #define SDCMD_IO                IO_PE11
    #define SDCLK_IO                IO_PE12
    #define SDDAT_IO                IO_PB3

#elif (SD0_MAPPING == SD0MAP_G6)
    //SDCMD(PB3),  SDCLK(PA6),  SDDAT0(PB4)
    #define SD0CMD_GP               B
    #define SD0CLK_GP               A
    #define SD0DAT_GP               B
    #define SD0CMD_BIT              BIT(3)
    #define SD0CLK_BIT              BIT(6)
    #define SD0DAT_BIT              BIT(6)
    #define SD0_FCON_INIT()         FUNCMCON0 = SD0MAP_G6

    #define SDCMD_IO                IO_PE11
    #define SDCLK_IO                IO_PE12
    #define SDDAT_IO                IO_PB4

#elif (SD0_MAPPING == SD0MAP_G7)
    #error "SD0MAP_G7 is not allowed!"
    //SDCMD(PB3), SDCLK(PE12), SDDAT0(PB4)
    #define SD0CMD_GP               B
    #define SD0CLK_GP               E
    #define SD0DAT_GP               B
    #define SD0CMD_BIT              BIT(11)
    #define SD0CLK_BIT              BIT(12)
    #define SD0DAT_BIT              BIT(6)
    #define SD0_FCON_INIT()         FUNCMCON0 = SD0MAP_G7

    #define SDCMD_IO                IO_PB3
    #define SDCLK_IO                IO_PE12
    #define SDDAT_IO                IO_PB4

#endif

#if (SD0_MAPPING >= SD0MAP_G1 && SD0_MAPPING <= SD0MAP_G7)
    #define SD0CMD_GPIODE           SET_MACRO(GPIO, SET_MACRO(SD0CMD_GP, DE))
    #define SD0CMD_GPIOFEN          SET_MACRO(GPIO, SET_MACRO(SD0CMD_GP, FEN))
    #define SD0CMD_GPIODIR          SET_MACRO(GPIO, SET_MACRO(SD0CMD_GP, DIR))
    #define SD0CMD_GPIOSET          SET_MACRO(GPIO, SET_MACRO(SD0CMD_GP, SET))
    #define SD0CMD_GPIOCLR          SET_MACRO(GPIO, SET_MACRO(SD0CMD_GP, CLR))
    #define SD0CMD_GPIOPU           SET_MACRO(GPIO, SET_MACRO(SD0CMD_GP, PU))
    #define SD0CMD_GPIOPU300        SET_MACRO(GPIO, SET_MACRO(SD0CMD_GP, PU300))
    #define SD0CMD_GPIO             SET_MACRO(GPIO, SD0CMD_GP)

    #define SD0CLK_GPIODE           SET_MACRO(GPIO, SET_MACRO(SD0CLK_GP, DE))
    #define SD0CLK_GPIOFEN          SET_MACRO(GPIO, SET_MACRO(SD0CLK_GP, FEN))
    #define SD0CLK_GPIODIR          SET_MACRO(GPIO, SET_MACRO(SD0CLK_GP, DIR))
    #define SD0CLK_GPIOSET          SET_MACRO(GPIO, SET_MACRO(SD0CLK_GP, SET))
    #define SD0CLK_GPIOCLR          SET_MACRO(GPIO, SET_MACRO(SD0CLK_GP, CLR))
    #define SD0CLK_GPIOPU           SET_MACRO(GPIO, SET_MACRO(SD0CLK_GP, PU))
    #define SD0CLK_GPIOPU300        SET_MACRO(GPIO, SET_MACRO(SD0CLK_GP, PU300))
    #define SD0CLK_GPIO             SET_MACRO(GPIO, SD0CLK_GP)

    #define SD0DAT_GPIODE           SET_MACRO(GPIO, SET_MACRO(SD0DAT_GP, DE))
    #define SD0DAT_GPIOFEN          SET_MACRO(GPIO, SET_MACRO(SD0DAT_GP, FEN))
    #define SD0DAT_GPIODIR          SET_MACRO(GPIO, SET_MACRO(SD0DAT_GP, DIR))
    #define SD0DAT_GPIOSET          SET_MACRO(GPIO, SET_MACRO(SD0DAT_GP, SET))
    #define SD0DAT_GPIOCLR          SET_MACRO(GPIO, SET_MACRO(SD0DAT_GP, CLR))
    #define SD0DAT_GPIOPU           SET_MACRO(GPIO, SET_MACRO(SD0DAT_GP, PU))
    #define SD0DAT_GPIOPU300        SET_MACRO(GPIO, SET_MACRO(SD0DAT_GP, PU300))
    #define SD0DAT_GPIO             SET_MACRO(GPIO, SD0DAT_GP)

    #define SD_MUX_IO_INIT()        {SD0CLK_GPIODE |= SD0CLK_BIT;\
                                    SD0CMD_GPIODE  |= SD0CMD_BIT;\
                                    SD0DAT_GPIODE  |= SD0DAT_BIT;\
                                    SD0CLK_GPIOCLR = SD0CLK_BIT;\
                                    SD0CLK_GPIODIR &= ~SD0CLK_BIT;\
                                    SD0CLK_GPIOPU  &= ~SD0CLK_BIT;\
                                    SD0CLK_GPIOFEN |= SD0CLK_BIT;\
                                    SD0CMD_GPIODIR |= SD0CMD_BIT;\
                                    SD0CMD_GPIOPU  |= SD0CMD_BIT;\
                                    SD0CMD_GPIOFEN |= SD0CMD_BIT;\
                                    SD0DAT_GPIODIR |= SD0DAT_BIT;\
                                    SD0DAT_GPIOPU  |= SD0DAT_BIT;\
                                    SD0DAT_GPIOFEN |= SD0DAT_BIT;\
                                    SD0_FCON_INIT();}
    #define SD_IO_INIT()            {SD0CLK_GPIODE |= SD0CLK_BIT;\
                                    SD0CMD_GPIODE  |= SD0CMD_BIT;\
                                    SD0DAT_GPIODE  |= SD0DAT_BIT;\
                                    SD0CLK_GPIODIR &= ~SD0CLK_BIT;\
                                    SD0CLK_GPIOFEN |= SD0CLK_BIT;\
                                    SD0CMD_GPIODIR |= SD0CMD_BIT;\
                                    SD0CMD_GPIOPU  |= SD0CMD_BIT;\
                                    SD0CMD_GPIOFEN |= SD0CMD_BIT;\
                                    SD0DAT_GPIODIR |= SD0DAT_BIT;\
                                    SD0DAT_GPIOPU  |= SD0DAT_BIT;\
                                    SD0DAT_GPIOFEN |= SD0DAT_BIT;\
                                    SD0_FCON_INIT();}
    #define SD_CLK_DIR_IN()         {SD0CLK_GPIODIR |= SD0CLK_BIT;  SD0CLK_GPIOPU  |= SD0CLK_BIT;}
    #define SD_CLK_IN_DIS_PU10K()   {SD0CLK_GPIODIR |= SD0CLK_BIT;  SD0CLK_GPIOPU  &= ~SD0CLK_BIT;}
    #define SD_CLK_DIR_OUT()        {SD0CLK_GPIOPU  &= ~SD0CLK_BIT; SD0CLK_GPIODIR &= ~SD0CLK_BIT;}
    #define SD_MUX_DETECT_INIT()    {SD0CLK_GPIODE  |= SD0CLK_BIT;  SD0CLK_GPIOPU  |= SD0CLK_BIT;  SD0CLK_GPIODIR |= SD0CLK_BIT;}
    #define SD_MUX_IS_ONLINE()      ((SD0CLK_GPIO & SD0CLK_BIT) == 0)
    #define SD_MUX_IS_BUSY()        ((SD0CLK_GPIODIR & SD0CLK_BIT) == 0)
    #define SD_MUX_CMD_IS_BUSY()    (SD0CMD_GPIOPU300 & SD0CMD_BIT)
    #define SD_CMD_MUX_PU300R()     {SD0CMD_GPIOPU300 |= SD0CMD_BIT; SD0CMD_GPIOPU    &= ~SD0CMD_BIT;}
    #define SD_CMD_MUX_PU10K()      {SD0CMD_GPIOPU    |= SD0CMD_BIT; SD0CMD_GPIOPU300 &= ~SD0CMD_BIT;}
    #define SD_DAT_MUX_PU300R()     {SD0DAT_GPIOPU300 |= SD0DAT_BIT; SD0DAT_GPIOPU    &= ~SD0DAT_BIT;}
    #define SD_DAT_MUX_PU10K()      {SD0DAT_GPIOPU    |= SD0DAT_BIT; SD0DAT_GPIOPU300 &= ~SD0DAT_BIT;}
    #define SD_CMD_MUX_IS_ONLINE()  ((SD0CMD_GPIO & SD0CMD_BIT) == 0)


    #define SD_CLK_OUT_H()          {SD0CLK_GPIOSET = SD0CLK_BIT;}
    #define SD_CLK_OUT_L()          {SD0CLK_GPIOCLR = SD0CLK_BIT;}
    #define SD_CLK_STA()            (SD0CLK_GPIO & SD0CLK_BIT)

    #define SD_DAT_DIR_OUT()        {SD0DAT_GPIODE  |= SD0DAT_BIT;   SD0DAT_GPIODIR &= ~SD0DAT_BIT;}
    #define SD_DAT_DIR_IN()         {SD0DAT_GPIODIR |= SD0DAT_BIT;   SD0DAT_GPIOPU  |= SD0DAT_BIT;}
    #define SD_DAT_OUT_H()          {SD0DAT_GPIOSET = SD0DAT_BIT;}
    #define SD_DAT_OUT_L()          {SD0DAT_GPIOCLR = SD0DAT_BIT;}
    #define SD_DAT_STA()            (SD0DAT_GPIO & SD0DAT_BIT)

    #define SD_CMD_DIR_OUT()        {SD0CMD_GPIODE  |= SD0CMD_BIT;   SD0CMD_GPIODIR &= ~SD0CMD_BIT;}
    #define SD_CMD_DIR_IN()         {SD0CMD_GPIODIR |= SD0CMD_BIT;   SD0CMD_GPIOPU  |= SD0CMD_BIT;}
    #define SD_CMD_OUT_H()          {SD0CMD_GPIOSET = SD0CMD_BIT;}
    #define SD_CMD_OUT_L()          {SD0CMD_GPIOCLR = SD0CMD_BIT;}
    #define SD_CMD_STA()            (SD0CMD_GPIO & SD0CMD_BIT)

	#define SD_DAT_DIS_UP() 		static u32 pu300, pu,dir;\
                                    pu300 = GPIOBPU300;\
                                    pu = SD0DAT_GPIOPU;\
                                    dir = SD0DAT_GPIODIR;\
                                    SD0DAT_GPIODIR |= BIT(3);\
                                    SD0DAT_GPIOPU300 &= ~BIT(3);\
                                    SD0DAT_GPIOPU &= ~BIT(3);
	#define SD_DAT_RES_UP() 		SD0DAT_GPIOPU300 = pu300;\
                                    SD0DAT_GPIOPU = pu;\
                                    SD0DAT_GPIODIR = dir;

#else
    #define SD_MUX_IO_INIT()
    #define SD_IO_INIT()
    #define SD_CLK_DIR_IN()
    #define SD_CLK_IN_DIS_PU10K()
    #define SD_CLK_DIR_OUT()
    #define SD_MUX_DETECT_INIT()
    #define SD_MUX_IS_ONLINE()      0
    #define SD_MUX_IS_BUSY()        0
    #define SD_MUX_CMD_IS_BUSY()    0
    #define SD_CMD_MUX_PU300R()
    #define SD_CMD_MUX_PU10K()
    #define SD_CMD_MUX_IS_ONLINE()  0

    #define SD_CLK_OUT_H()
    #define SD_CLK_OUT_L()
    #define SD_CLK_STA()            0

    #define SD_DAT_DIR_OUT()
    #define SD_DAT_DIR_IN()
    #define SD_DAT_OUT_H()
    #define SD_DAT_OUT_L()
    #define SD_DAT_STA()            0

    #define SD_CMD_DIR_OUT()
    #define SD_CMD_DIR_IN()
    #define SD_CMD_OUT_H()
    #define SD_CMD_OUT_L()
    #define SD_CMD_STA()            0

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_NONE
    #define SDCMD_IO                IO_NONE
    #define SDDAT_IO                IO_NONE
#endif


#endif //__IO_DEF_H
