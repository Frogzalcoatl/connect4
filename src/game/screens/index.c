#include "Connect4/game/screens/index.h"

void C4_Screen_UpdatePopup(C4_UI_Popup* popup, C4_Screen_UpdatePopup_Config* config) {
    if (!popup || !config || !config->renderer) {
        return;
    }
    popup->buttonGroup.bounds.h = config->buttonGroupHeight;
    C4_UI_ButtonGroup_ChangeDestination(&popup->buttonGroup, popup->buttonGroup.bounds);
    C4_UI_ButtonGroup_ChangeDirection(&popup->buttonGroup, config->direction);
    C4_UI_ButtonGroup_ChangePtSize(&popup->buttonGroup, config->buttonGroupPtSize, config->renderer);
    // Since the message texture is reloaded in the following func 
    popup->message.ptSize = config->messagePtSize;
    C4_UI_Popup_ChangeDestination(popup, config->destination, config->renderer);
}