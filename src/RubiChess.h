/*
  RubiChess is a UCI chess playing engine by Andreas Matthies.

  RubiChess is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  RubiChess is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define VERNUM "1.7"
//#define VERSTABLE

#if 0
#define STATISTICS
#endif

#if 0
#define SDEBUG
#endif

#if 0
#define TDEBUG
#endif

#if 0
#define EVALTUNE
#endif

#if 0
#define FINDMEMORYLEAKS
#endif

#ifdef FINDMEMORYLEAKS
#ifdef _DEBUG  
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)  
#else  
#define DEBUG_CLIENTBLOCK  
#endif // _DEBUG  

#ifdef _DEBUG  
#define new DEBUG_CLIENTBLOCK  
#endif  

#define _CRTDBG_MAP_ALLOC
#endif

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <string.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <thread>
#include <map>
#include <time.h>
#include <array>
#include <bitset>
#include <limits.h>
#include <math.h>
#include <regex>

#ifdef _WIN32

#include <conio.h>
#include <AclAPI.h>
#include <intrin.h>
#include <Windows.h>

#ifdef STACKDEBUG
#include <DbgHelp.h>
#define myassert(expression, pos, num, ...) (void)((!!(expression)) ||   (GetStackWalk(pos, (const char*)(#expression), (const char*)(__FILE__), (int)(__LINE__), (num), ##__VA_ARGS__), 0))
#else
#define myassert(expression, pos, num, ...) (void)(0)
#endif

#ifdef FINDMEMORYLEAKS
#include <crtdbg.h>
#endif

#else //_WIN32

#define myassert(expression, pos, num, ...) (void)(0)
#define sprintf_s sprintf
void Sleep(long x);

#endif

using namespace std;

#ifdef _MSC_VER
#ifdef EVALTUNE
#define PREFETCH(a) (void)(0)
#else
#define PREFETCH(a) _mm_prefetch((char*)(a), _MM_HINT_T0)
#endif
#else
#ifdef EVALTUNE
#define PREFETCH(a) (void)(0)
#else
#define PREFETCH(a) __builtin_prefetch(a)
#endif
#endif

#ifndef VERSTABLE
#ifdef GITVER
#define VERSION VERNUM "-dev " GITVER
#else
#define VERSION VERNUM "-dev"
#endif
#else
#define VERSION VERNUM
#endif
#define ENGINEVER "RubiChess " VERSION
#ifdef GITID
#define BUILD __DATE__ " " __TIME__ " commit " GITID
#else
#define BUILD __DATE__ " " __TIME__
#endif

#define BITSET(x) (1ULL << (x))
#define MORETHANONE(x) ((x) & ((x) - 1)) 
#define ONEORZERO(x) (!MORETHANONE(x))
#ifdef _MSC_VER
#define GETLSB(i,x) _BitScanForward64((DWORD*)&(i), (x))
inline int pullLsb(unsigned long long *x) {
    DWORD i;
    _BitScanForward64(&i, *x);
    *x &= *x - 1;  // this is faster than *x ^= (1ULL << i);
    return i;
}
#define GETMSB(i,x) _BitScanReverse64((DWORD*)&(i), (x))
inline int pullMsb(unsigned long long *x) {
    DWORD i;
    _BitScanReverse64(&i, *x);
    *x ^= (1ULL << i);
    return i;
}
#define POPCOUNT(x) (int)(__popcnt64(x))
#else
#define GETLSB(i,x) (i = __builtin_ctzll(x))
inline int pullLsb(unsigned long long *x) {
    int i = __builtin_ctzll(*x);
    *x &= *x - 1;  // this is faster than *x ^= (1ULL << i);
    return i;
}
#define GETMSB(i,x) (i = (63 - __builtin_clzll(x)))
inline int pullMsb(unsigned long long *x) {
    int i = 63 - __builtin_clzll(*x);
    *x ^= (1ULL << i);
    return i;
}
#define POPCOUNT(x) __builtin_popcountll(x)
#endif

enum { WHITE, BLACK };
#define WHITEBB 0x55aa55aa55aa55aa
#define BLACKBB 0xaa55aa55aa55aa55
#define FLANKLEFT  0x0f0f0f0f0f0f0f0f
#define FLANKRIGHT 0xf0f0f0f0f0f0f0f0
#define CENTER 0x0000001818000000
#define OUTPOSTAREA(s) ((s) ? 0x000000ffffff0000 : 0x0000ffffff000000)
#define RANK(x) ((x) >> 3)
#define RRANK(x,s) ((s) ? ((x) >> 3) ^ 7 : ((x) >> 3))
#define FILE(x) ((x) & 0x7)
#define INDEX(r,f) (((r) << 3) | (f))
#define BORDERDIST(f) ((f) > 3 ? 7 - (f) : (f))
#define PROMOTERANK(x) (RANK(x) == 0 || RANK(x) == 7)
#define PROMOTERANKBB 0xff000000000000ff
#define RANK2(s) ((s) ? 0x00ff000000000000 : 0x000000000000ff00)
#define RANK3(s) ((s) ? 0x0000ff0000000000 : 0x0000000000ff0000)
#define RANK7(s) ((s) ? 0x000000000000ff00 : 0x00ff000000000000)
#define RANK8(s) ((s) ? 0x00000000000000ff : 0xff00000000000000)
#define FILEABB 0x0101010101010101
#define FILEHBB 0x8080808080808080
#define ISOUTERFILE(x) (FILE(x) == 0 || FILE(x) == 7)
#define ISNEIGHBOUR(x,y) ((x) >= 0 && (x) < 64 && (y) >= 0 && (y) < 64 && abs(RANK(x) - RANK(y)) <= 1 && abs(FILE(x) - FILE(y)) <= 1)

#define S2MSIGN(s) (s ? -1 : 1)


typedef unsigned long long U64;
typedef signed long long S64;

// Forward definitions
class transposition;
class chessposition;
class searchthread;
struct pawnhashentry;


//
// eval stuff
//

#define GETMGVAL(v) ((int16_t)(((uint32_t)(v) + 0x8000) >> 16))
#define GETEGVAL(v) ((int16_t)((v) & 0xffff))

#ifdef EVALTUNE

class eval;
class sqevallist
{
public:
    sqevallist() {
        count = 0;
    }
    eval* ptr[1024];
    int count;
    void store(eval *s) { ptr[count++] = s; }
};

extern sqevallist sqglobal;

class eval {
public:
    int type;  // 0=linear mg->eg  1=constant  2=squre  3=only eg (0->eg)
    int groupindex;
    int32_t v;
    int g[2];
    eval() { v = g[0] = g[1] = 0; }
    eval(int m, int e) {
        v = ((int32_t)((uint32_t)(m) << 16) + (e)); g[0] = 0; type = 0;
    }
    eval(int t, int i, int x) { type = t; groupindex = i; v = x; sqglobal.store(this); }
    eval(int x) { v = (x); g[0] = 0; type = 1; }
    bool operator !=(const eval &x) { return this->type != x.type || this->v != x.v; }
    operator int() const { return v; }
    void addGrad(int i, int s = 0) { this->g[s] += i; }
    int getGrad(int s = 0) { return this->g[s]; }
    void resetGrad() { g[0] = g[1] = 0; }
    void replace(int i, int16_t b) { if (!i) v = ((int32_t)((uint32_t)GETMGVAL(v) << 16) + b); else v = ((int32_t)((uint32_t)b << 16) + GETEGVAL(v)); }
    void replace(int16_t b) { v = b; }
};


#define VALUE(m, e) eval(m, e)
#define EVAL(e, f) ((e).addGrad(f), (e) * (f))
#define SQVALUE(i, v) eval(2, i, v)
#define SQEVAL(e, f, s) ((e).addGrad(f, s), (e) * (f))
#define SQRESULT(v,s) ( v > 0 ? ((int32_t)((uint32_t)((v) * (v) * S2MSIGN(s) / 2048) << 16) + ((v) * S2MSIGN(s) / 16)) : 0 )
#define CVALUE(v) eval(v)
#define CEVAL(e, f) ((e).addGrad(f), (e) * (f))
#define EVALUE(v) eval(3, 0, v)
#define EEVAL(e, f) ((e).addGrad(f), (e) * (f))
#else // EVALTUNE

#define SQVALUE(i, v) (v)
#define VALUE(m, e) ((int32_t)((uint32_t)(m) << 16) + (e))
#define EVAL(e, f) ((e) * (f))
#define SQEVAL(e, f, s) ((e) * (f))
#define SQRESULT(v,s) ( v > 0 ? VALUE((v) * (v) * S2MSIGN(s) / 2048, (v) * S2MSIGN(s) / 16) : 0 )
#define CVALUE(v) (v)
#define CEVAL(e, f) ((e) * (f))
#define EVALUE(e) VALUE(0, e)
#define EEVAL(e, f) ((e) * (f))
typedef const int32_t eval;
#endif

#define PSQTINDEX(i,s) ((s) ? (i) : (i) ^ 0x38)

#define TAPEREDANDSCALEDEVAL(s, p, c) ((GETMGVAL(s) * (256 - (p)) + GETEGVAL(s) * (p) * (c) / SCALE_NORMAL) / 256)

struct evalparamset {
    // Tuned with Lichess-quiet (psqt), lc0games (kingdanger), manually (complex) and Laser games (everything else)
    eval eComplexpawnsbonus =  EVALUE(   4);
    eval eComplexpawnflanksbonus =  EVALUE(  66);
    eval eComplexonlypawnsbonus =  EVALUE(  71);
    eval eComplexadjust =  EVALUE(-100);
    eval eTempo =  CVALUE(  20);
    eval eKingpinpenalty[6] = {  VALUE(   0,   0), VALUE(   0,   0), VALUE(  38, -74), VALUE(  65, -61), VALUE( -29,  68), VALUE( -44, 163)  };
    eval ePawnstormblocked[4][5] = {
        {  VALUE(   0,   0), VALUE(   0,   0), VALUE( -11,  14), VALUE(  25,   0), VALUE(  32, -11)  },
        {  VALUE(   0,   0), VALUE(   0,   0), VALUE( -10,  13), VALUE(   6,   5), VALUE(  -2,   9)  },
        {  VALUE(   0,   0), VALUE(   0,   0), VALUE(  -6,   3), VALUE( -12,  10), VALUE( -10,   6)  },
        {  VALUE(   0,   0), VALUE(   0,   0), VALUE( -13,  11), VALUE( -24,  13), VALUE( -21,   3)  }
    };
    eval ePawnstormfree[4][5] = {
        {  VALUE(  22,  32), VALUE(  68,  34), VALUE(  19,   6), VALUE( -10,   9), VALUE(   4,   3)  },
        {  VALUE( -19,  23), VALUE( -42,  39), VALUE( -36,  15), VALUE( -16,  18), VALUE(  -2,   9)  },
        {  VALUE(   8,  14), VALUE(  -8,  32), VALUE( -11,   6), VALUE( -18,  14), VALUE( -11,  11)  },
        {  VALUE(  16,  12), VALUE(  14,  25), VALUE( -15,   9), VALUE( -28,  23), VALUE( -29,  22)  }
    };
    eval ePawnpushthreatbonus =  VALUE(  17,  24);
    eval eSafepawnattackbonus =  VALUE(  73,  30);
    eval eHangingpiecepenalty =  VALUE( -23, -36);
    eval ePassedpawnbonus[4][8] = {
        {  VALUE(   0,   0), VALUE(  -3,  -3), VALUE( -11,   7), VALUE(   2,  13), VALUE(  38,  32), VALUE(  88,  78), VALUE(  17,  84), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -21,   5), VALUE( -13,   7), VALUE( -17,   4), VALUE(  15,  10), VALUE(  70,  42), VALUE(   4,  44), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  12,  -6), VALUE(   0,   6), VALUE(   7,  34), VALUE(  36, 110), VALUE(  77, 217), VALUE( 119, 222), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(   6,   0), VALUE(  -8,   6), VALUE(  -8,   8), VALUE(   9,  22), VALUE(  55,  58), VALUE(  10,  64), VALUE(   0,   0)  }
    };
    eval eKingsupportspasserbonus[7][8] = {
        {  VALUE(   0,   0), VALUE(  13,  15), VALUE( -16,  14), VALUE(   4,  -3), VALUE(  58, -22), VALUE(  72, -46), VALUE( 113, -58), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -11,  20), VALUE( -13,   4), VALUE(  -9,  -9), VALUE( -27, -18), VALUE(  29, -63), VALUE( 114, -77), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(   6,   6), VALUE(   2,  -9), VALUE(  -6, -23), VALUE( -23, -36), VALUE( -40, -63), VALUE(  95,-102), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  12,   2), VALUE(   7, -10), VALUE(   2, -22), VALUE( -24, -54), VALUE( -41, -81), VALUE(  -2, -95), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  22,  -2), VALUE(  15,  -4), VALUE(  21, -23), VALUE(   5, -60), VALUE( -45, -83), VALUE(  -5, -94), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  27, -12), VALUE(  35, -17), VALUE(  40, -32), VALUE(  22, -69), VALUE( -13,-100), VALUE( -26, -87), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  24,  -6), VALUE(  16,  -5), VALUE(  29, -22), VALUE(  23, -69), VALUE(  -9, -93), VALUE( -23,-102), VALUE(   0,   0)  }
    };
    eval eKingdefendspasserpenalty[7][8] = {
        {  VALUE(   0,   0), VALUE(  20,  20), VALUE(  44,  14), VALUE(   7,   9), VALUE(  24,   1), VALUE(  39,   8), VALUE(   7, -15), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(   8,  -1), VALUE(   4,   7), VALUE(  13,   7), VALUE(  23,  12), VALUE(  51,  26), VALUE(  34,  53), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  -8,   8), VALUE(  12,   0), VALUE(   8,  14), VALUE(  28,  30), VALUE(  41,  79), VALUE(  14, 101), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(   2,  -5), VALUE(   8,   2), VALUE(   2,  19), VALUE(  16,  63), VALUE(  30, 104), VALUE(   4, 141), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -10,  -8), VALUE(   1,   1), VALUE( -16,  33), VALUE(   4,  79), VALUE(  51, 113), VALUE(  37, 155), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -15,  -6), VALUE( -22,  10), VALUE( -19,  34), VALUE(   4,  77), VALUE(  43, 115), VALUE(  26, 152), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -44,   3), VALUE( -28,  11), VALUE( -24,  33), VALUE(   3,  80), VALUE(  48, 115), VALUE(  14, 163), VALUE(   0,   0)  }
    };
    eval ePotentialpassedpawnbonus[2][8] = {
        {  VALUE(   0,   0), VALUE(  28, -15), VALUE(  -3,  14), VALUE(  17,   5), VALUE(  37,  60), VALUE(  75,  81), VALUE(  44, 108), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  -2,   0), VALUE(   1,   2), VALUE(   6,   6), VALUE(  20,  25), VALUE(  53,   7), VALUE( -14,  10), VALUE(   0,   0)  }
    };
    eval eAttackingpawnbonus[8] = {  VALUE(   0,   0), VALUE( -48,  12), VALUE( -14,   4), VALUE( -14,  -6), VALUE( -14,  -6), VALUE( -15,   1), VALUE(   0,   0), VALUE(   0,   0)  };
    eval eIsolatedpawnpenalty =  VALUE( -13, -12);
    eval eDoublepawnpenalty =  VALUE(  -9, -21);
    eval eConnectedbonus[6][6] = {
        {  VALUE(   0,   0), VALUE(  11, -10), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(   3,   3), VALUE(   9,   0), VALUE(  17,  -3), VALUE(  23,  -4), VALUE(  19, -16)  },
        {  VALUE(   0,   0), VALUE(  12,   0), VALUE(  14,   4), VALUE(  19,   0), VALUE(  23,   5), VALUE(  -2,  -2)  },
        {  VALUE(   0,   0), VALUE(  15,  14), VALUE(  22,  13), VALUE(  24,   3), VALUE(  30,  12), VALUE(  15, -12)  },
        {  VALUE(   0,   0), VALUE(  61,  61), VALUE(  35,  48), VALUE(  69,  73), VALUE(  42,  56), VALUE( -57, 253)  },
        {  VALUE(   0,   0), VALUE( 131, 193), VALUE(  81, 107), VALUE(   7, 400), VALUE( 254,  33), VALUE(   0,   0)  }
    };
    eval eBackwardpawnpenalty =  VALUE( -11, -16);
    eval eDoublebishopbonus =  VALUE(  56,  38);
    eval ePawnblocksbishoppenalty =  VALUE( -10, -18);
    eval eBishopcentercontrolbonus =  VALUE(  25,  13);
    eval eMobilitybonus[4][28] = {
        {  VALUE(  16, -90), VALUE(  38, -26), VALUE(  51,   1), VALUE(  57,  13), VALUE(  64,  27), VALUE(  71,  37), VALUE(  77,  36), VALUE(  84,  36),
           VALUE(  86,  30), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0)  },
        {  VALUE(  -8, -25), VALUE(  23,  -7), VALUE(  45,   2), VALUE(  55,  21), VALUE(  65,  31), VALUE(  74,  35), VALUE(  80,  43), VALUE(  81,  50),
           VALUE(  86,  51), VALUE(  85,  58), VALUE(  86,  52), VALUE(  95,  55), VALUE( 100,  54), VALUE(  89,  60), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0)  },
        {  VALUE( -57,  11), VALUE(  14,  17), VALUE(  31,  57), VALUE(  34,  70), VALUE(  36,  82), VALUE(  38,  90), VALUE(  40,  92), VALUE(  46,  96),
           VALUE(  47, 101), VALUE(  50, 111), VALUE(  54, 109), VALUE(  50, 115), VALUE(  49, 118), VALUE(  50, 116), VALUE(  44, 116), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0)  },
        {  VALUE(-4097,  83), VALUE(  13,-157), VALUE(  -2,  31), VALUE(   3,  75), VALUE(   5,  94), VALUE(   4, 152), VALUE(   6, 161), VALUE(  11, 177),
           VALUE(  10, 192), VALUE(  15, 187), VALUE(  14, 212), VALUE(  17, 213), VALUE(  16, 227), VALUE(  18, 229), VALUE(  17, 242), VALUE(  19, 251),
           VALUE(  20, 253), VALUE(  21, 256), VALUE(  18, 263), VALUE(  13, 270), VALUE(  45, 244), VALUE(  67, 233), VALUE(  72, 241), VALUE(  80, 230),
           VALUE(  69, 264), VALUE( 108, 231), VALUE( 107, 230), VALUE( 114, 198)  }
    };
    eval eRookon7thbonus =  VALUE(  -1,  22);
    eval eMinorbehindpawn[6] = {  VALUE(   1,  14), VALUE(  12,  10), VALUE(  15,  11), VALUE(  24,   9), VALUE(  37,  11), VALUE(  89, 110)  };
    eval eSlideronfreefilebonus[2] = {  VALUE(  21,   7), VALUE(  43,   1)  };
    eval eMaterialvalue[7] = {  VALUE(   0,   0), VALUE( 100, 100), VALUE( 314, 314), VALUE( 314, 314), VALUE( 483, 483), VALUE( 913, 913), VALUE(   0,   0)  };
    eval eKingshieldbonus =  VALUE(  15,  -2);
    eval eWeakkingringpenalty =  SQVALUE(   1,  70);
    eval eKingattackweight[7] = {  SQVALUE(   1,   0), SQVALUE(   1,   0), SQVALUE(   1,  25), SQVALUE(   1,  11), SQVALUE(   1,  15), SQVALUE(   1,  42), SQVALUE(   1,   0)  };
    eval eSafecheckbonus[6] = {  SQVALUE(   1,   0), SQVALUE(   1,   0), SQVALUE(   1, 282), SQVALUE(   1,  55), SQVALUE(   1, 244), SQVALUE(   1, 210)  };
    eval eKingdangerbyqueen =  SQVALUE(   1,-163);
    eval eKingringattack[6] = {  SQVALUE(   1, 111), SQVALUE(   1,   0), SQVALUE(   1,  31), SQVALUE(   1,   0), SQVALUE(   1,   0), SQVALUE(   1, -15)  };
    eval ePsqt[7][64] = {
        {  VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0)  },
        {  VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999),
           VALUE( 147,  76), VALUE(  63,  95), VALUE( 122,  60), VALUE( 144,  41), VALUE( 125,  63), VALUE(  91,  79), VALUE(  26,  98), VALUE(  41, 108),
           VALUE( -17,  46), VALUE( -25,  39), VALUE(  -5,  16), VALUE(  -4,   1), VALUE(   6,   8), VALUE(  56,  10), VALUE(  -7,  45), VALUE( -23,  53),
           VALUE( -27,  39), VALUE( -23,  26), VALUE( -16,   9), VALUE( -15,   4), VALUE(   9,  -1), VALUE(   0,   2), VALUE( -19,  18), VALUE( -19,  21),
           VALUE( -31,  24), VALUE( -34,  20), VALUE( -17,  10), VALUE(  -5,   8), VALUE(   0,   7), VALUE(  -8,  10), VALUE( -22,  13), VALUE( -26,   9),
           VALUE( -27,  18), VALUE( -32,  13), VALUE( -15,   7), VALUE( -14,  12), VALUE(   0,  16), VALUE( -17,  10), VALUE( -12,   3), VALUE( -21,   5),
           VALUE( -28,  28), VALUE( -29,  22), VALUE( -21,  19), VALUE( -25,  16), VALUE( -14,  32), VALUE(   6,  14), VALUE(   2,   9), VALUE( -27,   9),
           VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999)  },
        {  VALUE(-135,  18), VALUE( -53,  45), VALUE( -56,  47), VALUE( -37,  44), VALUE(   4,  39), VALUE( -85,  28), VALUE(-131,  36), VALUE( -84, -38),
           VALUE( -31,  30), VALUE( -15,  50), VALUE(  19,  49), VALUE(  63,  30), VALUE(  16,  29), VALUE(  85,  13), VALUE(   9,  31), VALUE(  -8,  14),
           VALUE(  -1,  40), VALUE(  44,  44), VALUE(  42,  57), VALUE(  61,  57), VALUE(  94,  33), VALUE(  83,  30), VALUE(  64,  27), VALUE(  37,  18),
           VALUE(  13,  62), VALUE(  38,  50), VALUE(  61,  59), VALUE(  90,  53), VALUE(  66,  63), VALUE(  86,  51), VALUE(  40,  46), VALUE(  55,  33),
           VALUE(  16,  49), VALUE(  35,  52), VALUE(  49,  58), VALUE(  46,  74), VALUE(  60,  69), VALUE(  58,  56), VALUE(  64,  51), VALUE(  35,  55),
           VALUE( -27,  39), VALUE(  -1,  35), VALUE(  20,  35), VALUE(  17,  56), VALUE(  30,  50), VALUE(  30,  34), VALUE(  33,  30), VALUE(   6,  39),
           VALUE( -32,  54), VALUE( -19,  27), VALUE( -11,  40), VALUE(  15,  38), VALUE(  18,  33), VALUE(  15,  29), VALUE(   7,  43), VALUE(   1,  59),
           VALUE( -64,  50), VALUE(  -7,  41), VALUE( -24,  45), VALUE(   4,  48), VALUE(  12,  41), VALUE(  11,  28), VALUE(  -5,  42), VALUE( -32,  42)  },
        {  VALUE(  -8,  56), VALUE( -49,  95), VALUE(  32,  75), VALUE( -29,  93), VALUE( -42,  83), VALUE( -13,  79), VALUE( -21,  54), VALUE( -37,  52),
           VALUE(  -5,  61), VALUE( -18,  53), VALUE(   6,  74), VALUE(  31,  68), VALUE(  22,  64), VALUE(   3,  66), VALUE( -28,  43), VALUE( -21,  60),
           VALUE(  29,  67), VALUE(  36,  76), VALUE(  24,  55), VALUE(  64,  50), VALUE(  40,  57), VALUE(  72,  53), VALUE(  40,  67), VALUE(  67,  55),
           VALUE(  16,  80), VALUE(  44,  77), VALUE(  53,  68), VALUE(  62,  75), VALUE(  74,  66), VALUE(  44,  75), VALUE(  64,  65), VALUE(  14,  72),
           VALUE(  26,  72), VALUE(  24,  75), VALUE(  44,  75), VALUE(  60,  72), VALUE(  53,  73), VALUE(  50,  69), VALUE(  42,  59), VALUE(  68,  46),
           VALUE(  26,  46), VALUE(  49,  72), VALUE(  14,  57), VALUE(  32,  68), VALUE(  38,  75), VALUE(  22,  53), VALUE(  54,  54), VALUE(  40,  56),
           VALUE(  36,  52), VALUE(  17,  33), VALUE(  36,  42), VALUE(  17,  62), VALUE(  26,  60), VALUE(  45,  45), VALUE(  36,  37), VALUE(  44,  40),
           VALUE(  21,  31), VALUE(  48,  36), VALUE(  37,  53), VALUE(  15,  57), VALUE(  33,  63), VALUE(  16,  68), VALUE(  34,  55), VALUE(  31,  24)  },
        {  VALUE(  36, 113), VALUE( -12, 136), VALUE(  12, 131), VALUE(  12, 136), VALUE(   2, 123), VALUE(  20, 128), VALUE(  22, 121), VALUE(  54, 109),
           VALUE(   5, 114), VALUE(  -8, 125), VALUE(  10, 129), VALUE(  23, 120), VALUE(  15, 113), VALUE(  14, 114), VALUE(   9, 114), VALUE(  37, 102),
           VALUE(  -7, 141), VALUE(  21, 133), VALUE(  17, 138), VALUE(  21, 120), VALUE(  64, 103), VALUE(  64, 105), VALUE(  83, 106), VALUE(  47, 108),
           VALUE(  -7, 136), VALUE(   6, 138), VALUE(  18, 132), VALUE(  13, 127), VALUE(  22, 114), VALUE(  31, 113), VALUE(  33, 114), VALUE(  30, 111),
           VALUE(  -7, 119), VALUE( -15, 122), VALUE(  -8, 127), VALUE(   4, 116), VALUE(   1, 116), VALUE(  -7, 122), VALUE(  33,  96), VALUE(  16, 101),
           VALUE( -13, 108), VALUE( -11,  97), VALUE( -12, 113), VALUE(  -4, 109), VALUE(  10,  96), VALUE(  14,  93), VALUE(  47,  70), VALUE(  16,  82),
           VALUE(  -9, 101), VALUE( -15, 104), VALUE(   1, 104), VALUE(   1, 100), VALUE(  11,  95), VALUE(  20,  85), VALUE(  29,  79), VALUE(   4,  87),
           VALUE(   5, 102), VALUE(   0,  96), VALUE(   3,  97), VALUE(  10,  91), VALUE(  17,  83), VALUE(  19,  92), VALUE(  29,  82), VALUE(   7,  96)  },
        {  VALUE(  17, 139), VALUE( -31, 195), VALUE(  17, 178), VALUE(  47, 170), VALUE(  11, 205), VALUE(   6, 208), VALUE(   3, 217), VALUE( -11, 183),
           VALUE( -29, 208), VALUE( -54, 232), VALUE( -59, 269), VALUE( -40, 258), VALUE( -60, 277), VALUE( -45, 256), VALUE( -73, 301), VALUE(  22, 210),
           VALUE(  -7, 231), VALUE( -12, 234), VALUE( -19, 272), VALUE(  -4, 273), VALUE( -16, 279), VALUE(  -9, 265), VALUE( -22, 268), VALUE(   9, 230),
           VALUE( -12, 253), VALUE(   0, 261), VALUE(  -7, 253), VALUE( -17, 272), VALUE( -26, 292), VALUE(   6, 254), VALUE(  10, 293), VALUE(  14, 247),
           VALUE(  -2, 244), VALUE( -10, 265), VALUE(  -1, 243), VALUE(   1, 245), VALUE(   7, 235), VALUE(   7, 249), VALUE(  25, 238), VALUE(  22, 243),
           VALUE( -12, 234), VALUE(   7, 230), VALUE(  -4, 241), VALUE(  -2, 232), VALUE(   9, 227), VALUE(  14, 229), VALUE(  27, 234), VALUE(  16, 199),
           VALUE(  -4, 208), VALUE(  -2, 212), VALUE(  11, 206), VALUE(  16, 211), VALUE(  15, 215), VALUE(  18, 194), VALUE(  24, 178), VALUE(  38, 161),
           VALUE( -15, 208), VALUE( -15, 204), VALUE( -11, 223), VALUE(   8, 187), VALUE(  -2, 209), VALUE( -29, 222), VALUE(   8, 195), VALUE(  -7, 227)  },
        {  VALUE(-116,-112), VALUE(  22, -21), VALUE( -13,  20), VALUE( -84,  45), VALUE(-171,  79), VALUE(-115,  85), VALUE( -23,  -7), VALUE( -99, -75),
           VALUE(-107,  24), VALUE( -11,  61), VALUE( -23,  93), VALUE(-103,  87), VALUE(-108,  92), VALUE( -38,  78), VALUE(  -6,  73), VALUE(-141,  24),
           VALUE(-178,  28), VALUE( -20,  51), VALUE( -43,  67), VALUE( -69,  85), VALUE( -36,  81), VALUE( -10,  64), VALUE( -52,  61), VALUE(-131,  33),
           VALUE(-156,  13), VALUE( -33,  31), VALUE( -72,  59), VALUE( -69,  75), VALUE( -37,  71), VALUE( -32,  55), VALUE( -50,  41), VALUE(-158,  22),
           VALUE(-134,   6), VALUE( -81,  27), VALUE( -38,  46), VALUE( -85,  63), VALUE( -53,  54), VALUE( -54,  43), VALUE( -67,  22), VALUE(-160,   2),
           VALUE( -55, -23), VALUE( -27,   1), VALUE( -50,  25), VALUE( -58,  41), VALUE( -59,  44), VALUE( -42,  24), VALUE( -30,   4), VALUE( -61,  -7),
           VALUE(  34, -43), VALUE(   3,  -9), VALUE( -19,  13), VALUE( -43,  17), VALUE( -49,  20), VALUE( -29,  15), VALUE(   3,  -4), VALUE(   9, -28),
           VALUE(  16, -73), VALUE(  45, -54), VALUE(  28, -23), VALUE( -70,   2), VALUE( -11, -16), VALUE( -45,   1), VALUE(  18, -31), VALUE(  24, -67)  }
    };
};

#ifdef EVALTUNE

struct evalparamset_preload {
    // Use these values as starting point for the tuner
    eval eComplexpawnsbonus =  EVALUE(   4);
    eval eComplexpawnflanksbonus =  EVALUE(  66);
    eval eComplexonlypawnsbonus =  EVALUE(  71);
    eval eComplexadjust =  EVALUE(-100);
    eval eTempo =  CVALUE(  20);
    eval eKingpinpenalty[6] = {  VALUE(   0,   0), VALUE(   0,   0), VALUE(-118,-180), VALUE(-124,-181), VALUE(-138,-242), VALUE(-164,-711)  };
    eval ePawnstormblocked[4][5] = {
        {  VALUE(   0,   0), VALUE(   0,   0), VALUE(  11,   5), VALUE(  -5,  -2), VALUE(  -6,  -3)  },
        {  VALUE(   0,   0), VALUE(   0,   0), VALUE(   5,   1), VALUE(  -5,   4), VALUE(  -4,  -2)  },
        {  VALUE(   0,   0), VALUE(   0,   0), VALUE(  10,  -1), VALUE(   6,  -6), VALUE(  -4,  -6)  },
        {  VALUE(   0,   0), VALUE(   0,   0), VALUE(   8,   2), VALUE(   3,  -4), VALUE( -10,   0)  }
    };
    eval ePawnstormfree[4][5] = {
        {  VALUE(  36, -17), VALUE(  36, -12), VALUE(  11,  -8), VALUE(  -5,   4), VALUE(   1,   1)  },
        {  VALUE(  79, -24), VALUE(  69, -10), VALUE(  36,  -7), VALUE(   2,  -3), VALUE(   3,   0)  },
        {  VALUE(  74, -19), VALUE(  55,  -5), VALUE(  42, -16), VALUE(   0,  -5), VALUE(   6,  -3)  },
        {  VALUE(  82, -28), VALUE(  46,  -6), VALUE(  25, -11), VALUE(   3,  -5), VALUE(  -3,   2)  }
    };
    eval ePawnpushthreatbonus =  VALUE(   0,  -1);
    eval eSafepawnattackbonus =  VALUE(  -5,   1);
    eval eHangingpiecepenalty =  VALUE(  30, -31);
    eval ePassedpawnbonus[4][8] = {
        {  VALUE(   0,   0), VALUE(  -5,   4), VALUE(  -8,  23), VALUE(  11,  -9), VALUE(  10,  -4), VALUE( -10, -24), VALUE(-137,-251), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -24,  28), VALUE(  -8,  32), VALUE(   9,  -6), VALUE(  20, -16), VALUE(   0, -20), VALUE(-118,-260), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -42,  28), VALUE( -31,  48), VALUE( -14,  11), VALUE(   3,   3), VALUE( -26, -11), VALUE(-198,-211), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -27,  27), VALUE( -15,  49), VALUE( -14,  18), VALUE(   2,  -2), VALUE( -21, -12), VALUE(-140,-245), VALUE(   0,   0)  }
    };
    eval eKingsupportspasserbonus[7][8] = {
        {  VALUE(   0,   0), VALUE(   3, 122), VALUE(  22, -17), VALUE(  25,   7), VALUE( -15, -30), VALUE( -18,-137), VALUE( -60,-152), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  11, 121), VALUE(  33, -16), VALUE(  13,  15), VALUE( -45, -12), VALUE( -16,-121), VALUE(  24,-188), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(   5, 122), VALUE(  29, -12), VALUE(   3,  23), VALUE( -10, -27), VALUE(   6,-128), VALUE(  26,-154), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  13, 124), VALUE(  26, -13), VALUE(   5,  21), VALUE( -19, -23), VALUE( -10,-117), VALUE( -31,-146), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(   7, 126), VALUE(  29, -19), VALUE(  -2,  24), VALUE( -15, -23), VALUE( -20,-116), VALUE( -28,-145), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(   8, 123), VALUE(  20, -19), VALUE(   1,  15), VALUE( -34, -16), VALUE( -37,-107), VALUE( -37,-143), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -10, 141), VALUE(  17,  -4), VALUE(  -5,  10), VALUE( -29, -27), VALUE(  11,-138), VALUE( -24,-165), VALUE(   0,   0)  }
    };
    eval eKingdefendspasserpenalty[7][8] = {
        {  VALUE(   0,   0), VALUE(  39,-147), VALUE( -20, -32), VALUE(  -9, -30), VALUE(  25,  16), VALUE(  31, 110), VALUE( -55, 176), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(-106,-132), VALUE( -52, -29), VALUE( -33, -25), VALUE(  -3,  25), VALUE(  37, 108), VALUE( -72, 180), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -26,-141), VALUE( -32, -34), VALUE( -13, -27), VALUE(   5,  23), VALUE(  54, 110), VALUE( -85, 187), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE( -24,-138), VALUE( -22, -31), VALUE(  -6, -30), VALUE(  13,  18), VALUE(  45, 107), VALUE( -95, 197), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  -6,-144), VALUE( -14, -32), VALUE(   4, -32), VALUE(  -1,  26), VALUE(  54, 111), VALUE( -82, 201), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(   6,-157), VALUE(  -4, -31), VALUE(  -1, -26), VALUE(  12,  19), VALUE(  49, 117), VALUE( -80, 199), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  15,-137), VALUE(   4, -38), VALUE(   6, -21), VALUE(  38,  19), VALUE(  83,  86), VALUE( -79, 218), VALUE(   0,   0)  }
    };
    eval ePotentialpassedpawnbonus[2][8] = {
        {  VALUE(   0,   0), VALUE(  28, -15), VALUE(  -3,  14), VALUE(  17,   5), VALUE(  37,  60), VALUE(  75,  81), VALUE(  44, 108), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(  -2,   0), VALUE(   1,   2), VALUE(   6,   6), VALUE(  20,  25), VALUE(  53,   7), VALUE( -14,  10), VALUE(   0,   0)  }
    };
    eval eAttackingpawnbonus[8] = {  VALUE(   0,   0), VALUE( -48,  12), VALUE( -14,   4), VALUE( -14,  -6), VALUE( -14,  -6), VALUE( -15,   1), VALUE(   0,   0), VALUE(   0,   0)  };
    eval eIsolatedpawnpenalty =  VALUE(   3,  -9);
    eval eDoublepawnpenalty =  VALUE(   2, -17);
    eval eConnectedbonus[6][6] = {
        {  VALUE(   0,   0), VALUE(   0, -14), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0)  },
        {  VALUE(   0,   0), VALUE(   3, -20), VALUE(   4, -27), VALUE(   0, -33), VALUE(   1, -44), VALUE(  13, -37)  },
        {  VALUE(   0,   0), VALUE(   6, -14), VALUE(   5, -24), VALUE(   8, -31), VALUE(   4, -39), VALUE(  -9, -24)  },
        {  VALUE(   0,   0), VALUE(  64, -75), VALUE(  10, -19), VALUE( 169,-354), VALUE(  13, -28), VALUE( 263,-557)  },
        {  VALUE(   0,   0), VALUE(  -9,  12), VALUE(  17, -19), VALUE(  16,   2), VALUE(  36, -35), VALUE( -57, 253)  },
        {  VALUE(   0,   0), VALUE(   0,  -9), VALUE(   1,  -6), VALUE(   7, 400), VALUE(-192, 109), VALUE(   0,   0)  }
    };
    eval eBackwardpawnpenalty =  VALUE(  -1,  -9);
    eval eDoublebishopbonus =  VALUE(  -9,  13);
    eval ePawnblocksbishoppenalty =  VALUE(  -1,   3);
    eval eBishopcentercontrolbonus =  VALUE(   0,   4);
    eval eMobilitybonus[4][28] = {
        {  VALUE(  16, -90), VALUE(  38, -26), VALUE(  51,   1), VALUE(  57,  13), VALUE(  64,  27), VALUE(  71,  37), VALUE(  77,  36), VALUE(  84,  36),
           VALUE(  86,  30), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0)  },
        {  VALUE(  -8, -25), VALUE(  23,  -7), VALUE(  45,   2), VALUE(  55,  21), VALUE(  65,  31), VALUE(  74,  35), VALUE(  80,  43), VALUE(  81,  50),
           VALUE(  86,  51), VALUE(  85,  58), VALUE(  86,  52), VALUE(  95,  55), VALUE( 100,  54), VALUE(  89,  60), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0)  },
        {  VALUE( -57,  11), VALUE(  14,  17), VALUE(  31,  57), VALUE(  34,  70), VALUE(  36,  82), VALUE(  38,  90), VALUE(  40,  92), VALUE(  46,  96),
           VALUE(  47, 101), VALUE(  50, 111), VALUE(  54, 109), VALUE(  50, 115), VALUE(  49, 118), VALUE(  50, 116), VALUE(  44, 116), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0)  },
        {  VALUE(-4097,  83), VALUE(  13,-157), VALUE(  -2,  31), VALUE(   3,  75), VALUE(   5,  94), VALUE(   4, 152), VALUE(   6, 161), VALUE(  11, 177),
           VALUE(  10, 192), VALUE(  15, 187), VALUE(  14, 212), VALUE(  17, 213), VALUE(  16, 227), VALUE(  18, 229), VALUE(  17, 242), VALUE(  19, 251),
           VALUE(  20, 253), VALUE(  21, 256), VALUE(  18, 263), VALUE(  13, 270), VALUE(  45, 244), VALUE(  67, 233), VALUE(  72, 241), VALUE(  80, 230),
           VALUE(  69, 264), VALUE( 108, 231), VALUE( 107, 230), VALUE( 114, 198)  }
    };
    eval eRookon7thbonus =  VALUE( -58,  16);
    eval eMinorbehindpawn[6] = {  VALUE(  -2,   4), VALUE(  -4,   1), VALUE(   2,   0), VALUE(  -2,   2), VALUE(  -5, -11), VALUE(-102,  13)  };
    eval eSlideronfreefilebonus[2] = {  VALUE( -10,  14), VALUE( -16,  17)  };
    eval eMaterialvalue[7] = {  VALUE(   0,   0), VALUE( 100, 100), VALUE( 314, 314), VALUE( 314, 314), VALUE( 483, 483), VALUE( 913, 913), VALUE(   0,   0)  };
    eval eKingshieldbonus =  VALUE(  -8,  -2);
    eval eWeakkingringpenalty =  SQVALUE(   1,  70);
    eval eKingattackweight[7] = {  SQVALUE(   1,   0), SQVALUE(   1,   0), SQVALUE(   1,  25), SQVALUE(   1,  11), SQVALUE(   1,  15), SQVALUE(   1,  42), SQVALUE(   1,   0)  };
    eval eSafecheckbonus[6] = {  SQVALUE(   1,   0), SQVALUE(   1,   0), SQVALUE(   1, 282), SQVALUE(   1,  55), SQVALUE(   1, 244), SQVALUE(   1, 210)  };
    eval eKingdangerbyqueen =  SQVALUE(   1,-163);
    eval eKingringattack[6] = {  SQVALUE(   1, 111), SQVALUE(   1,   0), SQVALUE(   1,  31), SQVALUE(   1,   0), SQVALUE(   1,   0), SQVALUE(   1, -15)  };
    eval ePsqt[7][64] = {
        {  VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0),
           VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0), VALUE(   0,   0)  },
        {  VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999),
           VALUE( 132, 107), VALUE( 119,  79), VALUE( 164, 101), VALUE( 134, 115), VALUE( 101, 130), VALUE( 112, 118), VALUE( 162,  85), VALUE( 115, 110),
           VALUE(-110, -69), VALUE(-108, -78), VALUE(-114, -67), VALUE(-129, -63), VALUE(-116, -74), VALUE(-122, -70), VALUE(-101, -80), VALUE(-102, -78),
           VALUE(-105, -81), VALUE( -99, -87), VALUE(-105, -82), VALUE(-105, -84), VALUE(-107, -90), VALUE(-103, -88), VALUE(-103, -87), VALUE(-105, -84),
           VALUE(-105, -76), VALUE( -99, -82), VALUE( -99, -85), VALUE( -98, -83), VALUE( -98, -79), VALUE( -99, -85), VALUE( -95, -89), VALUE( -99, -84),
           VALUE(-103, -71), VALUE( -95, -74), VALUE( -99, -73), VALUE( -96, -66), VALUE( -96, -59), VALUE( -93, -76), VALUE( -90, -69), VALUE( -96, -73),
           VALUE(-102, -77), VALUE( -98, -78), VALUE( -96, -87), VALUE( -96, -72), VALUE( -93, -70), VALUE( -95, -82), VALUE( -87, -87), VALUE( -95, -76),
           VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999), VALUE(-999,-999)  },
        {  VALUE(-259,-125), VALUE(-211,-158), VALUE(-237,-148), VALUE(-315,-133), VALUE(-310,-116), VALUE(-284,-155), VALUE(-263,-130), VALUE(-298,-119),
           VALUE(-196,-165), VALUE(-179,-175), VALUE(-220,-148), VALUE(-230,-156), VALUE(-218,-159), VALUE(-229,-159), VALUE(-210,-168), VALUE(-244,-159),
           VALUE(-201,-164), VALUE(-197,-169), VALUE(-184,-180), VALUE(-188,-177), VALUE(-216,-180), VALUE(-226,-188), VALUE(-212,-173), VALUE(-227,-175),
           VALUE(-179,-178), VALUE(-177,-182), VALUE(-167,-188), VALUE(-182,-180), VALUE(-186,-184), VALUE(-195,-186), VALUE(-201,-184), VALUE(-206,-177),
           VALUE(-172,-170), VALUE(-178,-176), VALUE(-171,-178), VALUE(-173,-180), VALUE(-175,-179), VALUE(-175,-179), VALUE(-186,-179), VALUE(-177,-175),
           VALUE(-169,-174), VALUE(-171,-174), VALUE(-167,-179), VALUE(-176,-176), VALUE(-168,-176), VALUE(-171,-176), VALUE(-180,-171), VALUE(-169,-184),
           VALUE(-175,-168), VALUE(-180,-160), VALUE(-173,-181), VALUE(-172,-170), VALUE(-172,-175), VALUE(-180,-167), VALUE(-179,-177), VALUE(-177,-192),
           VALUE(-184,-145), VALUE(-170,-182), VALUE(-180,-171), VALUE(-169,-183), VALUE(-173,-178), VALUE(-170,-182), VALUE(-165,-180), VALUE(-175,-159)  },
        {  VALUE(-230,-136), VALUE(-225,-140), VALUE(-178,-159), VALUE(-242,-138), VALUE(-267,-121), VALUE(-262,-128), VALUE(-242,-147), VALUE(-226,-145),
           VALUE(-174,-158), VALUE(-188,-161), VALUE(-188,-161), VALUE(-181,-162), VALUE(-199,-165), VALUE(-196,-168), VALUE(-181,-175), VALUE(-220,-164),
           VALUE(-169,-168), VALUE(-180,-175), VALUE(-176,-179), VALUE(-170,-178), VALUE(-177,-181), VALUE(-199,-171), VALUE(-192,-177), VALUE(-178,-177),
           VALUE(-174,-164), VALUE(-157,-181), VALUE(-174,-170), VALUE(-174,-177), VALUE(-174,-175), VALUE(-179,-173), VALUE(-157,-177), VALUE(-163,-175),
           VALUE(-164,-177), VALUE(-163,-176), VALUE(-161,-184), VALUE(-162,-182), VALUE(-173,-171), VALUE(-152,-185), VALUE(-161,-172), VALUE(-167,-182),
           VALUE(-159,-175), VALUE(-160,-187), VALUE(-159,-179), VALUE(-161,-178), VALUE(-155,-182), VALUE(-157,-183), VALUE(-163,-179), VALUE(-165,-165),
           VALUE(-164,-175), VALUE(-157,-178), VALUE(-162,-177), VALUE(-154,-176), VALUE(-154,-179), VALUE(-158,-182), VALUE(-158,-180), VALUE(-160,-187),
           VALUE(-161,-165), VALUE(-166,-191), VALUE(-152,-183), VALUE(-154,-177), VALUE(-162,-173), VALUE(-154,-176), VALUE(-171,-169), VALUE(-163,-159)  },
        {  VALUE(-301,-307), VALUE(-309,-304), VALUE(-310,-307), VALUE(-329,-300), VALUE(-345,-300), VALUE(-344,-294), VALUE(-325,-312), VALUE(-299,-312),
           VALUE(-245,-328), VALUE(-237,-332), VALUE(-247,-330), VALUE(-262,-334), VALUE(-252,-342), VALUE(-286,-325), VALUE(-292,-323), VALUE(-290,-320),
           VALUE(-294,-311), VALUE(-285,-311), VALUE(-266,-324), VALUE(-280,-331), VALUE(-276,-330), VALUE(-314,-316), VALUE(-317,-324), VALUE(-315,-307),
           VALUE(-282,-314), VALUE(-262,-328), VALUE(-267,-327), VALUE(-259,-330), VALUE(-268,-327), VALUE(-282,-322), VALUE(-313,-304), VALUE(-305,-304),
           VALUE(-275,-316), VALUE(-265,-325), VALUE(-275,-315), VALUE(-255,-331), VALUE(-266,-322), VALUE(-280,-324), VALUE(-304,-316), VALUE(-297,-314),
           VALUE(-269,-312), VALUE(-263,-329), VALUE(-259,-333), VALUE(-258,-324), VALUE(-259,-332), VALUE(-275,-327), VALUE(-301,-317), VALUE(-289,-313),
           VALUE(-277,-308), VALUE(-272,-326), VALUE(-256,-329), VALUE(-260,-325), VALUE(-258,-331), VALUE(-276,-324), VALUE(-279,-324), VALUE(-277,-318),
           VALUE(-268,-320), VALUE(-267,-321), VALUE(-260,-326), VALUE(-261,-324), VALUE(-262,-330), VALUE(-270,-324), VALUE(-281,-319), VALUE(-279,-312)  },
        {  VALUE(-511,-452), VALUE(-573,-406), VALUE(-552,-453), VALUE(-630,-399), VALUE(-568,-479), VALUE(-607,-458), VALUE(-551,-462), VALUE(-520,-483),
           VALUE(-432,-491), VALUE(-394,-556), VALUE(-419,-544), VALUE(-461,-522), VALUE(-518,-492), VALUE(-484,-561), VALUE(-443,-557), VALUE(-466,-515),
           VALUE(-399,-511), VALUE(-407,-512), VALUE(-415,-532), VALUE(-418,-555), VALUE(-415,-574), VALUE(-525,-520), VALUE(-478,-574), VALUE(-490,-524),
           VALUE(-421,-478), VALUE(-378,-546), VALUE(-398,-537), VALUE(-397,-578), VALUE(-406,-583), VALUE(-410,-574), VALUE(-401,-582), VALUE(-422,-555),
           VALUE(-369,-561), VALUE(-386,-540), VALUE(-370,-580), VALUE(-370,-595), VALUE(-376,-590), VALUE(-376,-585), VALUE(-384,-580), VALUE(-402,-549),
           VALUE(-375,-547), VALUE(-368,-568), VALUE(-371,-576), VALUE(-358,-600), VALUE(-362,-580), VALUE(-369,-589), VALUE(-384,-563), VALUE(-395,-549),
           VALUE(-383,-530), VALUE(-375,-557), VALUE(-373,-568), VALUE(-363,-573), VALUE(-365,-574), VALUE(-382,-551), VALUE(-390,-552), VALUE(-400,-499),
           VALUE(-356,-576), VALUE(-376,-549), VALUE(-372,-546), VALUE(-357,-602), VALUE(-369,-562), VALUE(-361,-562), VALUE(-364,-571), VALUE(-389,-510)  },
        {  VALUE( 512,-126), VALUE(  14,  -1), VALUE(   7, -14), VALUE( -35,   1), VALUE( 174,-108), VALUE(  47, -46), VALUE( 256, -87), VALUE( -35,  19),
           VALUE(  64, -27), VALUE( -37,   7), VALUE( 115, -28), VALUE(   2,   1), VALUE(  44,   6), VALUE(   7,   4), VALUE(  35, -21), VALUE(  94, -27),
           VALUE( 107, -31), VALUE(  29,   0), VALUE(  55,  -6), VALUE(  31,   0), VALUE(  23,  -1), VALUE(   4,   8), VALUE(  -6,   0), VALUE(  30,  -8),
           VALUE(  42,  -2), VALUE(  -5,  12), VALUE(  23,  17), VALUE(  75,   6), VALUE(  63,   8), VALUE(  70,   4), VALUE(  18,   8), VALUE(  90, -13),
           VALUE(  93, -29), VALUE(  64,  -6), VALUE(  71,   1), VALUE(  52,   8), VALUE(  62,  10), VALUE(  67,   3), VALUE(  48,  -7), VALUE(  58, -12),
           VALUE(  57, -17), VALUE(  49, -13), VALUE(  50,   0), VALUE(  60,   3), VALUE(  66,   2), VALUE(  51,  -3), VALUE(  44,  -7), VALUE(  39, -11),
           VALUE( -15,  15), VALUE(  -7,  13), VALUE(   8,   5), VALUE(  -2,  17), VALUE(  13,   6), VALUE(  -2,  14), VALUE(  -9,   9), VALUE( -13,   8),
           VALUE(  27,   5), VALUE(  -8,  22), VALUE(  -9,  20), VALUE(  23,  15), VALUE(  -8,  21), VALUE(  11,   9), VALUE( -17,  26), VALUE(  -8,  22)  }
    };
};



#define NUMOFEVALPARAMS (sizeof(evalparamset) / sizeof(eval))

struct evalparam {
    uint16_t index;
    int16_t g[2];
};

struct positiontuneset {
    uint8_t ph;
    uint8_t sc;
    uint16_t num;
    int16_t R;
    //int8_t padding[3];
};

struct tuneparamselection {
    eval *ev[NUMOFEVALPARAMS];
    string name[NUMOFEVALPARAMS];
    bool tune[NUMOFEVALPARAMS];
    int index1[NUMOFEVALPARAMS];
    int bound1[NUMOFEVALPARAMS];
    int index2[NUMOFEVALPARAMS];
    int bound2[NUMOFEVALPARAMS];
    U64 used[NUMOFEVALPARAMS];

    int count;
};

struct tuner {
    thread thr;
    int index;
    int paramindex;
    eval ev[NUMOFEVALPARAMS];
    int paramcount;
    double starterror;
    double error;
    bool busy = false;
};

struct tunerpool {
    int lowRunning;
    int highRunning;
    int lastImproved;
    tuner *tn;
};


void registeralltuners(chessposition *pos);

#endif

#define SCALE_NORMAL 128
#define SCALE_DRAW 0
#define SCALE_ONEPAWN 48
#define SCALE_HARDTOWIN 10
#define SCALE_OCB 32

enum EvalType { NOTRACE, TRACE};

//
// utils stuff
//
U64 calc_key_from_pcs(int *pcs, int mirror);
void getPcsFromStr(const char* str, int *pcs);
void getFenAndBmFromEpd(string input, string *fen, string *bm, string *am);
vector<string> SplitString(const char* s);
unsigned char AlgebraicToIndex(string s);
string IndexToAlgebraic(int i);
string AlgebraicFromShort(string s, chessposition *pos);
void BitboardDraw(U64 b);
U64 getTime();
#ifdef STACKDEBUG
void GetStackWalk(chessposition *pos, const char* message, const char* _File, int Line, int num, ...);
#endif
#ifdef EVALTUNE
typedef void(*initevalfunc)(void);
bool PGNtoFEN(string pgnfilename, bool quietonly, int ppg);
void TexelTune(string fenfilename, bool noqs, bool bOptimizeK, bool bPreload);

extern int tuningratio;

#endif


//
// transposition stuff
//
typedef unsigned long long u8;
typedef struct ranctx { u8 a; u8 b; u8 c; u8 d; } ranctx;

#define rot(x,k) (((x)<<(k))|((x)>>(64-(k))))

class zobrist
{
public:
    ranctx rnd;
    unsigned long long boardtable[64 * 16];
    unsigned long long cstl[32];
    unsigned long long ept[64];
    unsigned long long s2m;
    zobrist();
    unsigned long long getRnd();
    u8 getHash(chessposition *pos);
    u8 getPawnHash(chessposition *pos);
    u8 getMaterialHash(chessposition *pos);
};

#define TTBUCKETNUM 3


struct transpositionentry {
    uint32_t hashupper;
    uint16_t movecode;
    int16_t value;
    int16_t staticeval;
    uint8_t depth;
    uint8_t boundAndAge;
};

struct transpositioncluster {
    transpositionentry entry[TTBUCKETNUM];
    //char padding[2];
};


#define FIXMATESCOREPROBE(v,p) (MATEFORME(v) ? (v) - p : (MATEFOROPPONENT(v) ? (v) + p : v))
#define FIXMATESCOREADD(v,p) (MATEFORME(v) ? (v) + p : (MATEFOROPPONENT(v) ? (v) - p : v))

class transposition
{
public:
    transpositioncluster *table;
    U64 size;
    U64 sizemask;
    int numOfSearchShiftTwo;
    ~transposition();
    int setSize(int sizeMb);    // returns the number of Mb not used by allignment
    void clean();
    void addHash(U64 hash, int val, int16_t staticeval, int bound, int depth, uint16_t movecode);
    void printHashentry(U64 hash);
    bool probeHash(U64 hash, int *val, int *staticeval, uint16_t *movecode, int depth, int alpha, int beta, int ply);
    uint16_t getMoveCode(U64 hash);
    unsigned int getUsedinPermill();
    void nextSearch() { numOfSearchShiftTwo = (numOfSearchShiftTwo + 4) & 0xfc; }
};


typedef struct pawnhashentry {
    uint32_t hashupper;
    int32_t value;
    U64 passedpawnbb[2];
    U64 isolatedpawnbb[2];
    U64 backwardpawnbb[2];
    U64 attacked[2];
    U64 attackedBy2[2];
    bool bothFlanks;
    unsigned char semiopen[2];
} S_PAWNHASHENTRY;


class Pawnhash
{
public:
    S_PAWNHASHENTRY *table;
    U64 size;
    U64 sizemask;
    Pawnhash(int sizeMb);
    ~Pawnhash();
    bool probeHash(U64 hash, pawnhashentry **entry);
};


#define MATERIALHASHSIZE 0x10000
#define MATERIALHASHMASK (MATERIALHASHSIZE - 1)


struct Materialhashentry {
    U64 hash;
    int scale[2];
    bool onlyPawns;
    int numOfPawns;
};


class Materialhash
{
public:
    Materialhashentry table[MATERIALHASHSIZE];
    bool probeHash(U64 hash, Materialhashentry **entry);
};


extern zobrist zb;
extern transposition tp;
extern Materialhash mh;


//
// board stuff
//

#define STARTFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define BOARDSIZE 64
#define RANKMASK 0x38

#define BUFSIZE 4096

#define PieceType unsigned int
#define BLANKTYPE 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

#define PieceCode unsigned int
#define BLANK 0
#define WPAWN 2
#define BPAWN 3
#define WKNIGHT 4
#define BKNIGHT 5
#define WBISHOP 6
#define BBISHOP 7
#define WROOK 8
#define BROOK 9
#define WQUEEN 10
#define BQUEEN 11
#define WKING 12
#define BKING 13

// My son wants this in binary :-) but -pendantic claims that it's not C11 standard :-(
#define S2MMASK     0x01
#define WQCMASK     0x02
#define WKCMASK     0x04
#define BQCMASK     0x08
#define BKCMASK     0x10
#define CASTLEMASK  0x1e

#define WQC 1
#define WKC 2
#define BQC 3
#define BKC 4
const int QCMASK[2] = { WQCMASK, BQCMASK };
const int KCMASK[2] = { WKCMASK, BKCMASK };
const int castlerookfrom[] = {0, 0, 7, 56, 63 };
const int castlerookto[] = {0, 3, 5, 59, 61 };

const int EPTSIDEMASK[2] = { 0x8, 0x10 };

#define BOUNDMASK 0x03 
#define HASHALPHA 0x01
#define HASHBETA 0x02
#define HASHEXACT 0x00

#define MAXDEPTH 256
#define NOSCORE SHRT_MIN
#define SCOREBLACKWINS (SHRT_MIN + 3 + MAXDEPTH)
#define SCOREWHITEWINS (-SCOREBLACKWINS)
#define SCOREDRAW 0
#define SCORETBWIN 29900

#define MATEFORME(s) ((s) > SCOREWHITEWINS - MAXDEPTH)
#define MATEFOROPPONENT(s) ((s) < SCOREBLACKWINS + MAXDEPTH)
#define MATEDETECTED(s) (MATEFORME(s) || MATEFOROPPONENT(s))

/* Offsets for 64Bit  board*/
const int knightoffset[] = { -6, -10, -15, -17, 6, 10, 15, 17 };
const int diagonaloffset[] = { -7, -9, 7, 9 };
const int orthogonaloffset[] = { -8, -1, 1, 8 };
const int orthogonalanddiagonaloffset[] = { -8, -1, 1, 8, -7, -9, 7, 9 };

