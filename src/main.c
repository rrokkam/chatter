
/**
 * Start the chatter client.
 */
int main(int argc, char *argv[]) {
    int arg;
    while ((arg = getopt(argc, argv, "h")) != -1) {
        switch(arg) {
            case 'h':
                printf(USAGE_STRING);
                exit(EXIT_SUCCESS);
            default:  // getopt will print the invalid argument out.
                printf(USAGE_STRING);
                exit(EXIT_FAILURE);
        }
    }

    // arguments must be: programname username ip port
    if (argc != 4) {
        printf(USAGE_STRING);
        exit(EXIT_FAILURE);
    }

    char *name = argv[optind];
    char *server_ip = argv[optind + 1];
    int server_port = atoi(argv[optind + 2]);

    // len should be less than or equal to 10 chars, no space or rnrn
    int namelen = strlen(name);
    if (namelen > MAX_USERNAME_LENGTH || strcspn(name, ME2U_SPECIAL_CHARS) != namelen) {
        debug("Invalid username.");
        printf(USAGE_STRING);
        exit(EXIT_FAILURE);
    } 

    if (server_port < MIN_PORT) { // atoi gives 0 in error
        debug("Invalid port.");
        printf(USAGE_STRING);
        exit(EXIT_FAILURE);
    }

    struct sigaction action, oldaction;
    action.sa_handler = sigchld_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGCHLD, &action, &oldaction);

    run(name, server_ip, server_port);

    sigaction(SIGCHLD, &oldaction, NULL);
    exit(EXIT_SUCCESS);
}
