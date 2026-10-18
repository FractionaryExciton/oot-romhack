#ifndef SPEEDMETER_H
#define SPEEDMETER_H

#include "ultra64.h"

struct GraphicsContext;
struct GameState;

typedef struct SpeedMeter {
    /* 0x00 */ char unk_00[0x18];
    /* 0x18 */ s32 x; // Unused
    /* 0x1C */ s32 y;
} SpeedMeter; // size = 0x20

extern volatile OSTime gAudioThreadUpdateTimeTotalPerGfxTask;
extern volatile OSTime gGfxTaskSentToNextReadyMinusAudioThreadUpdateTime;
extern volatile OSTime gRSPAudioTimeTotal;
extern volatile OSTime gRSPGfxTimeTotal;
extern volatile OSTime gRDPTimeTotal;
extern volatile OSTime gGraphUpdatePeriod;
extern volatile OSTime gAudioThreadUpdateTimeStart;
extern volatile OSTime gAudioThreadUpdateTimeAcc;
extern volatile OSTime gRSPAudioTimeAcc;
extern volatile OSTime gRSPGfxTimeAcc;
extern volatile OSTime gRSPOtherTimeAcc;
extern volatile OSTime D_8016A578;
extern volatile OSTime gRDPTimeAcc;

void SpeedMeter_Init(SpeedMeter* this);
void SpeedMeter_Destroy(SpeedMeter* this);
void SpeedMeter_DrawTimeEntries(SpeedMeter* this, struct GraphicsContext* gfxCtx);
void SpeedMeter_DrawAllocEntries(SpeedMeter* meter, struct GraphicsContext* gfxCtx, struct GameState* state);
// START: MODIFIED
void SpeedMeter_DrawDigit(struct GraphicsContext* gfxCtx, s32 digit, s32 rectLeft, s32 rectTop);
void SpeedMeter_DrawMicroseconds(struct GraphicsContext* gfxCtx, u32 usec, s32 x, s32 y);
// END: MODIFIED; addded function declarations

#endif
