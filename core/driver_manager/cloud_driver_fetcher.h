#ifndef CLOUD_DRIVER_FETCHER_H
#define CLOUD_DRIVER_FETCHER_H
#include <stdint.h>
#include <stddef.h>
#include "../../drivers/unified_driver_framework/driver_framework.h"

// Fetch a driver from the cloud and cache it offline for the given hardware fingerprint
int cloud_fetch_driver(hw_fingerprint_t* fp);

#endif // CLOUD_DRIVER_FETCHER_H 