# History:

This prime benchmarking all started when my friend [imwasEnabled](https://github.com/imwasEnabled) was reading about Charles Babbage describing applications for using machines to print mathematical tables.

In his words:


> One of the speculations that Babbage made for his machines was the creation of
a printed table of prime numbers *up to 10 million*.  After reading that I wondered
what it would take to create my own table of the prime numbers up to 10 million
using a modern computer.  So I started with a quick review of the algorithm to
compute prime using naive approach of testing all of the odd numbers from 3 to the
square root of the target number.  20 minutes later I had a quick implementation in
Python but when I ran it on my i7-3770 it took about 6.5 minutes just to calculate
the primes up to 2.2 million and that seemed much slower than I expected... so I
did what anyone would do -- I ported my Python code to C.  My first C debug
version was able to complete the whole run up to 10 million in 64 seconds, with the
release version taking 21 seconds... while the poor Python version took 3,226 seconds
(~53 minutes).  

> My friend Michaelangel007 then got involved and what ensued was an attempt to find a better version... and then a
better version... then some threading, some heuristics and sieves...  Thank-you Mr.
Babbage, sorry it took almost 200 years, but here is a really fast way to calculate
your primes table.

This still isn't as fast as primesieve (see References) but it shows how trivial it is to add OpenMP to a project.


# Prime Benchmark

10,000,000 primes on i7 @ 2.8 GHz, Windows 7, SP1 (BootCamp)

|Threads|Ver.|Prime array|Algo.|x86 (Bootcamp)|Primes/Sec|x64 (Bootcamp)|Primes/Sec| OSX 10.10 i7 @ 2.6 GHz |Primes/Sec |
|:------|---:|:---------|:----:|:-------------|---------:|:-------------|---------:|:-----------------------|----------:|
|1      |1a|int array   |naive | 00:11:46.078 |  13 K#/s | 00:26:42.294 |   6 K#/s | | |
|1      |1b|int array   |6i+/-1| 00:11:46.081 |  13 K#/s | 00:26:33.725 |   6 K#/s | | |
|1      |1c|bit vector  |naive | 00:50:24.675 |   3 K#/s | 01:10:36.804 |   2 K#/s | | |
|1      |1d|bit vector  |6i+/-1| 00:50:25.634 |   3 K#/s | 01:03:52.921 |   2 K#/s | | |
|1      |2a|int array   |isqrt | 00:00:02.797 |   3 M#/s | 00:00:06.272 |   1 M#/s | 00:00:06.652 |   1 M#/s|
|1      |2b|int aray    |isqrt | 00:00:02.801 |   3 M#/s | 00:00:06.269 |   1 M#/s | 00:00:06.612 |   1 M#/s|
|1      |2c|bit vector  |isqrt | 00:00:01.724 |   5 M#/s | 00:00:02.682 |   3 M#/s | 00:00:01.574 |   5 M#/s|
|1 OMP  | 3|int array   |6i+/-1| 00:00:02.808 |   3 M#/s | 00:00:06.297 |   1 M#/s | 00:00:06.567 |   1 M#/s|
|2      | 3|int array   |6i+/-1| 00:00:01.761 |   5 M#/s | 00:00:03.931 |   2 M#/s | 00:00:04.141 |   2 M#/s|
|3      | 3|int array   |6i+/-1| 00:00:01.251 |   7 M#/s | 00:00:02.760 |   3 M#/s | 00:00:02.948 |   3 M#/s|
|4      | 3|int array   |6i+/-1| 00:00:00.988 |   9 M#/s | 00:00:02.150 |   4 M#/s | 00:00:02.326 |   3 M#/s|
|5      | 3|int array   |6i+/-1| 00:00:00.849 |  10 M#/s | 00:00:01.776 |   5 M#/s | 00:00:02.035 |   4 M#/s|
|6      | 3|int array   |6i+/-1| 00:00:00.743 |  12 M#/s | 00:00:01.549 |   5 M#/s | 00:00:02.186 |   4 M#/s|
|7      | 3|int array   |6i+/-1| 00:00:00.709 |  12 M#/s | 00:00:01.383 |   6 M#/s | 00:00:02.383 |   3 M#/s|
|8      | 3|int array   |6i+/-1| 00:00:00.657 |  13 M#/s | 00:00:01.244 |   7 M#/s | 00:00:02.404 |   3 M#/s|
|1 OMP  | 4|byte array  |sieve | 00:00:00.041 | 219 M#/s | 00:00:00.041 | 219 M#/s | 00:00:00.045 | 198 M#/s|
|2      | 4|byte array  |sieve | 00:00:00.037 | 243 M#/s | 00:00:00.037 | 243 M#/s | 00:00:00.044 | 203 M#/s|
|3      | 4|byte array  |sieve | 00:00:00.037 | 243 M#/s | 00:00:00.035 | 257 M#/s | 00:00:00.043 | 207 M#/s|
|4      | 4|byte array  |sieve | 00:00:00.036 | 250 M#/s | 00:00:00.035 | 257 M#/s | 00:00:00.038 | 233 M#/s|
|5      | 4|byte array  |sieve | 00:00:00.034 | 264 M#/s | 00:00:00.035 | 257 M#/s | 00:00:00.039 | 227 M#/s|
|6      | 4|byte array  |sieve | 00:00:00.034 | 264 M#/s | 00:00:00.034 | 264 M#/s | 00:00:00.038 | 232 M#/s|
|7      | 4|byte array  |sieve | 00:00:00.033 | 272 M#/s | 00:00:00.032 | 281 M#/s | 00:00:00.037 | 237 M#/s|
|8      | 4|byte array  |sieve | 00:00:00.033 | 272 M#/s | 00:00:00.033 | 272 M#/s | 00:00:00.037 | 241 M#/s|
|8 OMP  | 5|byte array  |sieve | 00:00:00.031 |          | 00:00:00.024 | 375 M#/s | 00:00:00.030 | 293 M#/s|

Legend:

    1a (naive) for each odd number 5..n, check if n mod primes 3..p evenly divides
    4 byte vector, sieve, multi-threaded (OpenMP)

NOTE: Will provide OSX @ 2.8 GHz later ...


| Range     |Exponent  | # of Primes   | n'th PRime | Description              |1a H:Min:Sec|1d (H:M:S)  |2a (H:M:S)  |4 (H:M:S)   |5 (H:M:S)   |
|----------:|:---------|--------------:|-----------:|:-------------------------|:-----------|:-----------|:-----------|:-----------|:-----------|
|        100|     10^2 |             25|          97| 25 primes between 1 & 100|            |            |            |            |            |
|        256|&nbsp;2^8 |             54|         251| Largest 8-bit prime      |            |            |            |            |            |
|       1000|     10^3 |            168|         997|                          |            |            |            |            |            |
|      10000|     10^4 |          1,229|       9,973|                          |            |            |            |            |            |
|      65536|&nbsp;2^16|          6,542|      65,521| Largest 16-bit prime     |            |            |            |            |            |
|     100000|     10^5 |          9,592|      99,991|                          |            |            |            |            |            |
|     611953|          |         50,000|     611,953| First 50,5000 primes     |00:00:09.071|00:00:19.983|00:00:00.122|00:00:00.002|            |
|    1000000|     10^6 |         78,498|     999,983|                          |00:00:22.389|00:00:49.865|00:00:00.243|00:00:00.004|            |
|   10000000|     10^7 |        664,579|   9,999,991|                          |00:26:42.294|            |00:00:06.254|00:00:00.036|            |
|   15485863|          |      1,000,000|  15,485,863| First 1,000,000 primes   |01:00:20.988|            |00:00:11.646|00:00:00.059|            |
|  100000000|     10^8 |      5,761,455|  99,999,989|                          |            |            |            |00:00:00.491|            |
| 1000000000|     10^9 |     50,847,534| 999,999,937|                          |            |            |            |00:00:10.580|00:00:09.930|
| 4294967296|&nbsp;2^32|    203,280,221|            |                          |            |            |            |            |00:00:50.808|
|10000000000|     10^10|    455,052,511|            |                          |            |            |            |            |00:02:13.700|
|       1e11|     10^11|  4,118,054,813|            |                          |            |            |            |            |            |
|       1e12|     10^12| 37,607,912,018|            |                          |            |            |            |            |            |
|       1e13|     10^13|346,065,536,839|            |                          |            |            |            |            |            |

Note:

 primgen generates 50,847,534 primes up to 1,000,000,000 (10^9) in just 8 seconds on a Pentium II-350.

# References:

* [Prime Sieve](http://primesieve.org)
* [primegen](http://cr.yp.to/primegen.html)
* [Sieve of Atkin](https://en.wikipedia.org/wiki/Sieve_of_Atkin)
* [Sieve of Atkin Simple Explanation](https://www.quora.com/How-can-sieve-of-Atkin-be-explained-in-simple-terms)

