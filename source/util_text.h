// ========================================================================
size_t atou( const char *text )
{
    size_t n = 0;
    while( *text )
    {
        n *= 10;
        n += (*text++ - '0');
    }

    return n;
}


// NOTE: Does NOT null terminate ouput
// ========================================================================
char* itoaComma( uint64_t n, char *output_ = NULL )
{
    const  size_t SIZE = 32; // 2^63 = 9,223,372,036,854,775,807
    static char   buffer[ SIZE ];
    /* */  char  *p = buffer + SIZE-1;
    *p-- = 0;

    while( n >= 1000 )
    {
        *p-- = '0' + (n % 10); n /= 10;
        *p-- = '0' + (n % 10); n /= 10;
        *p-- = '0' + (n % 10); n /= 10;
        *p-- = ','                    ;
    }

    /*      */ { *p-- = '0' + (n % 10); n /= 10; }
    if( n > 0) { *p-- = '0' + (n % 10); n /= 10; }
    if( n > 0) { *p-- = '0' + (n % 10); n /= 10; }

    if( output_ )
    {
        char   *pEnd = buffer + SIZE - 1;
        size_t  nLen = pEnd - p; 
        memcpy( output_, p+1, nLen );
    }

    return ++p;
}
