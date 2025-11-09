enum init_system {
    SYSTEMD,
    OPENRC,
    NUMBER_OF_SUPPORTED_INIT_SYSTEMS,
    UNSUPPORTED_INIT_SYSTEM,
};

#ifdef DEBUG
    static char init_system_name_dbg[][16] = {"systemd", "openRC", "INVALID", "INVALID"};
#endif

static char init_system_name[][16] = {"systemd\n", "openRC\n", "INVALID\n", "INVALID\n"};
