#ifndef _SCANNER_PAGE_H_
#define _SCANNER_PAGE_H_

#define MIN_RADIOCHANNEL     0x00
#define MAX_RADIOCHANNEL     0x4F

struct scanner_page {
    u8 channelnoise[MAX_RADIOCHANNEL - MIN_RADIOCHANNEL + 1];
    u8 channel;
    u8 time_to_scan : 1;
    u8 enable : 1;
    u8 scan_mode : 1;
    u8 attenuator : 1;
};
#endif
