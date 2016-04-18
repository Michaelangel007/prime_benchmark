// uint64_t
//    #include <stdint.h>
// typedef unsigned size_t uint64_t;

#ifdef _WIN32 // MSC_VER
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <Windows.h> // Windows.h -> WinDef.h defines min() max()

    /*
        typedef uint16_t WORD ;
        typedef uint32_t DWORD;

        typedef struct _FILETIME {
            DWORD dwLowDateTime;
            DWORD dwHighDateTime;
        } FILETIME;

        typedef struct _SYSTEMTIME {
              WORD wYear;
              WORD wMonth;
              WORD wDayOfWeek;
              WORD wDay;
              WORD wHour;
              WORD wMinute;
              WORD wSecond;
              WORD wMilliseconds;
        } SYSTEMTIME, *PSYSTEMTIME;
    */

    // WTF!?!? Exists in winsock2.h
    typedef struct timeval {
        long tv_sec;
        long tv_usec;
    } timeval;

    // *sigh* no gettimeofday on Win32/Win64
    int gettimeofday(struct timeval * tp, struct timezone * tzp)
    {
        // FILETIME Jan 1 1970 00:00:00
        // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
        static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL); 

        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724950(v=vs.85).aspx
        SYSTEMTIME  system_time;
        FILETIME    file_time;
        uint64_t    time;

        GetSystemTime( &system_time );
        SystemTimeToFileTime( &system_time, &file_time );
        time =  ((uint64_t)file_time.dwLowDateTime )      ;
        time += ((uint64_t)file_time.dwHighDateTime) << 32;

        tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
        tp->tv_usec = (long) (system_time.wMilliseconds * 1000); // 1,000 milliseconds / microsecond
        return 0;
    }
#endif // WIN32

struct DataRate
{
    char     prefix ;
    uint64_t samples;
    uint64_t per_sec;
};

struct TimeText
{
    char     day[ 16 ]; // output
    char     hms[ 12 ]; // output

    uint16_t _ms   ; // 0..999
    uint8_t  _secs ; // 0..59
    uint8_t  _mins ; // 0..59
    uint8_t  _hours; // 0..23
    uint32_t _days ; // 0..#

    void Format( double elapsed, bool bShowMilliSeconds = true )
    {
        _ms      = (uint16_t)(elapsed * 1000.0) % 1000;
        size_t s = (size_t)elapsed;
        _secs  = s % 60; s /= 60;
        _mins  = s % 60; s /= 60;
        _hours = s % 24; s /= 24;
        _days  = (uint32_t) s;

        day[0] = 0;
        if( _days > 0 )
            snprintf( day, 15, "%d day%s, ", _days, (_days == 1) ? "" : "s" );

        // TODO: FIXME: bShowMilliSeconds
        sprintf( hms, "%02d:%02d:%02d.%03d", _hours, _mins, _secs,_ms );
    }
};

class Timer
{
    timeval start, end; // Windows: winsock2.h  Unix: sys/time.h 
public:
    double   elapsed; // total seconds

    TimeText data;
    DataRate throughput;

    void Start() {
        gettimeofday( &start, NULL );
    }

    void Stop( bool bShowMilliSeconds = true ) {
        gettimeofday( &end, NULL );
        elapsed = (end.tv_sec - start.tv_sec);
        elapsed += (end.tv_usec - start.tv_usec) / (1000. * 1000.);

        data.Format( elapsed, bShowMilliSeconds );
    }

    // size is number of bytes in a file, or number of iterations that you want to benchmark
    DataRate Throughput( uint64_t size )
    {
        const int MAX_PREFIX = 4;
        DataRate datarate[ MAX_PREFIX ] = {
            {' '}, {'K'}, {'M'}, {'G'} // 1; 1,000; 1,000,000; 1,000,000,000
        };

        int best = 0;
        for( int units = 0; units < MAX_PREFIX; units++ ) {
            datarate[ units ].samples  = size >> (10*units);
            datarate[ units ].per_sec = (uint64_t) (datarate[units].samples / elapsed);
            if (datarate[units].per_sec > 0)
                best = units;
        }

        throughput = datarate[ best ];
        return throughput;
    }
};