const struct { int offset; bool needsblank; } pawnmove[] = { { 0x10, true }, { 0x0f, false }, { 0x11, false } };
extern const int materialvalue[];
extern int psqtable[14][64];
extern evalparamset eps;

// values for move ordering
const int mvv[] = { 0U << 27, 1U << 27, 2U << 27, 2U << 27, 3U << 27, 4U << 27, 5U << 27 };
const int lva[] = { 5 << 24, 4 << 24, 3 << 24, 3 << 24, 2 << 24, 1 << 24, 0 << 24 };
#define PVVAL (7 << 27)
#define KILLERVAL1 (1 << 26)
#define KILLERVAL2 (KILLERVAL1 - 1)
#define NMREFUTEVAL (1 << 25)
#define BADTACTICALFLAG (1 << 31)

#define ISEPCAPTURE 0x40
#define GETFROM(x) (((x) & 0x0fc0) >> 6)
#define GETTO(x) ((x) & 0x003f)
#define GETEPT(x) (((x) & 0x03f00000) >> 20)
#define GETEPCAPTURE(x) (((x) >> 20) & ISEPCAPTURE)

#define GETPROMOTION(x) (((x) & 0xf000) >> 12)
#define GETCAPTURE(x) (((x) & 0xf0000) >> 16)
#define ISTACTICAL(x) ((x) & 0xff000)
#define ISPROMOTION(x) ((x) & 0xf000)
#define ISCAPTURE(x) ((x) & 0xf0000)
#define GETPIECE(x) (((x) & 0xf0000000) >> 28)
#define GETTACTICALVALUE(x) (materialvalue[GETCAPTURE(x) >> 1] + (ISPROMOTION(x) ? materialvalue[GETPROMOTION(x) >> 1] - materialvalue[PAWN] : 0))
#define ISCASTLE(c) (((c) & 0xe0000249) == 0xc0000000)
#define GIVECHECKFLAG 0x08000000
#define GIVESCHECK(x) ((x) & GIVECHECKFLAG)

