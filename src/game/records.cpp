/*
    Copyright (C) 2005 Michael K. McCarty & Fritz Bronner

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// This file handles the Mission Records screen

#include "records.h"

#include <algorithm>

#include "display/graphics.h"

#include "Buzz_inc.h"
#include "draw.h"
#include "endianness.h"
#include "game_main.h"
#include "hardef.h"
#include "logging.h"
#include "mission_util.h"
#include "options.h"
#include "pace.h"
#include "place.h"
#include "port.h"
#include "replay.h"

LOG_DEFAULT_CATEGORY(LOG_ROOT_CAT)

#define MLL(a,b) (Data->P[a].History[b].MissionCode>=53 && Data->P[a].History[b].MissionCode<=56)

void RecChange(int i, int j, int k, int temp, int max, char Rec_Change);

char NREC[56][3];
Record_Entry rec[56][3];

void Move2rec(char* pos, char* pos2, char val);
void ClearRecord(char* pos2);
void BackTop(char* pos, char* pos2);
void Back1rec(char* pos, char* pos2);
void For1rec(char* pos, char* pos2);
void ForEnd(char* pos, char* pos2);
void Drec(char* pos, char* pos2, char mde);
void WriteRecord(int i, int j, int k, int temp);
void SwapRec(int Rc, int pl1, int pl2);
char CheckSucess(int i, int j);
void ImportRecordTable();
void ExportRecordTable();

int Pict[56] = {
    411, 2, 1, 177, 272, 275, 409, 501, 504, 507, 414,
    497, 476, 571, 162, 210, 574, 185, 506, 180, 181,
    496, 530, 521, 286, 325, 495, 172, 537, 441, 404,
    326, 572, 324, 301, 3, 17, 4, 18, 22, 19,
    295, 296, 262, 78, 282, 400, 309, 322, 308, 285,
    317, 247, 239, 322, 291
};

const char* Record_Names[56] = {
    "ORBITAL SATELLITE",
    "MAN IN SPACE",
    "WOMAN IN SPACE",
    "PERSON IN ORBIT",
    "SPACEWALK (MALE)",
    "SPACEWALK (FEMALE)",
    "LUNAR FLYBY",
    "MERCURY FLYBY",
    "VENUS FLYBY",
    "MARS FLYBY",
    "JUPITER FLYBY",
    "SATURN FLYBY",
    "LUNAR PROBE LANDING",
    "ONE-PERSON CRAFT",
    "TWO-PERSON CRAFT",
    "THREE-PERSON CRAFT",
    "MINISHUTTLE",
    "FOUR-PERSON CRAFT",
    "SPACECRAFT PRESTIGE POINTS",
    "MANNED LUNAR PASS",
    "MANNED LUNAR ORBIT",
    "MANNED LUNAR LANDING",
    "FEWEST CASUALTIES IN A GAME",
    "MOST CASUALTIES IN A GAME",
    "HIGHEST SAFETY AVG LUNAR LANDING",
    "LOWEST SAFETY AVG LUNAR LANDING",
    "FIRST DOCKING",
    "FIRST MANNED DOCKING",
    "FIRST ORBITAL LABORATORY",
    "LONGEST MISSION DURATION",
    "MISSION W/ MOST PRESTIGE POINTS",
    "FEWEST MISSIONS IN GAME",
    "MOST MISSIONS IN GAME",
    "MOST MANNED MISSIONS ATTEMPTED",
    "MOST SUCCESSFUL MANNED MISSIONS",
    "MOST SPACE MISSIONS (MALE)",
    "MOST SPACE MISSIONS (FEMALE)",
    "HIGHEST PRESTIGE POINTS (MALE)",
    "HIGHEST PRESTIGE POINTS (FEMALE)",
    "MOST DAYS IN SPACE (MALE)",
    "MOST DAYS IN SPACE (FEMALE)",
    "LONGEST TERM OF DUTY",
    "HIGHEST DIRECTOR RATING ",
    "HIGHEST AVERAGE BUDGET",
    "LOWEST AVERAGE BUDGET",
    "GREATEST PRESTIGE TOTAL",
    "EARLIEST LOR LANDING",
    "EARLIEST EOR LANDING",
    "EARLIEST DIRECT ASCENT LANDING",
    "EARLIEST HISTORICAL LANDING",
    "UNITED STATES VICTORIES",
    "SOVIET UNION VICTORIES",
    "LOR VICTORIES",
    "EOR VICTORIES",
    "DIRECT ASCENT VICTORIES",
    "HISTORICAL VICTORIES"
};


const char* Months[12] = {
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};


void MakeRecords()
{
    // check if Records file exists
    FILE* file = sOpen("RECORDS.DAT", "rb", FT_SAVE_CHECK);
    if (file != nullptr) { // if it does, we're done
        fclose(file);
        return;
    }

    for (int i = 0; i < 56; i++) {
        for (int j = 0; j < 3; j++) { // default values
            rec[i][j].country = -1;
            rec[i][j].tag = rec[i][j].month = rec[i][j].yr = rec[i][j].program = rec[i][j].type = 0;
            rec[i][j].place = 0;
        }
    }

    // write defaults out
    ExportRecordTable();
}

void Records(char plr)
{
    char pos = 0, pos2 = 0;
    
    ImportRecordTable();

    FadeOut(2, 5, 0, 0);
    PortPal(plr);
    display::graphics.screen()->clear();
    ShBox(0, 0, 319, 22);
    ShBox(0, 24, 319, 199);
    InBox(4, 27, 315, 196);
    fill_rectangle(5, 28, 314, 195, 0);

    ShBox(23, 29, 228, 52);
    ShBox(23, 54, 228, 77);
    ShBox(23, 79, 228, 102);

    IOBox(243, 3, 316, 19);  // Bottom half of the screen stuff
    ShBox(6, 104, 313, 194);
    InBox(10, 127, 309, 191);
    fill_rectangle(11, 128, 308, 190, 0);
    ShBox(297, 129, 307, 158);
    ShBox(297, 160, 307, 189);  // Arrows
    display::graphics.setForegroundColor(1);
    draw_string(258, 13, "CONTINUE");
    draw_heading(42, 5, "MISSION RECORDS", 0, -1);
    draw_up_arrow(299, 131);
    draw_down_arrow(299, 162);
    ShBox(6, 29, 21, 102);
    ShBox(230, 29, 313, 102);
    display::graphics.setForegroundColor(1);
    draw_number(12, 42, 1);
    draw_number(12, 66, 2);
    draw_number(12, 90, 3);
    fill_rectangle(70, 109, 266, 121, 4);
    fill_rectangle(72, 111, 264, 119, 0);
    IOBox(234, 84, 309, 99);
    display::graphics.setForegroundColor(1);
    draw_string(238, 93, "CLEAR RECORD");
    InBox(237, 34, 306, 81);
    Drec(&pos, &pos2, 0);
    FadeIn(2, 10, 0, 0);

    WaitForMouseUp();

    while (1) {
        GetMouse();

        // Parse Button actions, note that return is embedded in first pButton
        if ((x >= 245 && y >= 5 
              && x <= 314 && y <= 17 
              && mousebuttons > 0) 
            || key == K_ENTER 
            || key == K_ESCAPE) {
            InBox(245, 5, 314, 17);

            if (key > 0) {
                delay(300);
                key = 0;
            }

            WaitForMouseUp();

            OutBox(245, 5, 314, 17);

            key = 0;

            return;
        }

        if (key == K_HOME) {
            BackTop(&pos, &pos2);
        }

        if (key == K_PGUP) {
            Back1rec(&pos, &pos2);
            Back1rec(&pos, &pos2);
            Back1rec(&pos, &pos2);
            Back1rec(&pos, &pos2);
            Back1rec(&pos, &pos2);
            Back1rec(&pos, &pos2);
            Back1rec(&pos, &pos2);
            Back1rec(&pos, &pos2);
        }

        if (key == K_PGDN) {
            For1rec(&pos, &pos2);
            For1rec(&pos, &pos2);
            For1rec(&pos, &pos2);
            For1rec(&pos, &pos2);
            For1rec(&pos, &pos2);
            For1rec(&pos, &pos2);
            For1rec(&pos, &pos2);
            For1rec(&pos, &pos2);
        }

        if (key == K_END) {
            ForEnd(&pos, &pos2);
        }

        pButton(297, 129, 307, 158, Back1rec(&pos, &pos2), key >> 8, 72);
        pButton(297, 160, 307, 189, For1rec(&pos, &pos2), key >> 8, 80);
        pButton(236,  86, 307,  97, ClearRecord(&pos2), key, 'C');
        Button2( 15, 129, 160, 133, Move2rec(&pos, &pos2, 0), key, 49);
        Button2( 15, 136, 160, 140, Move2rec(&pos, &pos2, 1), key, 50);
        Button2( 15, 143, 160, 147, Move2rec(&pos, &pos2, 2), key, 51);
        Button2( 15, 150, 160, 154, Move2rec(&pos, &pos2, 3), key, 52);
        Button2( 15, 157, 160, 161, Move2rec(&pos, &pos2, 4), key, 53);
        Button2( 15, 164, 160, 168, Move2rec(&pos, &pos2, 5), key, 54);
        Button2( 15, 171, 160, 175, Move2rec(&pos, &pos2, 6), key, 55);
        Button2( 15, 178, 160, 182, Move2rec(&pos, &pos2, 7), key, 56);
        Button2( 15, 185, 160, 189, Move2rec(&pos, &pos2, 8), key, 57);
        key = 0;
    }
}

void Move2rec(char* pos, char* pos2, char val)
{
    *pos2 = *pos + val;
    Drec(pos, pos2, 1);
}

void ClearRecord(char* pos2)
{
    int choice = Help("i125");

    if (choice == -1) {
        return;
    }

    ImportRecordTable();

    //clear record
    for (int j = 0; j < 3; j++) {
        NREC[*pos2][j] = 0x00;
        rec[*pos2][j].country = -1;
        rec[*pos2][j].tag = rec[*pos2][j].month = rec[*pos2][j].yr = rec[*pos2][j].program = 0;
        rec[*pos2][j].place = 0;
    }

    ShBox(23, 29, 228, 52);
    ShBox(23, 54, 228, 77);
    ShBox(23, 79, 228, 102);
    fill_rectangle(24, 30, 227, 51, 3);
    fill_rectangle(24, 55, 227, 76, 3);
    fill_rectangle(24, 80, 227, 101, 3);
    display::graphics.setForegroundColor(1);
    draw_number(12, 42, 1);
    draw_number(12, 66, 2);
    draw_number(12, 90, 3);

    ExportRecordTable();
}

void BackTop(char* pos, char* pos2)
{
    *pos = 0;
    *pos2 = 0;
    Drec(pos, pos2, 1);
}

void Back1rec(char* pos, char* pos2)
{
    if (*pos2 == 0) {
        return;
    }

    *pos2 -= 1;

    if (*pos2 < *pos) {
        *pos -= 1;
    }

    Drec(pos, pos2, 1);
}

void For1rec(char* pos, char* pos2)
{
    if (*pos2 == 55) {
        return;
    }

    *pos2 += 1;

    if (*pos > 47) ;
    else if (*pos2 > *pos + 8) {
        *pos += 1;
    }

    Drec(pos, pos2, 1);
}

void ForEnd(char* pos, char* pos2)
{
    *pos = 47;
    *pos2 = 55;
    Drec(pos, pos2, 1);
}


void Drec(char* pos, char* pos2, char mde)
{
    display::graphics.setForegroundColor(1);
    draw_number(12, 42, 1);
    draw_number(12, 66, 2);
    draw_number(12, 90, 3);
    fill_rectangle(238, 35, 305, 80, 0);

    if (Pict[*pos2] != 0) {
        DispBaby(238, 35, Pict[*pos2], mde);
    }

    fill_rectangle(12, 129, 295, 190, 0);  // Clear bottom text

    for (int i=0; i<3; ++i) {
        if (NREC[*pos2][i] == 0) {
            ShBox(23, 29+25*i, 228, 52+25*i);
        } else {
            display::graphics.setForegroundColor(8);
            draw_number(12, 42+24*i, 1);
            InBox(23, 29+25*i, 228, 52+25*i);
        }
        fill_rectangle(24, 30+25*i, 227, 51+25*i, 3);
    }

    for (int i = *pos, j=0; i < *pos + 9; i++, j++) {
        if (i == *pos2) {
            display::graphics.setForegroundColor(11);
        } else {
            display::graphics.setForegroundColor(2);
        }

        draw_string(15, 133 + 7 * j, Record_Names[i]);

        if (i == *pos2) {
            display::graphics.setForegroundColor(11);
        } else {
            display::graphics.setForegroundColor(12);
        }
    }

    display::graphics.setForegroundColor(2);
    fill_rectangle(72, 111, 264, 119, 0);
    display::graphics.setForegroundColor(1);
    draw_string(83, 117, Record_Names[*pos2]);

    for (int i = 0; i < 3; i++) {
        if (rec[*pos2][i].country == NOT_SET) {
            return;
        }

        if (*pos2 < 52) {
            fill_rectangle(27, 33 + (i * 24), 54, 49 + (i * 24), 4);
            draw_small_flag(rec[*pos2][i].country, 28, 34 + (i * 24));

            if (*pos2 == 50 || *pos2 == 51) {
                fill_rectangle(196, 33 + (i * 24), 223, 49 + (i * 24), 4);
                draw_small_flag(rec[*pos2][i].country, 197, 34 + (i * 24));
            }
        } else {
            fill_rectangle(27, 33 + (i * 24), 54, 49 + (i * 24), 4);
            draw_small_flag(0, 28, 34 + (i * 24));
            fill_rectangle(196, 33 + (i * 24), 223, 49 + (i * 24), 4);
            draw_small_flag(1, 197, 34 + (i * 24));
        }

        if (*pos2 < 50) {
            display::graphics.setForegroundColor(9);
            draw_string(61, 48 + (i * 24), "DIR: ");
            display::graphics.setForegroundColor(1);
            draw_string(0, 0, rec[*pos2][i].name);
        }

        switch (rec[*pos2][i].type) {
        case 1:
            display::graphics.setForegroundColor(9);
            draw_string(61, 38 + (i * 24), "DATE: ");
            display::graphics.setForegroundColor(1);
            draw_string(0, 0, Months[rec[*pos2][i].month]);
            draw_string(0, 0, " ");
            draw_number(0, 0, rec[*pos2][i].yr + 1900);

            if (*pos2 == 29) {
                display::graphics.setForegroundColor(6);
                draw_string(143, 38 + (i * 24), "DURATION: ");
                display::graphics.setForegroundColor(1);

                if (rec[*pos2][i].tag >= 1 && rec[*pos2][i].tag <= 6) {
                    draw_character('A'+rec[*pos2][i].tag-1);
                }
            }

            break;

        case 2:
            display::graphics.setForegroundColor(9);

            if (rec[*pos2][i].country == 1) {
                draw_string(61, 38 + (i * 24), "COSMONAUT: ");
            } else {
                draw_string(61, 38 + (i * 24), "ASTRONAUT: ");
            }

            display::graphics.setForegroundColor(1);
            draw_string(0, 0, rec[*pos2][i].astro);

            switch (*pos2) {
            case 1: case 2: case 3:
            case 4: case 5: case 6:
                display::graphics.setForegroundColor(6);
                draw_string(143, 48 + (i * 24), "DATE: ");
                display::graphics.setForegroundColor(1);
                draw_string(0, 0, Months[rec[*pos2][i].month]);
                draw_string(0, 0, " ");
                draw_number(0, 0, rec[*pos2][i].yr + 1900);
                break;

            case 35: case 36:
                display::graphics.setForegroundColor(6);
                draw_string(143, 48 + (i * 24), "MISSIONS: ");
                display::graphics.setForegroundColor(1);
                draw_number(0, 0, rec[*pos2][i].tag);
                break;

            case 37: case 38:
                display::graphics.setForegroundColor(6);
                draw_string(143, 48 + (i * 24), "PRESTIGE: ");
                display::graphics.setForegroundColor(1);
                draw_number(0, 0, rec[*pos2][i].tag);
                break;

            case 39: case 40:
                display::graphics.setForegroundColor(6);
                draw_string(143, 48 + (i * 24), "DAYS: ");
                display::graphics.setForegroundColor(1);
                draw_number(0, 0, rec[*pos2][i].tag);
                break;

            case 41:
                display::graphics.setForegroundColor(6);
                draw_string(143, 48 + (i * 24), "SEASONS: ");
                display::graphics.setForegroundColor(1);
                draw_number(0, 0, rec[*pos2][i].tag);
                break;

            default:
                break;
            }

            break;

        case 3:
            display::graphics.setForegroundColor(9);
            draw_string(61, 38 + (i * 24), "RECORD: ");
            display::graphics.setForegroundColor(1);

            if (*pos2 == 18) {  //special case craft and prestige points
                display::graphics.setForegroundColor(1);
                draw_string(0, 0, Data->P[rec[*pos2][i].country].Manned[rec[*pos2][i].program].Name);
                display::graphics.setForegroundColor(6);
                draw_string(143, 48 + (i * 24), "PRESTIGE: ");
                display::graphics.setForegroundColor(1);
                draw_number(0, 0, rec[*pos2][i].tag);
            } else {
                draw_number(101, 38 + (i * 24), rec[*pos2][i].tag);

                switch (*pos2) {
                case 22: case 23:
                    draw_string(0, 0, " CASUALTIES");
                    break;

                case 24: case 25:
                    draw_string(0, 0, " PERCENT");
                    break;

                case 31: case 32:
                    draw_string(0, 0, " MISSIONS");
                    break;

                case 33: case 34:
                    draw_string(0, 0, " MISSIONS");
                    break;

                case 42:
                    draw_string(0, 0, " POINTS");
                    break;

                case 43: case 44:
                    draw_string(0, 0, " M.B.");
                    break;

                case 50: case 51: case 52:
                case 53: case 54: case 55:
                    draw_string(0, 0, " TIMES");
                    break;

                default:
                    break;
                }
            }

            break;

        default:
            break;
        }
    }
}

void WriteRecord(int i, int j, int k, int temp)
{
    auto& pData = Data->P[i];
    auto& mission = pData.History[j];
    char t = rec[k][0].place;
    NREC[k][t] = 1;
    ++rec[k][0].place;

    switch (rec[k][t].type) {
    case 1:
        rec[k][t].yr = mission.MissionYear;
        rec[k][t].month = mission.Month;
        rec[k][t].program = mission.MissionCode;

        if (k == 29 || k == 30) {
            rec[k][t].tag = temp;
        }

        break;

    case 2:
        if (k >= 35 && k <= 41) {
            rec[k][t].tag = temp;
            strcpy(rec[k][t].astro, pData.Pool[j].Name);
        } else {
            rec[k][t].yr = mission.MissionYear;
            rec[k][t].month = mission.Month;
            strcpy(rec[k][t].astro, pData.Pool[temp].Name);
        }

        break;

    case 3:
        rec[k][t].tag = temp;

        if (k == 18) {
            rec[k][t].program = j;
        }

        break;

    default:
        break;
    }

    strcpy(rec[k][t].name, pData.Name);
    rec[k][t].country = i;
}

void SwapRec(int Rc, int pl1, int pl2)
{
    char Tmp[20];

    std::swap(NREC[Rc][pl1], NREC[Rc][pl2]);
    std::swap(rec[Rc][pl1].country, rec[Rc][pl2].country);
    std::swap(rec[Rc][pl1].yr, rec[Rc][pl2].yr);
    std::swap(rec[Rc][pl1].month, rec[Rc][pl2].month);
    std::swap(rec[Rc][pl1].program, rec[Rc][pl2].program);
    std::swap(rec[Rc][pl1].tag, rec[Rc][pl2].tag);
// Swap Astro
    strcpy(Tmp, rec[Rc][pl1].astro);
    strcpy(rec[Rc][pl1].astro, rec[Rc][pl2].astro);
    strcpy(rec[Rc][pl2].astro, Tmp);
// Swap Name
    strcpy(Tmp, rec[Rc][pl1].name);
    strcpy(rec[Rc][pl1].name, rec[Rc][pl2].name);
    strcpy(rec[Rc][pl2].name, Tmp);
}


char CheckSucess(int i, int j)
{
    auto& mission = Data->P[i].History[j];
    if (mission.Duration != 0) {
        if (mission.spResult < 500 || mission.spResult >= 5000) {
            return 1;
        } else {
            return 0;
        }
    } else if (mission.spResult == 1 && mission.Event == 0) {
        return 1;
    } else {
        return 0;
    }
}

void SafetyRecords(char plr, int temp)
{
    ImportRecordTable();
    
    // deal with case highest safety and lowest safety average
    rec[24][0].type = 3;
    rec[24][1].type = 3;
    rec[24][2].type = 3;
    rec[25][0].type = 3;
    rec[25][1].type = 3;
    rec[25][2].type = 3;
    
    int j=0; //starting value
    for (int k=24; k<26; ++k) {
        switch (rec[k][0].place) {
        case 0:
            WriteRecord(plr, j, k, temp);
            break;

        case 1:
            if ((k == 24 && rec[k][0].tag <= temp) || (k == 25 && rec[k][0].tag > temp)) {
                WriteRecord(plr, j, k, temp);
                SwapRec(k, 1, 0);
            } else {
                WriteRecord(plr, j, k, temp);
            }

            break;

        case 2:
            if ((k == 24 && rec[k][1].tag <= temp) || (k == 25 && rec[k][1].tag > temp)) {
                WriteRecord(plr, j, k, temp);
                SwapRec(k, 2, 1);

                if ((k == 24 && rec[k][0].tag <= temp) || (k == 25 && rec[k][0].tag > temp)) {
                    SwapRec(k, 1, 0);
                }
            } else {
                WriteRecord(plr, j, k, temp);
            }

            break;

        case 3:
            if ((k == 24 && rec[k][2].tag <= temp) || (k == 25 && rec[k][2].tag > temp)) {
                --rec[k][0].place;
                WriteRecord(plr, j, k, temp);

                if ((k == 24 && rec[k][1].tag <= temp) || (k == 25 && rec[k][1].tag > temp)) {
                    SwapRec(k, 2, 1);

                    if ((k == 24 && rec[k][0].tag <= temp) || (k == 25 && rec[k][0].tag > temp)) {
                        SwapRec(k, 1, 0);
                    }
                }
            }

            break;

        default:
            break;
        }
    }

    ExportRecordTable();
}

void UpdateRecords(char Ty)
{
    if (options.want_cheats) return; // see #1136
                                     // TODO some kind of "used cheats in savefile" flag instead

    for (int j = 0; j < 56; j++) {
        for (int i = 0; i < 3; i++) {
            NREC[j][i] = 0x00;
        }
    }

    ImportRecordTable();

    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (AI[i]) continue;
        auto& pData = Data->P[i];
        for (int j = 0; j < pData.PastMissionCount; j++) {
            if (CheckSucess(i, j) != 1) continue;
            auto& mission = pData.History[j];
            
            for (int k = 0; k < 56; k++) {
                char Rec_Change = -1;
                int temp = 0;
                int max = 0;

                //GetMisData(mission.MissionCode);
                switch (k) {
                case 0: //Orbital Satellite
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_Orbital_Satellite) {
                            Rec_Change = 0;
                        }
                    } else {
                        if (mission.MissionCode == Mission_Orbital_Satellite) {
                            Rec_Change = 1;
                        }
                    }

                    break;

                case 1:
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    if (mission.Duration > 0) {
                        for (int m = 0; m < 4; m++) {
                            int spaceman_idx = mission.Man[PAD_A][m];
                            if (spaceman_idx == -1) continue;
                            if (pData.Pool[spaceman_idx].Sex != 0) continue;
                            
                            temp = spaceman_idx;

                            if (rec[k][0].place == 0) {
                                Rec_Change = 0;
                            } else {
                                Rec_Change = 2;
                            }

                            RecChange(i, j, k, temp, m, Rec_Change);
                        }
                    }

                    Rec_Change = -1;
                    break;

                case 2:
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    if (mission.Duration > 0) {
                        for (int m = 0; m < 4; m++) {
                            int spaceman_idx = mission.Man[PAD_A][m];
                            if (spaceman_idx == -1) continue;
                            if (pData.Pool[spaceman_idx].Sex != 1) continue;
                            
                            temp = spaceman_idx;

                            if (rec[k][0].place == 0) {
                                Rec_Change = 0;
                            } else {
                                Rec_Change = 2;
                            }

                            RecChange(i, j, k, temp, m, Rec_Change);
                        }
                    }

                    Rec_Change = -1;
                    break;

                case 3:
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    if (mission.Duration > 0) {
                        for (int m = 0; m < 4; m++) {
                            int spaceman_idx = mission.Man[PAD_A][m];
                            if (spaceman_idx == -1) continue;
                            
                            temp = spaceman_idx;

                            if (rec[k][0].place == 0) {
                                Rec_Change = 0;
                            } else {
                                Rec_Change = 2;
                            }

                            RecChange(i, j, k, temp, m, Rec_Change);
                        }
                    }

                    Rec_Change = -1;
                    break;

                case 4: //EVA record (MALE)
                    {
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    int m;
                    switch (mission.Hard[PAD_A][Mission_Capsule]) {
                    case MANNED_HW_ONE_MAN_CAPSULE:
                        m = 0;
                        break;

                    case MANNED_HW_TWO_MAN_CAPSULE:
                    case MANNED_HW_THREE_MAN_CAPSULE:
                    case MANNED_HW_MINISHUTTLE:
                        m = 1;
                        break;

                    case MANNED_HW_FOUR_MAN_CAPSULE:
                        m = 2;
                        break;

                    default:
                        m = 0;
                        break;
                    }
                        
                    int spaceman_idx = mission.Man[PAD_A][m];

                    if (GetMissionPlan(mission.MissionCode).EVA) {
                        if (spaceman_idx != -1)
                            if (pData.Pool[spaceman_idx].Sex == 0) {
                                temp = spaceman_idx;

                                if (rec[k][0].place == 0) {
                                    Rec_Change = 0;
                                } else {
                                    Rec_Change = 2;
                                }

                                RecChange(i, j, k, temp, m, Rec_Change);
                            }
                    }

                    Rec_Change = -1;
                    break;
                    }

                case 5: //EVA record (FEMALE)
                    {
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    int m;
                    switch (mission.Hard[PAD_A][Mission_Capsule]) {
                    case MANNED_HW_ONE_MAN_CAPSULE:
                        m = 0;
                        break;

                    case MANNED_HW_TWO_MAN_CAPSULE:
                    case MANNED_HW_THREE_MAN_CAPSULE:
                    case MANNED_HW_MINISHUTTLE:
                        m = 1;
                        break;

                    case MANNED_HW_FOUR_MAN_CAPSULE:
                        m = 2;
                        break;

                    default:
                        m = 0;
                        break;
                    }
                        
                    int spaceman_idx = mission.Man[PAD_A][m];
                        
                    Rec_Change = -1;

                    if (! GetMissionPlan(mission.MissionCode).EVA) break;
                    if (spaceman_idx == -1) break;
                    if (pData.Pool[spaceman_idx].Sex != 1) break;
                    
                    temp = spaceman_idx;

                    if (rec[k][0].place == 0) {
                        Rec_Change = 0;
                    } else {
                        Rec_Change = 2;
                    }

                    RecChange(i, j, k, spaceman_idx, m, Rec_Change);

                    Rec_Change = -1;
                    break;
                    }

                case 6: //LUNAR flyby
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_LunarFlyby) {
                            Rec_Change = 0;
                        }
                    } else if (mission.MissionCode == Mission_LunarFlyby) {
                        Rec_Change = 1;
                    }

                    break;

                case 7: //MERCURY flyby
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_MercuryFlyby) {
                            Rec_Change = 0;
                        }
                    } else if (mission.MissionCode == Mission_MercuryFlyby) {
                        Rec_Change = 1;
                    }

                    break;

                case 8: //VENUS flyby
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_VenusFlyby) {
                            Rec_Change = 0;
                        }
                    } else if (mission.MissionCode == Mission_VenusFlyby) {
                        Rec_Change = 1;
                    }

                    break;

                case 9: //MARS flyby
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_MarsFlyby) {
                            Rec_Change = 0;
                        }
                    } else if (mission.MissionCode == Mission_MarsFlyby) {
                        Rec_Change = 1;
                    }

                    break;

                case 10: //JUPITER flyby
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_JupiterFlyby) {
                            Rec_Change = 0;
                        }
                    } else if (mission.MissionCode == Mission_JupiterFlyby) {
                        Rec_Change = 1;
                    }

                    break;

                case 11: //SATURN flyby
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_SaturnFlyby) {
                            Rec_Change = 0;
                        }
                    } else if (mission.MissionCode == Mission_SaturnFlyby) {
                        Rec_Change = 1;
                    }

                    break;

                case 12: //LUNAR PROBE landing
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_Lunar_Probe) {
                            Rec_Change = 0;
                        }
                    } else if (mission.MissionCode == Mission_Lunar_Probe) {
                        Rec_Change = 1;
                    }

                    break;

                case 13: //ONE-person craft
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.Hard[PAD_A][Mission_Capsule] == MANNED_HW_ONE_MAN_CAPSULE 
                            && mission.Man[PAD_A][0] != -1) {
                            Rec_Change = 0;
                        }
                    } else if (mission.Hard[PAD_A][Mission_Capsule] == MANNED_HW_ONE_MAN_CAPSULE 
                               && mission.Man[PAD_A][0] != -1) {
                        Rec_Change = 1;
                    }

                    break;

                case 14: //TWO-person craft
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.Hard[PAD_A][Mission_Capsule] == MANNED_HW_TWO_MAN_CAPSULE 
                            && mission.Man[PAD_A][0] != -1) {
                            Rec_Change = 0;
                        }
                    } else if (mission.Hard[PAD_A][Mission_Capsule] == MANNED_HW_TWO_MAN_CAPSULE 
                               && mission.Man[PAD_A][0] != -1) {
                        Rec_Change = 1;
                    }

                    break;

                case 15: //THREE-person craft
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.Hard[PAD_A][Mission_Capsule] == MANNED_HW_THREE_MAN_CAPSULE 
                            && mission.Man[PAD_A][0] != -1) {
                            WriteRecord(i, j, k, 0);
                        }
                    } else if (mission.Hard[PAD_A][Mission_Capsule] == MANNED_HW_THREE_MAN_CAPSULE 
                               && mission.Man[PAD_A][0] != -1) {
                        Rec_Change = 1;
                    }

                    break;

                case 16: //MINISHUTTLE craft
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.Hard[PAD_A][Mission_Capsule] == MANNED_HW_MINISHUTTLE 
                            && mission.Man[PAD_A][0] != -1) {
                            Rec_Change = 0;
                        }
                    } else if (mission.Hard[PAD_A][Mission_Capsule] == MANNED_HW_MINISHUTTLE 
                               && mission.Man[PAD_A][0] != -1) {
                        Rec_Change = 1;
                    }

                    break;

                case 17: //FOUR-person craft
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.Hard[PAD_A][Mission_Capsule] == MANNED_HW_FOUR_MAN_CAPSULE 
                            && mission.Man[PAD_A][0] != -1) {
                            Rec_Change = 0;
                        }
                    } else if (mission.Hard[PAD_A][Mission_Capsule] == MANNED_HW_FOUR_MAN_CAPSULE 
                               && mission.Man[PAD_A][0] != -1) {
                        Rec_Change = 1;
                    }

                    break;

                case 18: //Spacecraft Prestige Points
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;

                    Rec_Change = -1;
                    if (Ty != 1) break;
                    
                    for (int craft = 0; craft < 5; craft++) {
                        temp = 0;
                        Rec_Change = -1;

                        for (int loop = 0; loop < pData.PastMissionCount; loop++) {
                            if (pData.History[loop].Prestige <= 0) continue;

                            if (pData.History[loop].Hard[PAD_A][Mission_Capsule] == craft) {
                                temp += pData.History[loop].Prestige;
                                Rec_Change = 1;
                            }
                        }

                        for (int loop = 0; loop < 3; loop++) {
                            if (rec[k][loop].program == craft && rec[k][loop].tag == temp) {
                                Rec_Change = -1;
                            }
                        }

                        if (Rec_Change != 1) continue;
                        
                        switch (rec[k][0].place) {
                        case 0:
                            WriteRecord(i, craft, k, temp);
                            break;

                        case 1:
                            WriteRecord(i, craft, k, temp);
                            
                            if (rec[k][0].tag > temp) break;
                            WriteRecord(i, craft, k, temp);
                            SwapRec(k, 1, 0);

                            break;

                        case 2:
                            WriteRecord(i, craft, k, temp);
                            
                            if (rec[k][1].tag > temp) break;
                            SwapRec(k, 2, 1);

                            if (rec[k][0].tag > temp) break;
                            SwapRec(k, 1, 0);

                            break;

                        case 3:
                            if (rec[k][2].tag > temp) break;
                            --rec[k][0].place;
                            WriteRecord(i, craft, k, temp);

                            if (rec[k][1].tag > temp) break;
                            SwapRec(k, 2, 1);

                            if (rec[k][0].tag > temp) break;
                            SwapRec(k, 1, 0);

                            break;

                        default:
                            break;
                        } //switch (rec[k][0].place)
                    }  //for (cra

                    Rec_Change = -1;
                    break;

                case 19: //Earliest Manned Lunar Pass
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_LunarPass) {
                            Rec_Change = 0;
                        }
                    } else {
                        if (mission.MissionCode == Mission_LunarPass) {
                            Rec_Change = 1;
                        }
                    }

                    break;

                case 20: //Earliest Manned Lunar Orbit
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_LunarOrbital ||
                            mission.MissionCode == Mission_Lunar_Orbital) {
                            Rec_Change = 0;
                        }
                    } else {
                        if (mission.MissionCode == Mission_LunarOrbital ||
                            mission.MissionCode == Mission_Lunar_Orbital) {
                            Rec_Change = 1;
                        }
                    }

                    break;

                case 21: //Earliest Manned Lunar Landing
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (MLL(i, j) == 1 || (i == 1 && mission.MissionCode == Mission_Soyuz_LL)) {
                            Rec_Change = 0;
                        }
                    } else {
                        if (MLL(i, j) == 1 || (i == 1 && mission.MissionCode == Mission_Soyuz_LL)) {
                            Rec_Change = 1;
                        }
                    }

                    break;

                case 22: //Fewest Casualties In A Game
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;

                    if (Ty != 1) break;
                    
                    temp = std::count_if(pData.Pool, pData.Pool + pData.AstroCount,
                                         [](auto& spaceman){return spaceman.Status == AST_ST_DEAD;});

                    if (rec[k][0].place == 0) {
                        Rec_Change = 0;
                    } else {
                        Rec_Change = 4;
                    }

                    break;

                case 23: //Most Casualties In A Game
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;

                    if (Ty != 1) break;
                    
                    temp = std::count_if(pData.Pool, pData.Pool + pData.AstroCount,
                                         [](auto& spaceman){return spaceman.Status == AST_ST_DEAD;});

                    if (rec[k][0].place == 0) {
                        Rec_Change = 0;
                    } else {
                        Rec_Change = 3;
                    }

                    break;

                case 24: //Highest Safety Average (Lunar Mission)
                    // TODO
                    break;

                case 25: //Lowest Safety Average (Lunar Mission)
                    // TODO
                    break;

                case 26: //Earliest Docking Mission
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (IsDocking(mission.MissionCode)) {
                            Rec_Change = 0;
                        }
                    } else if (IsDocking(mission.MissionCode)) {
                        Rec_Change = 1;
                    }

                    break;

                case 27: //Earliest Manned Docking
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (mission.MissionCode != Mission_U_Orbital_D 
                        && mission.MissionCode != Mission_Jt_Unmanned_Orbital_Docking 
                        && IsDocking(mission.MissionCode)) {
                        
                        if (rec[k][0].place == 0) {
                            Rec_Change = 0;
                        } else  {
                            Rec_Change = 1;
                        }
                    }

                    break;

                case 28: //Earliest Orbital Laboratory
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_Jt_OrbitingLab 
                            || mission.MissionCode == Mission_Jt_OrbitingLab_EVA) {
                            Rec_Change = 0;
                        }
                    } else if (mission.MissionCode == Mission_Jt_OrbitingLab 
                               || mission.MissionCode == Mission_Jt_OrbitingLab_EVA) {
                        Rec_Change = 1;
                    }

                    break;

                case 29: //Longest Mission Duration
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;
                    temp = mission.Duration;

                    if (rec[k][0].place == 0) {
                        if (temp > 0) {
                            Rec_Change = 0;
                        }
                    } else if (temp > 0) {
                        Rec_Change = 5;
                    } else {
                        Rec_Change = -1;
                    }

                    break;

                case 30: //Single Mission Prestige
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;
                    temp = mission.Prestige;

                    if (rec[k][0].place == 0) {
                        if (temp > 0) {
                            Rec_Change = 0;
                        }
                    } else if (temp > 0) {
                        Rec_Change = 3;
                    } else {
                        Rec_Change = -1;
                    }

                    break;

                case 31: //Fewest Missions
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;

                    if (Ty != 1) break;
                    
                    temp = pData.PastMissionCount;

                    if (rec[k][0].place == 0) {
                        if (temp > 0) {
                            Rec_Change = 0;
                        }
                    } else if (temp > 0) {
                        Rec_Change = 4;
                    } else {
                        Rec_Change = -1;
                    }

                    break;

                case 32: //Most Missions
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;

                    if (Ty != 1) break;
                    
                    temp = pData.PastMissionCount;

                    if (rec[k][0].place == 0) {
                        if (temp > 0) {
                            Rec_Change = 0;
                        }
                    } else if (temp > 0) {
                        Rec_Change = 3;
                    }

                    break;

                case 33: //Most Space Missions Attempted
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;

                    if (Ty != 1) break;
                    
                    temp = std::count_if(pData.History, pData.History + pData.PastMissionCount,
                                         [](MissionType& mission){return mission.Duration > 0;});

                    if (rec[k][0].place == 0) {
                        Rec_Change = 0;
                    } else {
                        Rec_Change = 3;
                    }

                    break;

                case 34: //Most Successful Space Missions
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;

                    if (Ty != 1) break;
                    
                    temp = 0;

                    for (int loop = 0; loop < pData.PastMissionCount; loop++) {
                        if (pData.History[loop].Duration > 0 && CheckSucess(i, loop) == 1) {
                            ++temp;
                        }
                    }

                    if (rec[k][0].place == 0) {
                        Rec_Change = 0;
                    } else {
                        Rec_Change = 3;
                    }

                    break;

                case 35: //Most Missions (MALE)
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;
                    max = 0;

                    Rec_Change = -1;
                    
                    if (Ty != 1) break;
                    
                    temp = 0;
                    max = 1;

                    for (int loop = 0; loop < pData.AstroCount; loop++) {
                        if (pData.Pool[loop].Missions < max || pData.Pool[loop].Sex != 0) continue;

                        if (rec[k][0].place == 0) {
                            Rec_Change = 0;
                        } else {
                            Rec_Change = 3;
                        }
                        RecChange(i, j, k, loop, pData.Pool[loop].Missions, Rec_Change);
                    }

                    Rec_Change = -1;
                    break;

                case 36: //Most Missions (FEMALE)
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    Rec_Change = -1;
                    if (Ty != 1) break;
                    
                    temp = 0;
                    max = 1;

                    for (int loop = 0; loop < pData.AstroCount; loop++) {
                        if (pData.Pool[loop].Missions < max || pData.Pool[loop].Sex != 1) continue;
                        
                        if (rec[k][0].place == 0) {
                            Rec_Change = 0;
                        } else {
                            Rec_Change = 3;
                        }
                        RecChange(i, j, k, loop, pData.Pool[loop].Missions, Rec_Change);
                    }

                    Rec_Change = -1;
                    break;

                case 37: //Most Prestige (MALE)
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    Rec_Change = -1;
                    if (Ty != 1) break;
                    
                    temp = 0;
                    max = 1;

                    for (int loop = 0; loop < pData.AstroCount; loop++) {
                        if (pData.Pool[loop].Prestige < max || pData.Pool[loop].Sex != 0) continue;
                        
                        if (rec[k][0].place == 0) {
                            Rec_Change = 0;
                        } else {
                            Rec_Change = 3;
                        }
                        RecChange(i, j, k, loop, pData.Pool[loop].Prestige, Rec_Change);
                    }

                    Rec_Change = -1;

                    break;

                case 38: //Most Prestige (FEMALE)
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    Rec_Change = -1;
                    if (Ty != 1) break;
                    
                    temp = 0;
                    max = 1;

                    for (int loop = 0; loop < pData.AstroCount; loop++) {
                        if (pData.Pool[loop].Prestige < max || pData.Pool[loop].Sex != 1) continue;
                        
                        if (rec[k][0].place == 0) {
                            Rec_Change = 0;
                        } else {
                            Rec_Change = 3;
                        }
                        RecChange(i, j, k, loop, pData.Pool[loop].Prestige, Rec_Change);
                    }

                    Rec_Change = -1;
                    break;

                case 39: //Most Days (MALE)
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    Rec_Change = -1;
                    if (Ty != 1) break;
                    
                    temp = 0;
                    max = 1;

                    for (int loop = 0; loop < pData.AstroCount; loop++) {
                        auto& spaceman = pData.Pool[loop];
                        if (spaceman.Days < max || spaceman.Sex != 0) continue;

                        if (rec[k][0].place == 0) {
                            Rec_Change = 0;
                        } else {
                            Rec_Change = 3;
                        }

                        RecChange(i, j, k, loop, spaceman.Days, Rec_Change);
                    }

                    Rec_Change = -1;
                    break;

                case 40: //Most Days (FEMALE)
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    if (Ty == 1) {
                        temp = 0;
                        max = 1;

                        for (int loop = 0; loop < pData.AstroCount; loop++) {
                            auto& spaceman = pData.Pool[loop];
                            if (spaceman.Days < max && spaceman.Sex != 1) continue;
                            
                            if (rec[k][0].place == 0) {
                                Rec_Change = 0;
                            } else {
                                Rec_Change = 3;
                            }
                            RecChange(i, j, k, loop, spaceman.Days, Rec_Change);
                        }
                    }

                    Rec_Change = -1;

                    break;

                case 41: //Longest Term Of Duty
                    rec[k][0].type = 2;
                    rec[k][1].type = 2;
                    rec[k][2].type = 2;

                    Rec_Change = -1;
                    if (Ty != 1) break;
                    
                    temp = 0;
                    max = 1;

                    for (int loop = 0; loop < pData.AstroCount; loop++) {
                        auto& spaceman = pData.Pool[loop];
                        if (spaceman.Active < max) continue;
                        
                        if (rec[k][0].place == 0) {
                            Rec_Change = 0;
                        } else {
                            Rec_Change = 3;
                        }

                        RecChange(i, j, k, loop, spaceman.Active, Rec_Change);
                    }

                    Rec_Change = -1;
                    break;

                case 42: //Highest Rating
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;
                    temp = max = 0;

                    if (Ty != 1) break;
                    
                    if (i == 0) {
                        temp = (unsigned)CalcScore(0, Data->Def.Lev1, Data->Def.Lev2);
                    } else if (i == 1) {
                        temp = (unsigned)CalcScore(1, Data->Def.Lev2, Data->Def.Lev1);
                    }

                    if (rec[k][0].place == 0) {
                        Rec_Change = 0;
                    } else {
                        Rec_Change = 3;
                    }

                    break;

                case 43: //Highest Average Budget
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;
                    temp = max = 0;

                    if (Ty != 1) break;
                    
                    for (int loop = 0; loop < 40; ++loop) {
                        if (pData.BudgetHistory[loop] != 0) {
                            temp += pData.BudgetHistory[loop];
                        } else if (loop >= 6 && max == 0) {
                            max = loop - 1;
                        }
                    }

                    temp = temp / max;

                    if (rec[k][0].place == 0) {
                        Rec_Change = 0;
                    } else {
                        Rec_Change = 3;
                    }

                    break;

                case 44: //Lowest Average Budget
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;
                    temp = max = 0;

                    if (Ty != 1) break;
                    
                    for (int loop = 0; loop < 40; ++loop) {
                        if (pData.BudgetHistory[loop] != 0) {
                            temp += pData.BudgetHistory[loop];
                        } else if (loop >= 6 && max == 0) {
                            max = loop - 1;
                        }
                    }

                    temp = temp / max;

                    if (rec[k][0].place == 0) {
                        Rec_Change = 0;
                    } else {
                        Rec_Change = 4;
                    }

                    break;

                case 45: //Highest Prestige Total
                    rec[k][0].type = 3;
                    rec[k][1].type = 3;
                    rec[k][2].type = 3;
                    temp = 0;

                    if (Ty != 1) break;
                    
                    for (int loop = 0; loop < pData.PastMissionCount; loop++) {
                        temp += pData.History[loop].Prestige;
                    }

                    if (rec[k][0].place == 0) {
                        Rec_Change = 0;
                    } else {
                        Rec_Change = 3;
                    }

                    break;

                case 46: //Earliest EOR Landing
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_Jt_LunarLanding_LOR) {
                            Rec_Change = 0;
                        }
                    } else {
                        if (mission.MissionCode == Mission_Jt_LunarLanding_LOR) {
                            Rec_Change = 1;
                        }
                    }

                    break;

                case 47: //Earliest LOR Landing
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_Jt_LunarLanding_EOR) {
                            Rec_Change = 0;
                        }
                    } else {
                        if (mission.MissionCode == Mission_Jt_LunarLanding_EOR) {
                            Rec_Change = 1;
                        }
                    }

                    break;

                case 48: //Earliest Direct Ascent Landing
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_DirectAscent_LL) {
                            Rec_Change = 0;
                        }
                    } else {
                        if (mission.MissionCode == Mission_DirectAscent_LL) {
                            Rec_Change = 1;
                        }
                    }

                    break;

                case 49: //Earliest Historical Lunar Landing
                    rec[k][0].type = 1;
                    rec[k][1].type = 1;
                    rec[k][2].type = 1;

                    if (rec[k][0].place == 0) {
                        if (mission.MissionCode == Mission_HistoricalLanding) {
                            Rec_Change = 0;
                        }
                    } else {
                        if (mission.MissionCode == Mission_HistoricalLanding) {
                            Rec_Change = 1;
                        }
                    }

                    break;

                case 50: //Total US Wins
                    if (Ty != 1) break;
                    
                    if (MLL(i, j) == 1 
                        && Data->Prestige[Prestige_MannedLunarLanding].Place == 0) {
                        ++rec[k][0].tag;
                        rec[k][0].place = 1;
                        rec[k][0].type = 3;
                        rec[k][0].country = 0;
                        Rec_Change = -1;
                    }

                    break;

                case 51: //Total SOVIET Wins
                    if (Ty != 1) break;
                    
                    if ((MLL(i, j) == 1 
                          || (i == 1 
                              && mission.MissionCode == Mission_Soyuz_LL)) 
                        && Data->Prestige[Prestige_MannedLunarLanding].Place == 1) {
                        ++rec[k][0].tag;
                        rec[k][0].place = 1;
                        rec[k][0].type = 3;
                        rec[k][0].country = 1;
                        Rec_Change = -1;
                    }

                    break;

                case 52: //Total LOR Wins
                    if (Ty != 1) break;
                    if (mission.MissionCode != Mission_Jt_LunarLanding_EOR) break;
                    
                    ++rec[k][0].tag;
                    rec[k][0].country = 2;
                    rec[k][0].type = 3;
                    Rec_Change = -1;

                    break;

                case 53: //Total EOR Wins
                    if (Ty != 1) break;
                    if (mission.MissionCode != Mission_Jt_LunarLanding_LOR) break;
                    
                    ++rec[k][0].tag;
                    rec[k][0].country = 2;
                    rec[k][0].type = 3;
                    Rec_Change = -1;

                    break;

                case 54:  //Total Direct Ascent Wins
                    if (Ty != 1) break;
                    if (mission.MissionCode != Mission_DirectAscent_LL) break;
                    
                    ++rec[k][0].tag;
                    rec[k][0].country = 2;
                    rec[k][0].type = 3;
                    Rec_Change = -1;

                    break;

                case 55: //Total Historical Wins
                    if (Ty != 1) break;
                    if (mission.MissionCode != Mission_HistoricalLanding) break;
                    
                    ++rec[k][0].tag;
                    rec[k][0].country = 2;
                    rec[k][0].type = 3;
                    Rec_Change = -1;

                    break;

                default:
                    break;
                }  // end switch

                if (! (k >= 35 && k <= 41) 
                    && k != 18) {
                    RecChange(i, j, k, temp, max, Rec_Change);
                }
            }
        }
    }

    //Change and Update Records
    ExportRecordTable();
}

void RecChange(int i, int j, int k, int temp, int max, char Rec_Change)
{
    auto& pData = Data->P[i];
    auto& mission = pData.History[j];
    /* XXX: SEGFAULT - some record entries are null (rec[k][loop].astro) */

    if (Rec_Change != 0)
    {
        for (int loop = 0; loop < 3; loop++) {
            if (rec[k][loop].type == 3 && rec[k][loop].tag == temp && (rec[k][loop].country == i)) {
                return;
            }

            // rec[k][loop].country is sometimes -1
            // guard against this case
            // (is this the issue cited above?)
            if (rec[k][loop].type == 2 
                && rec[k][loop].country >= 0 
                && strcmp(rec[k][loop].astro
                          , Data->P[rec[k][loop].country].Pool[temp].Name) == 0) {
                return;
            }
        }
    }

    switch (Rec_Change) {
    case 0:
        if (k >= 35 && k <= 41) {
            j = temp;  // holds astronaut
            temp = max;  // holds record
        }

        WriteRecord(i, j, k, temp);
        break;

    case 1: case 2:
        if (Rec_Change == 2) {
            temp = mission.Man[PAD_A][max];
        } else {
            temp = mission.Man[PAD_A][0];
        }
        auto is_worse = [](Record_Entry& old_record, MissionType& mission){
            if (rec[k][0].yr < mission.MissionYear) return true;
            if (rec[k][0].yr > mission.MissionYear) return false;
            
            if (rec[k][0].month < mission.Month) return true;
            if (rec[k][0].month > mission.Month) return false;

            return false;
        }

        switch (rec[k][0].place) {
        case 1:
            WriteRecord(i, j, k, temp);
            
            if (is_worse(rec[k][0], mission)) break;
            
            SwapRec(k, 1, 0);

            break;

        case 2:
            WriteRecord(i, j, k, temp);
            
            if (is_worse(rec[k][1], mission)) break;
            
            SwapRec(k, 2, 1);

            if (is_worse(rec[k][0], mission)) break;
            
            SwapRec(k, 1, 0);

            break;

        case 3:
            if (is_worse(rec[k][2], mission)) break;
            
            --rec[k][0].place;
            WriteRecord(i, j, k, temp);

            if (is_worse(rec[k][1], mission)) break;
            
            SwapRec(k, 2, 1);

            if (is_worse(rec[k][0], mission)) break;
                        
            SwapRec(k, 1, 0);

            break;

        default:
            break;
        }

        break;

    case 3:
        if (k >= 35 && k <= 41) {
            j = temp;  // holds astronaut
            temp = max;  // holds record
        }

        switch (rec[k][0].place) {
        case 1:
            WriteRecord(i, j, k, temp);
            if (rec[k][0].tag <= temp) {
                SwapRec(k, 1, 0);
            }
            break;

        case 2:
            WriteRecord(i, j, k, temp);
            
            if (rec[k][1].tag > temp) break;
            
            SwapRec(k, 2, 1);

            if (rec[k][0].tag > temp) break;
            
            SwapRec(k, 1, 0);

            break;

        case 3:
            if (rec[k][2].tag > temp) break;
            
            --rec[k][0].place;
            WriteRecord(i, j, k, temp);

            if (rec[k][1].tag > temp) break;
            
            SwapRec(k, 2, 1);

            if (rec[k][0].tag > temp) break;
            
            SwapRec(k, 1, 0);

            break;

        default:
            break;
        }

        break;

    case 4:
        switch (rec[k][0].place) {
        case 1:
            WriteRecord(i, j, k, temp);
            
            if (rec[k][0].tag <= temp) break;
            
            SwapRec(k, 1, 0);

            break;

        case 2:  // kludge cases 35-40 max holds tag temp holds astro
            WriteRecord(i, j, k, temp);
            
            if (rec[k][1].tag <= temp) break;
            
            SwapRec(k, 2, 1);

            if (rec[k][0].tag <= temp) break;
            
            SwapRec(k, 1, 0);

            break;

        case 3:
            if (rec[k][2].tag <= temp) break;
            
            --rec[k][0].place;
            WriteRecord(i, j, k, temp);

            if (rec[k][1].tag <= temp) break;
            
            SwapRec(k, 2, 1);

            if (rec[k][0].tag <= temp) break;
            
            SwapRec(k, 1, 0);

            break;

        default:
            break;
        }

        break;

    case 5:
        {
        auto is_worse = [](Record_Entry& old_record, int temp, MissionType& mission){
            if (old_record.tag > temp) return true;
            if (old_record.tag < temp) return false;

            if (old_record.yr < mission.MissionYear) return true;
            if (old_record.yr > mission.MissionYear) return false;
            
            if (old_record.month < mission.Month) return true;
            if (old_record.month > mission.Month) return false;

            return false;
        }
        bool hold = false;

        switch (rec[k][0].place) {
        case 1:
            WriteRecord(i, j, k, temp);
            
            if (is_worse(rec[k][0], temp, mission)) break;
            
            SwapRec(k, 1, 0);

            break;

        case 2:
            WriteRecord(i, j, k, temp);
            
            if (is_worse(rec[k][1], temp, mission)) break;

            SwapRec(k, 2, 1);

            if (is_worse(rec[k][0], temp, mission)) break;
            
            SwapRec(k, 1, 0);

            break;

        case 3:
            if (is_worse(rec[k][2], temp, mission)) break;

            --rec[k][0].place;
            WriteRecord(i, j, k, temp);

            if (is_worse(rec[k][1], temp, mission)) break;
                
            SwapRec(k, 2, 1);
                
            if (is_worse(rec[k][0], temp, mission)) break;
            
            SwapRec(k, 1, 0);

            break;

        default:
            break;
        }  //end case #5 switch

        break;
        } // end of case 5 itself

    default:
        break;
    }  // end switch (Rec_Change)
}


