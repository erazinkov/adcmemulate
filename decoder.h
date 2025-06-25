#ifndef DECODER_H
#define DECODER_H

#include "adcm_df.h"
#include "channelmap.h"

class Decoder
{
public:
    Decoder(const std::string &, const ChannelMap &);
    std::vector<dec_ev_t> & events();
    dec_cnt_t & counters();
    const std::vector<u_int32_t> &offsets() const;

    void process();

private:
    const std::string fileName_;
    ChannelMap pre_;
    std::vector<dec_ev_t> events_;
    dec_cnt_t counters_;
    std::vector<u_int32_t> offsets_;
};


#endif // DECODER_H