#define PAWNATTACK(s, p) ((s) ? (((p) & ~FILEHBB) >> 7) | (((p) & ~FILEABB) >> 9) : (((p) & ~FILEABB) << 7) | (((p) & ~FILEHBB) << 9))
#define PAWNPUSH(s, p) ((s) ? ((p) >> 8) : ((p) << 8))
#define PAWNPUSHINDEX(s, i) ((s) ? (i) - 8 : (i) + 8)

// passedPawnMask[18][WHITE]:
// 01110000
// 01110000
// 01110000
// 01110000
// 01110000
// 00o00000
// 00000000
// 00000000
extern U64 passedPawnMask[64][2];

// filebarrierMask[18][WHITE]:
// 00100000
// 00100000
// 00100000
// 00100000
// 00100000
// 00o00000
// 00000000
// 00000000
extern U64 filebarrierMask[64][2];

// neighbourfilesMask[18]:
// 01010000
// 01010000
// 01010000
// 01010000
// 01010000
// 01o10000
// 01010000
// 01010000
extern U64 neighbourfilesMask[64];

// phalanxMask[18]:
// 00000000
// 00000000
// 00000000
// 00000000
// 00000000
// 0xox0000
// 00000000
// 000000o0
extern U64 phalanxMask[64];

// kingshieldMask[6][WHITE]:
// 00000000
// 00000000
// 00000000
// 00000000
// 00000000
// 00000xxx
// 00000xxx
// 000000o0
extern U64 kingshieldMask[64][2];

