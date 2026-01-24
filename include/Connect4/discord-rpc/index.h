#pragma once

void C4_Discord_Init(void);
void C4_Discord_UpdateStatus(const char* stateText, const char* detailsText);
void C4_Discord_Loop(void);
void C4_Discord_Shutdown(void);