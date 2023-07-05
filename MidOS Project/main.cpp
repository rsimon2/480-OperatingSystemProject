#include "system.h"
//for the main we are making sure we have the right amount of arguments in the command line and creating threads for the files and controlling access to critical section

int main(int argc, char *argv[]) {
//this checks to see that we arguements in command line
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <memory size> <programs>" << endl;
        exit(1);
    }
//this makes sure that multiple threads do not access the critical section at the same time
    sem_init(&critical, 0, 1);
//this creates threads for files
    int arguments = argc - 2;
    pthread_t *threads = new pthread_t[arguments];
    for (int i = 2; i < argc; i++) {
        pthread_create(&threads[i - 2], NULL, runFile, (void *)argv[i]);
        sleep(1);
    }
//makes sure threads execute to completion 
    for (int i = 2; i < argc; i++) {
        pthread_join(threads[i - 2], NULL);
    }
    return 0;
}