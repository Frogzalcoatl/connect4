// USE_DISCORD is from the CMakeLists file
#ifdef USE_DISCORD
    #include "Connect4/discord/index.h"
    #include "Discord/discord_rpc.h"
    #include "SDL3/SDL.h"
    #include <string.h>
    #include <time.h>

    static time_t startTime;
    static bool isConnected = false;
    static time_t lastConnectionAttempt = 0;
    static const int RECONNECT_INTERVAL = 10;

    static char cachedState[128] = "";
    static char cachedDetails[128] = "";

    static void handleDiscordReady(const DiscordUser* connectedUser) {
        (void)connectedUser;
        isConnected = true;
        C4_Discord_UpdateStatus(cachedState, cachedDetails);
    }

    static void handleDiscordDisconnected(int errorCode, const char* message) {
        (void)errorCode; (void)message;
        isConnected = false;
    }

    void C4_Discord_Init() {
        DiscordEventHandlers handlers;
        memset(&handlers, 0, sizeof(handlers));
        handlers.ready = handleDiscordReady;
        handlers.disconnected = handleDiscordDisconnected;
        handlers.errored = handleDiscordDisconnected;
        Discord_Initialize("1454580494376108218", &handlers, 1, NULL);
        startTime = time(NULL);
    }
    
    void C4_Discord_UpdateStatus(const char* state, const char* details) {
        DiscordRichPresence discordPresence;
        memset(&discordPresence, 0, sizeof(discordPresence));
        if (state) {
            // _TRUNCATE will automatically truncate string that are longer than 128 bytes with a /0 to prevent overflow
            strncpy_s(cachedState, sizeof(cachedState), state, _TRUNCATE);
        }
        if (details) {
            strncpy_s(cachedDetails, sizeof(cachedDetails), details, _TRUNCATE);
        }
        if (!isConnected) {
            return;
        }
        discordPresence.state = cachedState;
        discordPresence.details = cachedDetails;
        discordPresence.largeImageKey = "icon";
        discordPresence.largeImageText = "Connect4";
        discordPresence.startTimestamp = startTime;
        Discord_UpdatePresence(&discordPresence);
    }

    void C4_Discord_Loop() {
        time_t now = time(NULL);
        if (!isConnected && (now - lastConnectionAttempt) > RECONNECT_INTERVAL) {
            lastConnectionAttempt = now;
            C4_Discord_Init();
        }
        Discord_RunCallbacks();
    }

    void C4_Discord_Shutdown() {
        Discord_Shutdown();
    }
#else
    void C4_Discord_Init() {}
    // Below function is to avoid compiler warnings
    void C4_Discord_UpdateStatus(const char* stateText, const char* detailsText) { (void)stateText; (void)detailsText }
    void C4_Discord_Loop() {}
    void C4_Discord_Shutdown() {}
#endif