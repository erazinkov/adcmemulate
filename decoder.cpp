#include "decoder.h"

#include <iostream>
#include <bits/stdc++.h>

#include "progressbar.h"

Decoder::Decoder(const std::string &fileName, const ChannelMap &pre)
    : fileName_{fileName} , pre_{pre}
{

}

std::vector<dec_ev_t> &Decoder::events()
{
    return events_;
}

dec_cnt_t &Decoder::counters()
{
    return counters_;
}

const std::vector<u_int32_t> &Decoder::offsets() const
{
    return offsets_;
}

void Decoder::process()
{
    offsets_.clear();

    std::ifstream ifs;
    ifs.open(fileName_, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
    {
        std::cerr << "Can't open input file" << std::endl;
        return;
    }

    ifs.seekg(0, std::ios::end);
    u_int32_t size{static_cast<u_int32_t>(ifs.tellg())};
    ifs.seekg(0);

    stor_packet_hdr_t hdr;
    adcm_cmap_t cmap;
    adcm_counters_t counters;

    auto c{false};
    u_int32_t currentPosition{0};
    while (ifs)
    {
        ifs >> hdr;
        currentPosition += sizeof(stor_packet_hdr_t);
        if (hdr.id == STOR_ID_CMAP && hdr.size > sizeof(stor_packet_hdr_t))
        {
            currentPosition = static_cast<u_int32_t>(ifs.tellg());
            ifs >> cmap;
            c = pre_.isCorrect(cmap.map);
            if (c)
            {
                currentPosition -= sizeof(stor_packet_hdr_t);
                offsets_.push_back(currentPosition);
                ProgressBar<u_int32_t>::show(currentPosition, size);
            }
            continue;
        }
        if (hdr.id == STOR_ID_EVNT && hdr.size > sizeof(stor_packet_hdr_t))
        {
            hdr.size -= sizeof(stor_packet_hdr_t);
            ifs.ignore(hdr.size);
            continue;
        }
        if (hdr.id == STOR_ID_CNTR && hdr.size > sizeof(stor_packet_hdr_t))
        {
            if (!c)
            {
                hdr.size -= sizeof(stor_packet_hdr_t);
                ifs.ignore(hdr.size);
                continue;
            }
            ifs >> counters;
            counters_.time += counters.time;
            ifs.ignore(hdr.size
                       - sizeof(stor_packet_hdr_t)
                       - (sizeof(counters.n) + sizeof(counters.time) + sizeof(*counters.rawhits.cbegin()) * counters.n));
            continue;
        }
        ifs.seekg(1 - static_cast<u_int32_t>(sizeof(stor_packet_hdr_t)), std::ios_base::cur);
    }
    ifs.close();
}


