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

void Decoder::process()
{
    // std::ifstream ifs;
    // ifs.open(fileName_, std::ios::in | std::ios::binary);
    // if (!ifs.is_open())
    // {
    //     std::clog << "Can't open file" << std::endl;
    //     return;
    // }

    // counters_.rawhits.resize(pre_.numberOfChannelsAlpha());

    // events_.clear();

    // stor_packet_hdr_t hdr;
    // stor_ev_hdr_t ev;
    // adcm_cmap_t cmap;
    // adcm_counters_t counters;

    // auto c{false};

    // while (ifs)
    // {
    //     ifs >> hdr;

    //     if (hdr.id == STOR_ID_CMAP && hdr.size > sizeof(stor_packet_hdr_t))
    //     {
    //         ifs >> cmap;
    //         c = pre_.isCorrect(cmap.map);
    //         continue;
    //     }
    //     if (hdr.id == STOR_ID_EVNT && hdr.size > sizeof(stor_packet_hdr_t))
    //     {
    //         if (!c)
    //         {
    //             hdr.size -= sizeof(stor_packet_hdr_t);
    //             ifs.ignore(hdr.size);
    //             continue;
    //         }
    //         ifs >> ev;
    //         if (ev.np != 2)
    //         {
    //             hdr.size -= sizeof(stor_packet_hdr_t);
    //             hdr.size -= sizeof(stor_ev_hdr_t);
    //             ifs.ignore(hdr.size);
    //             continue;
    //         }
    //         stor_puls_t *g = new stor_puls_t();
    //         stor_puls_t *a = new stor_puls_t();
    //         ifs >> *g >> *a;
    //         if (g->ch < pre_.map().size() && a->ch < pre_.map().size())
    //         {
    //             dec_ev_t event;
    //             auto numberGamma{pre_.numberByChannel(g->ch)};
    //             auto numberAlpha{pre_.numberByChannel(a->ch)};
    //             event.g.index = numberGamma;
    //             event.g.amp = g->a;
    //             event.a.index = numberAlpha;
    //             event.a.amp = g->a;
    //             event.tdc = g->t - a->t;
    //             event.ts = ev.ts;
    //             events_.push_back(event);
    //         }
    //         delete g;
    //         delete a;

    //         continue;
    //     }
    //     if (hdr.id == STOR_ID_CNTR && hdr.size > sizeof(stor_packet_hdr_t))
    //     {
    //         if (!c)
    //         {
    //             hdr.size -= sizeof(stor_packet_hdr_t);
    //             ifs.ignore(hdr.size);
    //         }
    //         ifs >> counters;
    //         for (size_t i{0}; i < pre_.map().size(); ++i)
    //         {
    //             auto number{pre_.numberByChannel(static_cast<u_int32_t>(i))};
    //             auto type{pre_.typeByChannel(static_cast<u_int32_t>(i))};
    //             switch (type)
    //             {
    //                 case ALPHA:
    //                 {
    //                     counters_.rawhits[number] += counters.rawhits.at(i);
    //                     counters_.time += counters.time;
    //                     break;
    //                 }
    //                 default:
    //                 {
    //                     break;
    //                 }
    //             }
    //         }
    //         continue;
    //     }
    //     ifs.seekg(1 - static_cast<long long>(sizeof(stor_packet_hdr_t)), std::ios_base::cur);
    // }
    // ifs.close();
}

std::vector<u_int32_t> Decoder::positionsOfCMAPHeaders()
{
    std::ifstream ifs;
    std::vector<u_int32_t> pos{};
    ifs.open(fileName_, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
    {
        std::clog << "Can't open file" << std::endl;
        return pos;
    }

    ifs.seekg(0, std::ios::end);
    u_int32_t size{static_cast<u_int32_t>(ifs.tellg())};
    ifs.seekg(0);

    stor_packet_hdr_t hdr;
    adcm_cmap_t cmap;


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
                pos.push_back(currentPosition);
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
            // continue;
            // hdr.size -= sizeof(stor_packet_hdr_t);
            // ifs.ignore(hdr.size);
            adcm_counters_t counters;
            ifs >> counters;
            ifs.ignore(hdr.size - sizeof(stor_packet_hdr_t) - (sizeof(counters.n) +
                                                               sizeof(counters.time) +
                                                               sizeof(*counters.rawhits.cbegin()) * counters.n));
            continue;
        }
        std::cout << "Error" << std::endl;
        ifs.seekg(1 - static_cast<u_int32_t>(sizeof(stor_packet_hdr_t)), std::ios_base::cur);
    }
    ifs.close();

    return pos;
}