// kingdangerMask[14][WHITE]:
// 00000000
// 00000000
// 00000000
// 00000000
// 00000xxx
// 00000xxx
// 00000xox
// 00000xxx
extern U64 kingdangerMask[64][2];

// fileMask[18]:
// 00100000
// 00100000
// 00100000
// 00100000
// 00100000
// 00x00000
// 00100000
// 00100000
extern U64 fileMask[64];

// rankMask[18]:
// 00000000
// 00000000
// 00000000
// 00000000
// 00000000
// 11x11111
// 00000000
// 00000000
extern U64 rankMask[64];

// betweenMask[18][45]:
// 00000000
// 00000000
// 00000x00
// 00001000
// 00010000
// 00x00000
// 00000000
// 00000000
extern U64 betweenMask[64][64];

extern int squareDistance[64][64];

struct chessmovestack
{
    int state;
    int ept;
    int kingpos[2];
    unsigned long long hash;
    unsigned long long pawnhash;
    unsigned long long materialhash;
    int halfmovescounter;
    int fullmovescounter;
    U64 isCheckbb;
    int lastnullmove;
    uint32_t movecode;
    U64 kingPinned[2];
};

#define MAXMOVELISTLENGTH 256	// for lists of possible pseudo-legal moves
#define MAXMOVESEQUENCELENGTH 512	// for move sequences in a game


