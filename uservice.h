enum init_system {
    SYSTEMD,
    OPENRC,
    NUMBER_OF_SUPPORTED_INIT_SYSTEMS,
    UNSUPPORTED_INIT_SYSTEM,
};

static const char* init_system_name[] = {"systemd", "openRC", "INVALID", "INVALID"};
