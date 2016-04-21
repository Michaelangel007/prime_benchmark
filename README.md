# Prime Benchmark

10,000,000 primes on i7 @ 2.8 GHz, Windows 7, SP1 (BootCamp)

|Threads|Version                      | x86       | x64        | Timing                           |
|:------|:----------------------------|:----------|-----------:|:---------------------------------|
|1      |1a array of primes (naive)   |           |    15 mins |                                  |
|1      |1c bit vector      (naive)   |           |   772 secs | (12 min, 2 secs)                 |
|1      |1d bit vector      (6i+/-1)  |           |  3832 secs | 01:03:52.921  Primes/Sec: 2 K#/s |
|1      |2a array of primes (isqrt)   | 6.21 secs | 6.188 secs | 00:00:06.188  Primes/Sec: 1 M#/s |
|1      |2b array of primes (isqrt)   |           | 6.184 secs | 00:00:06.184  Primes/Sec: 1 M#/s |
|1      |2c bit vector      (isqrt)   |           | 2.749 secs | 00:00:02.749  Primes/Sec: 3 M#/s |
|1      |3 OpnemP -j 1                |           | 6.212 secs | 00:00:06.212  Primes/Sec: 1 M#/s |
|2      |3 OpenMP -j 2                |           | 3.901 secs | 00:00:03.901  Primes/Sec: 2 M#/s |
|3      |3 OpenMP -j 3                |           | 2.731 secs | 00:00:02.731  Primes/Sec: 3 M#/s |
|4      |3 OpenMP -j 4                |           | 2.103 secs | 00:00:02.103  Primes/Sec: 4 M#/s |
|5      |3 OpenMP -j 5                |           | 1.788 secs | 00:00:01.788  Primes/Sec: 5 M#/s |
|6      |3 OpenMP -j 6                |           | 1.530 secs | 00:00:01.530  Primes/Sec: 5 M#/s |
|7      |3 OpenMP -j 7                |           | 1.354 secs | 00:00:01.354  Primes/Sec: 6 M#/s |
|8      |3 OpenMP -j 8                | 0.7  secs | 1.217 secs | 00:00:01.217  Primes/Sec: 7 M#/s |


