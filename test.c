#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>  // for usleep

long current_time_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000L + t.tv_usec / 1000;
}

int main(void) {
    long origin = current_time_ms();  // ⏱️ Reference start time
    long next_tick = origin + 100;

    while (1) {
        long now;
        do {
            usleep(100);  // Micro-sleep to reduce CPU usage
            now = current_time_ms();
        } while (now < next_tick);  // Wait until exact 100 ms has passed

        printf("Timestamp: %ld ms since the Epoch\n", next_tick - origin);

        next_tick += 100;  // Schedule next timestamp exactly 100 ms later
    }

    return 0;
}
