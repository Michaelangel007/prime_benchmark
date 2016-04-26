#
# prime numbers to 10 millions, per a quest by Charles Babbage in 1822...
#
import math

for n in range(1,10000000,2):

    nsqrt = math.sqrt(n)
    isPrime = True
    
    if (float(int(nsqrt)) == nsqrt):
        # not a prime, it's sqrt is a whole numbers
        # print("n [", n, "] has a whole number sqrt [", nsqrt, "]")
        isPrime = False
    else:
        # we skip 1 and jump to 3 b/c everything divides by 1...
        for i in range(3,int(nsqrt+1.0),2):
            if (float(int(n / i)) == n / i):
                # print("n [", n, "] divides evenly with [", i, "]")
                isPrime = False

    if (isPrime):
        print("n [", n, "] is prime.")
