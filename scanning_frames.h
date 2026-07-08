#ifndef SCANNING_FRAMES_H
#define SCANNING_FRAMES_H

#include <stdint.h>

// ===== FULL RESOLUTION FRAMES (160x128) =====
#include "scanning_gif2/1.h"
#include "scanning_gif2/2.h"
#include "scanning_gif2/3.h"
#include "scanning_gif2/4.h"
#include "scanning_gif2/5.h"
#include "scanning_gif2/6.h"
#include "scanning_gif2/7.h"
#include "scanning_gif2/8.h"
#include "scanning_gif2/9.h"
#include "scanning_gif2/10.h"
#include "scanning_gif2/11.h"
#include "scanning_gif2/12.h"
#include "scanning_gif2/13.h"
#include "scanning_gif2/14.h"
#include "scanning_gif2/15.h"

// pointer array
static const uint16_t* const scanningFrames[] = {
  _1,_2,_3,_4,_5,
  _6,_7,_8,_9,_10,
  _11,_12,_13,_14,_15
};

#define SCAN_FRAME_COUNT 15

#define SCAN_WIDTH 160
#define SCAN_HEIGHT 128

#endif
