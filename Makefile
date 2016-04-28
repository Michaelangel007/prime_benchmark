all: bin \
	bin bin/1a_clamp_on_naive \
	bin/1b_clamp_off_naive    \
	bin/1c_bitvector_naive    \
	bin/1d_bitvector_minimal  \
	bin/2a_clamp_on_isqrt     \
	bin/2b_clamp_off_isqrt    \
	bin/2c_bitvector_isqrt    \
	bin/3_openmp              \
	bin/4_openmp_bytevector   \
	bin/5_openmp_bytevector   \
	bin/count_primes

# OpenMP needs gcc
CC=/usr/local/bin/g++
INC=$(wildcard source/*.h)
CFLAGS=-Isource/ -O2 -std=c++11

bin:
	mkdir -p $@

bin/1a_clamp_on_naive: source/1a_clamp_on_naive.cpp $(INC)
	g++ $(CFLAGS) $< -o $@

bin/1b_clamp_off_naive: source/1b_clamp_off_naive.cpp $(INC)
	g++ $(CFLAGS) $< -o $@

bin/1c_bitvector_naive: source/1c_bitvector_naive.cpp $(INC)
	g++ $(CFLAGS) $< -o $@

bin/1d_bitvector_minimal: source/1d_bitvector_minimal.cpp $(INC)
	g++ $(CFLAGS) $< -o $@

bin/2a_clamp_on_isqrt: source/2a_clamp_on_isqrt.cpp $(INC)
	g++ $(CFLAGS) $< -o $@

bin/2b_clamp_off_isqrt: source/2b_clamp_off_isqrt.cpp $(INC)
	g++ $(CFLAGS) $< -o $@

bin/2c_bitvector_isqrt: source/2c_bitvector_isqrt.cpp $(INC)
	g++ $(CFLAGS) $< -o $@

bin/3_openmp: source/3_openmp.cpp $(INC)
	$(CC) $(CFLAGS) -fopenmp $< -o $@

bin/4_openmp_bytevector: source/4_openmp_bytevector.cpp $(INC)
	$(CC) $(CFLAGS) -fopenmp $< -o $@

bin/5_openmp_bytevector: source/5_openmp_bytevector.cpp $(INC)
	$(CC) $(CFLAGS) -fopenmp $< -o $@

bin/count_primes: source/count_primes_segmented_sieve.cpp $(INC)
	g++ $(CFLAGS)  $< -o $@

clean:
	rm bin/*