class chessmove
{
public:
    // ppppyxeeeeeeccccrrrrfffffftttttt
    // t(6): index of 'to'-square
    // f(6): index of 'from'-square
    // r(4): piececode of promote
    // c(4): piececode of capture
    // e(4): index of ep capture target
    // x(1): flags an ep capture move
    // y(1): flags a move givin check (not every move that gives check is flagged!); not implemented yet
    // p(4): piececode of the moving piece

    uint32_t code;
    int value;

    chessmove();
    chessmove(int from, int to, PieceCode piece);
    chessmove(int from, int to, PieceCode capture, PieceCode piece);
    chessmove(int from, int to, PieceCode promote, PieceCode capture, PieceCode piece);
    chessmove(int from, int to, PieceCode promote, PieceCode capture, int ept, PieceCode piece);

    bool operator<(const chessmove cm) const { return (value < cm.value); }
    bool operator>(const chessmove cm) const { return (value > cm.value); }
    string toString();
    void print();
};

#define MAXMULTIPV 64
#define MAXTHREADS 256
#define DEFAULTHASH 16
#define CMPLIES 2


// FIXME: This is ugly! Almost the same classes with doubled code.
class chessmovesequencelist
{
public:
	int length;
	chessmove move[MAXMOVESEQUENCELENGTH];
	chessmovesequencelist();
	string toString();
	void print();
};


