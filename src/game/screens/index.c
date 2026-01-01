#include "Connect4/game/screens/index.h"
#include "Connect4/game/events.h"

void C4_DefaultButtonHoverCallback(void* data) {
    (void)data;
    C4_PushEvent_SoundRequest(C4_SoundEffect_ButtonHover);
}