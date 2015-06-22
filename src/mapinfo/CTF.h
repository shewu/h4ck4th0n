#ifndef MAPINFO_CTF_H
#define MAPINFO_CTF_H

enum CTFWallType {
    CTF_WT_NORMAL,
    CTF_WT_DEADLY,
    CTF_WT_GOAL1,
    CTF_WT_GOAL2,

    CTF_WT_NUMTYPES
};

static const std::map<std::string, WallTypeData> CTFWTMap = {
    {"normal",
     WallTypeData(CTF_WT_NORMAL, MaterialPtr(new Color(101, 67, 33)))},
    {"deadly", WallTypeData(CTF_WT_DEADLY, MaterialPtr(new Color(0, 255, 0)))},
    {"goal1", WallTypeData(CTF_WT_GOAL1, MaterialPtr(new Color(200, 0, 0)))},
    {"goal2", WallTypeData(CTF_WT_GOAL2, MaterialPtr(new Color(0, 0, 200)))}};

#endif
