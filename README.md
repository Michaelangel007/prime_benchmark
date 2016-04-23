# Prime Benchmark

10,000,000 primes on i7 @ 2.8 GHz, Windows 7, SP1 (BootCamp)

|Threads|Version                      | x86       | x64        | Windows 7 Timing                  | OSX 10.10 i7 @ 2.6 GHz |
|:------|:----------------------------|:----------|-----------:|:----------------------------------|-----------------------:|
|1      |1a array of primes (naive)   |           |  1570 secs | 00:26:10.202  Primes/Sec: 6 K#/s  | |
|1      |1c bit vector      (naive)   |           |  4236 secs | 01:10:36.804  Primes/Sec: 2 K#/s  | |
|1      |1d bit vector      (6i+/-1)  |           |  3832 secs | 01:03:52.921  Primes/Sec: 2 K#/s  | |
|1      |2a array of primes (isqrt)   | 6.21 secs | 6.188 secs | 00:00:06.188  Primes/Sec: 1 M#/s  | 6.652 secs = 00:00:06.652  Primes/Sec: 1 M#/s |
|1      |2b array of primes (isqrt)   |           | 6.184 secs | 00:00:06.184  Primes/Sec: 1 M#/s  | 6.612 secs = 00:00:06.612  Primes/Sec: 1 M#/s |
|1      |2c bit vector      (isqrt)   |           | 2.749 secs | 00:00:02.749  Primes/Sec: 3 M#/s  | 1.574 secs = 00:00:01.574  Primes/Sec: 5 M#/s |
|1      |3 OpnemP -j 1                |           | 6.212 secs | 00:00:06.212  Primes/Sec: 1 M#/s  | 6.568 secs = 00:00:06.567  Primes/Sec: 1 M#/s |
|2      |3 OpenMP -j 2                |           | 3.901 secs | 00:00:03.901  Primes/Sec: 2 M#/s  | 4.141 secs = 00:00:04.141  Primes/Sec: 2 M#/s |
|3      |3 OpenMP -j 3                |           | 2.731 secs | 00:00:02.731  Primes/Sec: 3 M#/s  | 2.948 secs = 00:00:02.948  Primes/Sec: 3 M#/s |
|4      |3 OpenMP -j 4                |           | 2.103 secs | 00:00:02.103  Primes/Sec: 4 M#/s  | 2.326 secs = 00:00:02.326  Primes/Sec: 3 M#/s |
|5      |3 OpenMP -j 5                |           | 1.788 secs | 00:00:01.788  Primes/Sec: 5 M#/s  | 2.036 secs = 00:00:02.035  Primes/Sec: 4 M#/s |
|6      |3 OpenMP -j 6                |           | 1.530 secs | 00:00:01.530  Primes/Sec: 5 M#/s  | 2.187 secs = 00:00:02.186  Primes/Sec: 4 M#/s |
|7      |3 OpenMP -j 7                |           | 1.354 secs | 00:00:01.354  Primes/Sec: 6 M#/s  | 2.384 secs = 00:00:02.383  Primes/Sec: 3 M#/s |
|8      |3 OpenMP -j 8                | 0.7  secs | 1.217 secs | 00:00:01.217  Primes/Sec: 7 M#/s  | 2.404 secs = 00:00:02.404  Primes/Sec: 3 M#/s |
|1      |4 OpenMP -j 1      (sieve)   |           | 0.041 secs | 00:00:00.041  Primes/Sec: 219 M#/s| |
|2      |4 OpenMP -j 2      (sieve)   |           | 0.037 secs | 00:00:00.037  Primes/Sec: 243 M#/s| |
|3      |4 OpenMP -j 3      (sieve)   |           | 0.035 secs = 00:00:00.035  Primes/Sec: 257 M#/s| |
|4      |4 OpenMP -j 4      (sieve)   |           | 0.035 secs = 00:00:00.035  Primes/Sec: 257 M#/s| |
|5      |4 OpenMP -j 5      (sieve)   |           | 0.035 secs = 00:00:00.035  Primes/Sec: 257 M#/s| |
|6      |4 OpenMP -j 6      (sieve)   |           | 0.034 secs = 00:00:00.034  Primes/Sec: 264 M#/s| |
|7      |4 OpenMP -j 7      (sieve)   |           | 0.032 secs = 00:00:00.032  Primes/Sec: 281 M#/s| |
|8      |4 OpenMP -j 8      (sieve)   |           | 0.033 secs = 00:00:00.033  Primes/Sec: 272 M#/s| |

Legend:

    4 byte vector, sieve, multi-threaded

NOTE: Will provide OSX @ 2.8 GHz later ...

