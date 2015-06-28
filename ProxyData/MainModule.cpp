
#include "proxyArduino.h"

int32_t main(int32_t argc, char **argv) {
    msv::proxyArduino p(argc, argv);
    return p.runModule();
}
