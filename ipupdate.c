#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define CHECK_INTERVAL 300 // Interval in seconds to check for updates

volatile sig_atomic_t keep_running = 1;

// Signal handler to stop the service gracefully
void handle_sigterm(int signum) {
    keep_running = 0;
}

void resolve_domain_to_ip(const char *domain, char *resolved_ip) {
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(domain, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // Loop through results and get the first IPv4 address
    for (p = res; p != NULL; p = p->ai_next) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
        strcpy(resolved_ip, ipstr);
        break;
    }

    freeaddrinfo(res); // Free the linked list
}

void update_ufw_firewall(const char *ip) {
    char command[256];

    // Construct the UFW command
    snprintf(command, sizeof(command), "ufw allow from %s", ip);

    // Execute the command
    int ret = system(command);

    if (ret != 0) {
        fprintf(stderr, "Failed to update UFW firewall rules.\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <domain>\n", argv[0]);
        return 1;
    }

    const char *domain = argv[1];
    char resolved_ip[INET_ADDRSTRLEN] = {0};
    char last_ip[INET_ADDRSTRLEN] = {0};

    // Set up signal handling for graceful termination
    signal(SIGTERM, handle_sigterm);
    signal(SIGINT, handle_sigterm);

    while (keep_running) {
        // Resolve domain to IP
        printf("Checking domain %s...\n", domain);
        resolve_domain_to_ip(domain, resolved_ip);

        // Update UFW only if the IP has changed
        if (strcmp(resolved_ip, last_ip) != 0) {
            printf("Resolved IP: %s (Updating UFW)\n", resolved_ip);
            update_ufw_firewall(resolved_ip);
            strcpy(last_ip, resolved_ip);
        } else {
            printf("Resolved IP: %s (No changes)\n", resolved_ip);
        }

        // Sleep for the interval or until termination signal
        for (int i = 0; i < CHECK_INTERVAL && keep_running; i++) {
            sleep(1);
        }
    }

    printf("Service stopping...\n");
    return 0;
}