class chessmovelist
{
public:
    int length;
    chessmove move[MAXMOVELISTLENGTH];
	chessmovelist();
	string toString();
	string toStringWithValue();
	void print();
    chessmove* getNextMove(int minval);
};


enum MoveSelector_State { INITSTATE, HASHMOVESTATE, TACTICALINITSTATE, TACTICALSTATE, KILLERMOVE1STATE, KILLERMOVE2STATE,
    COUNTERMOVESTATE, QUIETINITSTATE, QUIETSTATE, BADTACTICALSTATE, BADTACTICALEND, EVASIONINITSTATE, EVASIONSTATE };

class MoveSelector
{
    chessposition *pos;
public:
    int state;
    chessmovelist* captures;
    chessmovelist* quiets;
    chessmove hashmove;
    chessmove killermove1;
    chessmove killermove2;
    chessmove countermove;
    int legalmovenum;
    bool onlyGoodCaptures;
    int16_t *cmptr[CMPLIES];

public:
    void SetPreferredMoves(chessposition *p);  // for quiescence move selector
    void SetPreferredMoves(chessposition *p, uint16_t hshm, uint32_t kllm1, uint32_t kllm2, uint32_t counter, int excludemove);
    chessmove* next();
};

extern U64 pawn_attacks_to[64][2];
extern U64 knight_attacks[64];
extern U64 king_attacks[64];