/**
 * Read packed Record_Entry data from a file into an instance.
 */
void ImportRecordTable()
{
    FILE* fin = sOpen("RECORDS.DAT", "rb", FT_SAVE);
    for (int i=0; i<56; ++i)
    {
        for (int j=0; j<3; ++j)
        {
            auto& dst = rec[i][j];
            fread(&dst.country, sizeof(dst.country), 1, fin);
            fread(&dst.month, sizeof(dst.month), 1, fin);
            fread(&dst.yr, sizeof(dst.yr), 1, fin);
            fread(&dst.program, sizeof(dst.program), 1, fin);
            fread(&dst.tag, sizeof(dst.tag), 1, fin);
            fread(&dst.type, sizeof(dst.type), 1, fin);
            fread(&dst.place, sizeof(dst.place), 1, fin);
            fread(&dst.name[0], sizeof(dst.name), 1, fin);
            fread(&dst.astro[0], sizeof(dst.astro), 1, fin);
            Swap16bit(dst.tag);
        }
    }
    fclose(fin);
}


/**
 * Write a Record_Entry instance as a packed byte stream to a file.
 *
 * Outputs using little-endian ordering.
 * TODO: JSON
 */
void ExportRecordTable()
{
    FILE* fout = sOpen("RECORDS.DAT", "wb", FT_SAVE);
    if (fout == nullptr) {
        /* XXX: very drastic */
        LOG_CRITICAL("can't create required file RECORDS.DAT");
        exit(EXIT_FAILURE);
    }

    for (int i=0; i<56; ++i)
    {
        for (int j=0; j<3; ++j)
        {
            auto& src = rec[i][j];
            uint16_t tempTag = src.tag;
            Swap16bit(tempTag);
            fwrite(&src.country, sizeof(src.country), 1, fout);
            fwrite(&src.month, sizeof(src.month), 1, fout);
            fwrite(&src.yr, sizeof(src.yr), 1, fout);
            fwrite(&src.program, sizeof(src.program), 1, fout);
            fwrite(&tempTag, sizeof(tempTag), 1, fout);
            fwrite(&src.type, sizeof(src.type), 1, fout);
            fwrite(&src.place, sizeof(src.place), 1, fout);
            fwrite(&src.name[0], sizeof(src.name), 1, fout);
            fwrite(&src.astro[0], sizeof(src.astro), 1, fout);
        }
    }
    fclose(fout);
}
