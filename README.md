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

|Threads|Ver.|Prime array|Algo.|x86 (Bootcamp)|Primes/s|x64 (Bootcamp)|Primes/s| OSX 10.10 i7 @ 2.6 GHz |Primes/s|
|:------|---:|:--|:----:|:-------------|---------:|:-------------|---------:|:-----------------------|----------:|
|1      |1a|int  |naive | 00:11:46.078 |  13 K#/s | 00:26:42.294 |   6 K#/s | 00:12:22.641 |  13 K#/s|
|1      |1b|int  |6i+/-1| 00:11:46.081 |  13 K#/s | 00:26:33.725 |   6 K#/s | 00:12:23.646 |  13 K#/s|
|1      |1c|bit  |naive | 00:50:24.675 |   3 K#/s | 01:10:36.804 |   2 K#/s | 01:07:47.442 |   2 K#/s|
|1      |1d|bit  |6i+/-1| 00:50:25.634 |   3 K#/s | 01:03:52.921 |   2 K#/s | 00:56:41.401 |   2 K#/s|
|1      |2a|int  |isqrt | 00:00:02.797 |   3 M#/s | 00:00:06.272 |   1 M#/s | 00:00:06.652 |   1 M#/s|
|1      |2b|int  |isqrt | 00:00:02.801 |   3 M#/s | 00:00:06.269 |   1 M#/s | 00:00:06.612 |   1 M#/s|
|1      |2c|bit  |isqrt | 00:00:01.724 |   5 M#/s | 00:00:02.682 |   3 M#/s | 00:00:01.574 |   5 M#/s|
|1 OMP  | 3|int  |6i+/-1| 00:00:02.808 |   3 M#/s | 00:00:06.297 |   1 M#/s | 00:00:06.567 |   1 M#/s|
|2      | 3|int  |6i+/-1| 00:00:01.761 |   5 M#/s | 00:00:03.931 |   2 M#/s | 00:00:04.141 |   2 M#/s|
|3      | 3|int  |6i+/-1| 00:00:01.251 |   7 M#/s | 00:00:02.760 |   3 M#/s | 00:00:02.948 |   3 M#/s|
|4      | 3|int  |6i+/-1| 00:00:00.988 |   9 M#/s | 00:00:02.150 |   4 M#/s | 00:00:02.326 |   3 M#/s|
|5      | 3|int  |6i+/-1| 00:00:00.849 |  10 M#/s | 00:00:01.776 |   5 M#/s | 00:00:02.035 |   4 M#/s|
|6      | 3|int  |6i+/-1| 00:00:00.743 |  12 M#/s | 00:00:01.549 |   5 M#/s | 00:00:02.186 |   4 M#/s|
|7      | 3|int  |6i+/-1| 00:00:00.709 |  12 M#/s | 00:00:01.383 |   6 M#/s | 00:00:02.383 |   3 M#/s|
|8      | 3|int  |6i+/-1| 00:00:00.657 |  13 M#/s | 00:00:01.244 |   7 M#/s | 00:00:02.404 |   3 M#/s|
|1 OMP  | 4|byte+|sieve | 00:00:00.041 | 219 M#/s | 00:00:00.041 | 219 M#/s | 00:00:00.045 | 198 M#/s|
|2      | 4|byte+|sieve | 00:00:00.037 | 243 M#/s | 00:00:00.037 | 243 M#/s | 00:00:00.044 | 203 M#/s|
|3      | 4|byte+|sieve | 00:00:00.037 | 243 M#/s | 00:00:00.035 | 257 M#/s | 00:00:00.043 | 207 M#/s|
|4      | 4|byte+|sieve | 00:00:00.036 | 250 M#/s | 00:00:00.035 | 257 M#/s | 00:00:00.038 | 233 M#/s|
|5      | 4|byte+|sieve | 00:00:00.034 | 264 M#/s | 00:00:00.035 | 257 M#/s | 00:00:00.039 | 227 M#/s|
|6      | 4|byte+|sieve | 00:00:00.034 | 264 M#/s | 00:00:00.034 | 264 M#/s | 00:00:00.038 | 232 M#/s|
|7      | 4|byte+|sieve | 00:00:00.033 | 272 M#/s | 00:00:00.032 | 281 M#/s | 00:00:00.037 | 237 M#/s|
|8      | 4|byte+|sieve | 00:00:00.033 | 272 M#/s | 00:00:00.033 | 272 M#/s | 00:00:00.037 | 241 M#/s|
|1 OMP  | 5|byte |sieve | 00:00:00.032 | 281 M#/s | 00:00:00.033 | 272 M#/s |              |         |
|8      | 5|byte |sieve | 00:00:00.025 | 360 M#/s | 00:00:00.024 | 375 M#/s | 00:00:00.030 | 293 M#/s|