struct SMagic {
    U64 mask;  // to mask relevant squares of both lines (no outer squares)
    U64 magic; // magic 64-bit factor
};

extern SMagic mBishopTbl[64];
extern SMagic mRookTbl[64];

#define BISHOPINDEXBITS 9
#define ROOKINDEXBITS 12
#define MAGICBISHOPINDEX(m,x) (int)((((m) & mBishopTbl[x].mask) * mBishopTbl[x].magic) >> (64 - BISHOPINDEXBITS))
#define MAGICROOKINDEX(m,x) (int)((((m) & mRookTbl[x].mask) * mRookTbl[x].magic) >> (64 - ROOKINDEXBITS))
#define MAGICBISHOPATTACKS(m,x) (mBishopAttacks[x][MAGICBISHOPINDEX(m,x)])
#define MAGICROOKATTACKS(m,x) (mRookAttacks[x][MAGICROOKINDEX(m,x)])

extern U64 mBishopAttacks[64][1 << BISHOPINDEXBITS];
extern U64 mRookAttacks[64][1 << ROOKINDEXBITS];

enum MoveType { QUIET = 1, CAPTURE = 2, PROMOTE = 4, TACTICAL = 6, ALL = 7, EVASION = 8, QUIETWITHCHECK = 9 };
enum RootsearchType { SinglePVSearch, MultiPVSearch, PonderSearch };

template <MoveType Mt> int CreateMovelist(chessposition *pos, chessmove* m);
template <MoveType Mt> void evaluateMoves(chessmovelist *ml, chessposition *pos, int16_t **cmptr);

enum AttackType { FREE, OCCUPIED, OCCUPIEDANDKING };

struct positioneval {
    pawnhashentry *phentry;
    Materialhashentry *mhentry;
    int kingattackpiececount[2][7] = { { 0 } };
    int kingringattacks[2] = { 0 };
    int kingattackers[2];
};

#ifdef SDEBUG
enum PvAbortType {
    PVA_UNKNOWN = 0, PVA_FROMTT, PVA_DIFFERENTFROMTT, PVA_RAZORPRUNED, PVA_REVFUTILITYPRUNED, PVA_NMPRUNED, PVA_PROBCUTPRUNED, PVA_LMPRUNED,
    PVA_FUTILITYPRUNED, PVA_SEEPRUNED, PVA_BADHISTORYPRUNED, PVA_MULTICUT, PVA_BESTMOVE, PVA_NOTBESTMOVE, PVA_OMITTED, PVA_BETACUT, PVA_BELOWALPHA }; 
#endif

class chessposition
{
public:
    U64 nodes;
    U64 piece00[14];
    U64 occupied00[2];
    U64 attackedBy2[2];
    U64 attackedBy[2][7];

    // The following block is mapped/copied to the movestack, so its important to keep the order
    int state;
    int ept;
    int kingpos[2];
    unsigned long long hash;
    unsigned long long pawnhash;
    unsigned long long materialhash;
    int halfmovescounter;
    int fullmovescounter;
    U64 isCheckbb;
    int lastnullmove;
    uint32_t movecode;
    U64 kingPinned[2];

