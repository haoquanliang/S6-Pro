#ifndef __MODULES_H
#define __MODULES_H

#include "key/key_scan.h"
#include "audio/audio.h"
#include "bluetooth/bluetooth.h"

#include "gui/gui.h"
#include "fs/fs.h"

#include "charge/charge.h"
#include "charge/charge_box.h"

#include "music/bsp_id3_tag.h"
#include "music/bsp_karaok.h"
#include "music/bsp_lrc.h"
#include "music/bsp_piano.h"
#include "music/bsp_music.h"
#include "music/bsp_abp.h"

#include "record/record.h"

#include "test/iodm.h"
#include "test/qtest.h"

#include "usb_device/usb_audio.h"
#include "usb_device/usb_enum.h"
#include "usb_device/usb_table.h"

#include "warning/warning_play.h"

#include "update/upd_uart.h"

#include "debug/offline_log.h"
#include "debug/debug.h"

#if LE_DUEROS_DMA_EN
#include "bluetooth/app/dueros_dma/dueros_dma_app.h"
#endif


#endif // __MODULES_H