Legend:

    int    int array
    bit    bit vector
    byte   byte array
    byte+  byte array + int array

    1a (naive) for each odd number 5..n, check if n mod primes 3..p evenly divides
    4 byte vector, sieve, multi-threaded (OpenMP), single sqrt(), prime array: uint32_t (explicit)
    5 byte vector, sieve, multi-threaded (OpenMP), single sqrt(), prime array: n/a      (implicit)

**Note**: Will provide OSX @ 2.8 GHz later so is, pardon the pun, apple-to-apples comparison.

Pivot Table showing scalability. Exp. is exponent. Time is (Hour:Min:Seconds)

|               Range|Exp. |    # of Primes|                n'th PRime|Description             |1a          |1d          |2a          |2c          |4           |5           |
|-------------------:|:----|--------------:|-------------------------:|:-----------------------|:-----------|:-----------|:-----------|:-----------|:-----------|:-----------|
|                 100|10^2 |             25|                        97|Primes between 1 & 100  |            |            |            |            |            |            |
|                 256| 2^8 |             54|                       251|Largest 8-bit prime     |            |            |            |            |            |            |
|                1000|10^3 |            168|                       997|                        |            |            |            |            |            |            |
|               10000|10^4 |          1,229|                     9,973|                        |            |            |            |            |            |            |
|               65536| 2^16|          6,542|                    65,521|Largest 16-bit prime    |            |            |            |            |            |            |
|              100000|10^5 |          9,592|                    99,991|                        |            |            |            |            |            |            |
|              611953|     |         50,000|                   611,953|First 50,5000 primes    |00:00:09.071|00:00:19.983|00:00:00.122|00:00:00.061|00:00:00.002|00:00:00.002|
|             1000000|10^6 |         78,498|                   999,983|                        |00:00:22.389|00:00:49.865|00:00:00.243|00:00:00.118|00:00:00.004|00:00:00.002|
|            10000000|10^7 |        664,579|                 9,999,991|                        |00:26:42.294|01:01:48.889|00:00:06.254|00:00:02.631|00:00:00.036|00:00:00.023|
|            15485863|     |      1,000,000|                15,485,863|First 1,000,000 primes  |01:00:20.988|02:21:01.471|00:00:11.646|00:00:04.924|00:00:00.059|00:00:00.047|
|           100000000|10^8 |      5,761,455|                99,999,989|                        |1 day, 9 hrs|            |00:02:45.363|00:01:20.898|00:00:00.491|00:00:00.428|
|          1000000000|10^9 |     50,847,534|               999,999,937|                        |            |            |01:16:08.727|00:39:27.486|00:00:10.580|00:00:09.930|
|          2038074743|     |    100,000,000|             2,038,074,743|First 100,000,000 primes|            |            |            |            |            |00:00:21.912|
|          2147483648| 2^31|    105,097,565|             2,147,483,647|Largest 31-bit prime    |            |            |            |            |            |00:00:23.327|
|          4294967296| 2^32|    203,280,221|             4,294,967,291|Largest 32-bit prime    |            |            |            |            |            |00:00:50.808|
|         10000000000|10^10|    455,052,511|             9,999,999,967|                        |            |            |            |            |            |00:02:13.700|
|                1e11|10^11|  4,118,054,813|                         ?|                        |            |            |            |            |            |            |
|                1e12|10^12| 37,607,912,018|                         ?|                        |            |            |            |            |            |            |
|                1e13|10^13|346,065,536,839|                         ?|                        |            |            |            |            |            |            |
| 9223372036854775808| 2^63|              ?| 9,223,372,036,854,775,783|Largest 63-bit prime    |            |            |            |            |            |            |
|18446744073709551616| 2^64|              ?|18,446,744,073,709,551,557|largest 64-bit prime    |            |            |            |            |            |            |

Note:

 primgen generates 50,847,534 primes up to 1,000,000,000 (10^9) in just 8 seconds on a Pentium II-350.

Yes, 1a really took that long for 10^8

```
c:\...\>1a_clamp_on_naive.exe 100000000
Allocating memory..: 100,000,000 * 4 = 400,000,000 bytes
Finding primes: 1 .. 100,000,000
Primes found: 5,761,455'th = 99,999,989
Elapsed: 119489.947 secs = 1 day, 09:11:29.947   Primes/Sec: 836  #/s
```

# References:

* [Prime Sieve](http://primesieve.org)
* [primegen](http://cr.yp.to/primegen.html)
* [Sieve of Atkin](https://en.wikipedia.org/wiki/Sieve_of_Atkin)
* [Sieve of Atkin Simple Explanation](https://www.quora.com/How-can-sieve-of-Atkin-be-explained-in-simple-terms)
* [Fast prime-sieving programs and software](http://mathforum.org/kb/message.jspa?messageID=9807702)
* [Magic Numbers: Integers](https://msdn.microsoft.com/en-us/library/ee621251.aspx)