    uint8_t mailbox[BOARDSIZE]; // redundand for faster "which piece is on field x"
    chessmovestack movestack[MAXMOVESEQUENCELENGTH];
    uint16_t excludemovestack[MAXMOVESEQUENCELENGTH];
    int16_t staticevalstack[MAXMOVESEQUENCELENGTH];
    int mstop;      // 0 at last non-reversible move before root, rootheight at root position
    int ply;        // 0 at root position
    int rootheight; // fixed stack offset in root position 
    int seldepth;
    int nullmoveside;
    int nullmoveply = 0;
    chessmovelist rootmovelist;
    chessmove bestmove;
    int bestmovescore[MAXMULTIPV];
    int lastbestmovescore;
    chessmove pondermove;
    int LegalMoves[MAXDEPTH];
    uint32_t killer[MAXDEPTH][2];
    uint32_t countermove[14][64];
    int16_t history[2][64][64];
    int16_t counterhistory[14][64][14*64];
    uint32_t bestFailingLow;
    Pawnhash *pwnhsh;
    int threadindex;
    int psqval;
#ifdef SDEBUG
    unsigned long long debughash = 0;
    uint16_t pvdebug[MAXMOVESEQUENCELENGTH];
    int pvdepth[MAXMOVESEQUENCELENGTH];
    int pvmovenum[MAXMOVESEQUENCELENGTH];
    PvAbortType pvaborttype[MAXMOVESEQUENCELENGTH];
    int pvabortval[MAXMOVESEQUENCELENGTH];
#endif
    uint32_t pvtable[MAXDEPTH][MAXDEPTH];
    uint32_t multipvtable[MAXMULTIPV][MAXDEPTH];
    uint32_t lastpv[MAXDEPTH];
    int ph; // to store the phase during different evaluation functions
    int sc; // to stor scaling factor used for evaluation
    int useTb;
    int useRootmoveScore;
    int tbPosition;
    chessmove defaultmove; // fallback if search in time trouble didn't finish a single iteration
    chessmovelist captureslist[MAXDEPTH];
    chessmovelist quietslist[MAXDEPTH];
    chessmovelist singularcaptureslist[MAXDEPTH];   // extra move lists for singular testing
    chessmovelist singularquietslist[MAXDEPTH];
#ifdef EVALTUNE
    bool isQuiet;
    bool noQs;
    tuneparamselection tps;
    positiontuneset pts;
    evalparam ev[NUMOFEVALPARAMS];
    void resetTuner();
    void getPositionTuneSet(positiontuneset *p, evalparam *e);
    void copyPositionTuneSet(positiontuneset *from, evalparam *efrom, positiontuneset *to, evalparam *eto);
    string getGradientString();
#endif
    bool w2m();
    void BitboardSet(int index, PieceCode p);
    void BitboardClear(int index, PieceCode p);
    void BitboardMove(int from, int to, PieceCode p);
    void BitboardPrint(U64 b);
    int getFromFen(const char* sFen);
    string toFen();
    bool applyMove(string s);
    void print(ostream* os = &cout);
    int phase();
    U64 movesTo(PieceCode pc, int from);
    template <PieceType Pt> U64 pieceMovesTo(int from);
    bool isAttacked(int index);
    U64 isAttackedByMySlider(int index, U64 occ, int me);  // special simple version to detect giving check by removing blocker
    U64 attackedByBB(int index, U64 occ);  // returns bitboard of all pieces of both colors attacking index square 
    template <AttackType At> U64 isAttackedBy(int index, int col);    // returns the bitboard of cols pieces attacking the index square; At controls if pawns are moved to block or capture
    bool see(uint32_t move, int threshold);
    int getBestPossibleCapture();
    int getMoves(chessmove *m, MoveType t = ALL);
    void getRootMoves();
    void tbFilterRootMoves();
    void prepareStack();
    string movesOnStack();
    bool playMove(chessmove *cm);
    void unplayMove(chessmove *cm);
    void playNullMove();
    void unplayNullMove();
    void updatePins();
    bool moveGivesCheck(uint32_t c);  // simple and imperfect as it doesn't handle special moves and cases (mainly to avoid pruning of important moves)
    bool moveIsPseudoLegal(uint32_t c);     // test if move is possible in current position
    uint32_t shortMove2FullMove(uint16_t c); // transfer movecode from tt to full move code without checking if pseudoLegal
    void getpsqval();  // only for eval trace
    template <EvalType Et, int Me> int getGeneralEval(positioneval *pe);
    template <EvalType Et, PieceType Pt, int Me> int getPieceEval(positioneval *pe);
    template <EvalType Et, int Me> int getLateEval(positioneval *pe);
    template <EvalType Et, int Me> void getPawnAndKingEval(pawnhashentry *entry);
    template <EvalType Et> int getEval();
    int getScaling(int col, Materialhashentry** mhentry);
    int getComplexity(int eval, pawnhashentry *phentry, Materialhashentry *mhentry);

    template <RootsearchType RT> int rootsearch(int alpha, int beta, int depth);
    int alphabeta(int alpha, int beta, int depth);
    int getQuiescence(int alpha, int beta, int depth);
    void updateHistory(uint32_t code, int16_t **cmptr, int value);
    void getCmptr(int16_t **cmptr);
    void updatePvTable(uint32_t mc, bool recursive);
    void updateMultiPvTable(int pvindex, uint32_t mc);
    string getPv(uint32_t *table);
    int getHistory(uint32_t code, int16_t **cmptr);
    inline void CheckForImmediateStop();

#ifdef SDEBUG
    bool triggerDebug(chessmove* nextmove);
    void pvdebugout();
#endif
    int testRepetiton();
    void mirror();
};

//
// uci stuff
//

enum GuiToken { UNKNOWN, UCI, UCIDEBUG, ISREADY, SETOPTION, REGISTER, UCINEWGAME, POSITION, GO, STOP, PONDERHIT, QUIT, EVAL };

const map<string, GuiToken> GuiCommandMap = {
    { "uci", UCI },
    { "debug", UCIDEBUG },
    { "isready", ISREADY },
    { "setoption", SETOPTION },
    { "register", REGISTER },
    { "ucinewgame", UCINEWGAME },
    { "position", POSITION },
    { "go", GO },
    { "stop", STOP },
    { "ponderhit", PONDERHIT },
    { "quit", QUIT },
    { "eval", EVAL }
};

//
// engine stuff
//

#define ENGINERUN 0
#define ENGINEWANTSTOP 1
#define ENGINESTOPIMMEDIATELY 2
#define ENGINETERMINATEDSEARCH 3

#define NODESPERCHECK 0xfff

class engine
{
public:
    engine();
    ~engine();
    const char* name = ENGINEVER;
    const char* author = "Andreas Matthies";
    bool isWhite;
    U64 tbhits;
    U64 starttime;
    U64 endtime1; // time to stop before starting next iteration
    U64 endtime2; // time to stop immediately
    U64 frequency;
    int wtime, btime, winc, binc, movestogo, mate, movetime, maxdepth;
    U64 maxnodes;
    bool infinite;
    bool debug = false;
    bool moveoutput;
    int sizeOfTp = 0;
    int restSizeOfTp = 0;
    int sizeOfPh;
    int moveOverhead;
    int MultiPV;
    bool ponder;
    string SyzygyPath;
    bool Syzygy50MoveRule = true;
    int SyzygyProbeLimit;
    chessposition rootposition;
    int Threads;
    searchthread *sthread;
    enum { NO, PONDERING, HITPONDER } pondersearch;
    int terminationscore = SHRT_MAX;
    int lastReport;
    int benchdepth;
    string benchmove;
    int stopLevel = ENGINETERMINATEDSEARCH;
#ifdef STACKDEBUG
    string assertfile = "";
#endif
#ifdef TDEBUG
    int t1stop = 0;     // regular stop
    int t2stop = 0;     // immediate stop
    bool bStopCount;
#endif
    GuiToken parse(vector<string>*, string ss);
    void send(const char* format, ...);
    void communicate(string inputstring);
    void setOption(string sName, string sValue);
    void allocThreads();
    void allocPawnhash();
    U64 getTotalNodes();
    bool isPondering() { return (pondersearch == PONDERING); }
    void HitPonder() { pondersearch = HITPONDER; }
    bool testPonderHit() { return (pondersearch == HITPONDER); }
    void resetPonder() { pondersearch = NO; }
    long long perft(int depth, bool dotests);
    void prepareThreads();
};

PieceType GetPieceType(char c);

// enginestate is for communication with external engine process
struct enginestate
{
public:
    int phase;
    string bestmoves;
    string avoidmoves;
    clock_t starttime;
    int firstbesttimesec;
    int score;
    int allscore;
    string enginesbestmove;
    bool doCompare;
    bool doEval;
    bool comparesuccess;
    int comparetime;
    int comparescore;
};

extern engine en;

#ifdef SDEBUG
#define SDEBUGDO(c, s) if (c) {s}
#else
#define SDEBUGDO(c, s)
#endif


//
// search stuff
//
class searchthread
{
public:
    chessposition pos;
    Pawnhash *pwnhsh;
    thread thr;
    int index;
    int depth;
    int numofthreads;
    int lastCompleteDepth;
    searchthread *searchthreads;
    searchthread();
    ~searchthread();
};

void searchStart();
void searchWaitStop(bool forceStop = true);
void searchinit();
void resetEndTime(int constantRootMoves, bool complete = true);


//
// TB stuff
//
extern int TBlargest; // 5 if 5-piece tables, 6 if 6-piece tables were found.

void init_tablebases(char *path);
int probe_wdl(int *success, chessposition *pos);
int probe_dtz(int *success, chessposition *pos);
int root_probe_dtz(chessposition *pos);
int root_probe_wdl(chessposition *pos);


//
// statistics stuff
//
#ifdef STATISTICS
struct statistic {
    U64 qs_n[2];                // total calls to qs split into no check / check
    U64 qs_tt;                  // qs hits tt
    U64 qs_pat;                 // qs returns with pat score
    U64 qs_delta;               // qs return with delta pruning before move loop
    U64 qs_loop_n;              // qs enters moves loop
    U64 qs_move_delta;          // qs moves delta-pruned
    U64 qs_moves;               // moves done in qs
    U64 qs_moves_fh;            // qs moves that cause a fail high

    U64 ab_n;                   // total calls to alphabeta
    U64 ab_pv;                  // number of PV nodes
    U64 ab_tt;                  // alphabeta exit by tt hit
    U64 ab_draw_or_win;         // alphabeta returns draw or mate score
    U64 ab_qs;                  // alphabeta calls qsearch
    U64 ab_tb;                  // alphabeta exits with tb score

    U64 prune_futility;         // nodes pruned by reverse futility
    U64 prune_nm;               // nodes pruned by null move;
    U64 prune_probcut;          // nodes pruned by PobCut
    U64 prune_multicut;         // nodes pruned by Multicut (detected by failed singular test)

    U64 moves_loop_n;           // counts how often the moves loop is entered
    U64 moves_n[2];             // all moves in alphabeta move loop split into quites ans tactical
    U64 moves_pruned_lmp;       // moves pruned by lmp
    U64 moves_pruned_futility;  // moves pruned by futility
    U64 moves_pruned_badsee;    // moves pruned by bad see
    U64 moves_played[2];        // moves that are played split into quites ans tactical
    U64 moves_fail_high;        // moves that cause a fail high;

    U64 red_total;              // total reductions
    U64 red_lmr[2];             // total late-move-reductions for (not) improved moves
    U64 red_pi[2];              // number of quiets moves that are reduced split into (not) / improved moves
    S64 red_history;            // total reduction by history
    S64 red_pv;                 // total reduction by pv nodes
    S64 red_correction;         // total reduction correction by over-/underflow

    U64 extend_singular;        // total extended moves
};

extern struct statistic statistics;

void search_statistics();

// some macros to limit the ifdef STATISTICS inside the code
#define STATISTICSINC(x)        statistics.x++ 
#define STATISTICSADD(x, v)     statistics.x += (v)
#define STATISTICSDO(x)         x

#else
#define STATISTICSINC(x)
#define STATISTICSADD(x, v)
#define STATISTICSDO(x)
#endif

