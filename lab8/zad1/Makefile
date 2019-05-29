CC = gcc
CFLAGS = -Wall
HEADERS = filter.h image.h shared_func.h exit_func.h
SRC = filter.c image.c shared_func.c exit_func.c
IMG_IN = ./images/lena.ascii.pgm
IMG_OUT = ./output/lena_filtered.ascii.pgm
FLT_PATH = ./filters/

.PHONY: clean

all: main

main: $(SRC) $(HEADERS) main.c
	$(CC) $(CFLAGS) $(SRC) main.c -o main -lm -pthread

filter_generator: $(SRC) $(HEADERS) filter_generator.c
	$(CC) $(CFLAGS) $(SRC) filter_generator.c -o filter_generator -lm -pthread

positive_filter_generator: $(SRC) $(HEADERS) positive_filter_generator.c
	$(CC) $(CFLAGS) $(SRC) positive_filter_generator.c -o positive_filter_generator -lm -pthread

test:
	rm -f ./Times.txt

	make all
	make filter_generator
	./filter_generator

	./main 1 0 $(IMG_IN) $(FLT_PATH)sharpen.txt $(IMG_OUT)
	./main 1 0 $(IMG_IN) $(FLT_PATH)gaussian_blur.txt $(IMG_OUT)
	./main 1 0 $(IMG_IN) $(FLT_PATH)random_filter_10.txt $(IMG_OUT)
	./main 1 0 $(IMG_IN) $(FLT_PATH)random_filter_20.txt $(IMG_OUT)
	./main 1 0 $(IMG_IN) $(FLT_PATH)random_filter_30.txt $(IMG_OUT)
	./main 1 0 $(IMG_IN) $(FLT_PATH)random_filter_40.txt $(IMG_OUT)
	./main 1 0 $(IMG_IN) $(FLT_PATH)random_filter_50.txt $(IMG_OUT)
	./main 1 0 $(IMG_IN) $(FLT_PATH)random_filter_65.txt $(IMG_OUT)

	./main 2 0 $(IMG_IN) $(FLT_PATH)sharpen.txt $(IMG_OUT)
	./main 2 0 $(IMG_IN) $(FLT_PATH)gaussian_blur.txt $(IMG_OUT)
	./main 2 0 $(IMG_IN) $(FLT_PATH)random_filter_10.txt $(IMG_OUT)
	./main 2 0 $(IMG_IN) $(FLT_PATH)random_filter_20.txt $(IMG_OUT)
	./main 2 0 $(IMG_IN) $(FLT_PATH)random_filter_30.txt $(IMG_OUT)
	./main 2 0 $(IMG_IN) $(FLT_PATH)random_filter_40.txt $(IMG_OUT)
	./main 2 0 $(IMG_IN) $(FLT_PATH)random_filter_50.txt $(IMG_OUT)
	./main 2 0 $(IMG_IN) $(FLT_PATH)random_filter_65.txt $(IMG_OUT)

	./main 4 0 $(IMG_IN) $(FLT_PATH)sharpen.txt $(IMG_OUT)
	./main 4 0 $(IMG_IN) $(FLT_PATH)gaussian_blur.txt $(IMG_OUT)
	./main 4 0 $(IMG_IN) $(FLT_PATH)random_filter_10.txt $(IMG_OUT)
	./main 4 0 $(IMG_IN) $(FLT_PATH)random_filter_20.txt $(IMG_OUT)
	./main 4 0 $(IMG_IN) $(FLT_PATH)random_filter_30.txt $(IMG_OUT)
	./main 4 0 $(IMG_IN) $(FLT_PATH)random_filter_40.txt $(IMG_OUT)
	./main 4 0 $(IMG_IN) $(FLT_PATH)random_filter_50.txt $(IMG_OUT)
	./main 4 0 $(IMG_IN) $(FLT_PATH)random_filter_65.txt $(IMG_OUT)

	./main 8 0 $(IMG_IN) $(FLT_PATH)sharpen.txt $(IMG_OUT)
	./main 8 0 $(IMG_IN) $(FLT_PATH)gaussian_blur.txt $(IMG_OUT)
	./main 8 0 $(IMG_IN) $(FLT_PATH)random_filter_10.txt $(IMG_OUT)
	./main 8 0 $(IMG_IN) $(FLT_PATH)random_filter_20.txt $(IMG_OUT)
	./main 8 0 $(IMG_IN) $(FLT_PATH)random_filter_30.txt $(IMG_OUT)
	./main 8 0 $(IMG_IN) $(FLT_PATH)random_filter_40.txt $(IMG_OUT)
	./main 8 0 $(IMG_IN) $(FLT_PATH)random_filter_50.txt $(IMG_OUT)
	./main 8 0 $(IMG_IN) $(FLT_PATH)random_filter_65.txt $(IMG_OUT)

	./main 1 1 $(IMG_IN) $(FLT_PATH)sharpen.txt $(IMG_OUT)
	./main 1 1 $(IMG_IN) $(FLT_PATH)gaussian_blur.txt $(IMG_OUT)
	./main 1 1 $(IMG_IN) $(FLT_PATH)random_filter_10.txt $(IMG_OUT)
	./main 1 1 $(IMG_IN) $(FLT_PATH)random_filter_20.txt $(IMG_OUT)
	./main 1 1 $(IMG_IN) $(FLT_PATH)random_filter_30.txt $(IMG_OUT)
	./main 1 1 $(IMG_IN) $(FLT_PATH)random_filter_40.txt $(IMG_OUT)
	./main 1 1 $(IMG_IN) $(FLT_PATH)random_filter_50.txt $(IMG_OUT)
	./main 1 1 $(IMG_IN) $(FLT_PATH)random_filter_65.txt $(IMG_OUT)

	./main 2 1 $(IMG_IN) $(FLT_PATH)sharpen.txt $(IMG_OUT)
	./main 2 1 $(IMG_IN) $(FLT_PATH)gaussian_blur.txt $(IMG_OUT)
	./main 2 1 $(IMG_IN) $(FLT_PATH)random_filter_10.txt $(IMG_OUT)
	./main 2 1 $(IMG_IN) $(FLT_PATH)random_filter_20.txt $(IMG_OUT)
	./main 2 1 $(IMG_IN) $(FLT_PATH)random_filter_30.txt $(IMG_OUT)
	./main 2 1 $(IMG_IN) $(FLT_PATH)random_filter_40.txt $(IMG_OUT)
	./main 2 1 $(IMG_IN) $(FLT_PATH)random_filter_50.txt $(IMG_OUT)
	./main 2 1 $(IMG_IN) $(FLT_PATH)random_filter_65.txt $(IMG_OUT)

	./main 4 1 $(IMG_IN) $(FLT_PATH)sharpen.txt $(IMG_OUT)
	./main 4 1 $(IMG_IN) $(FLT_PATH)gaussian_blur.txt $(IMG_OUT)
	./main 4 1 $(IMG_IN) $(FLT_PATH)random_filter_10.txt $(IMG_OUT)
	./main 4 1 $(IMG_IN) $(FLT_PATH)random_filter_20.txt $(IMG_OUT)
	./main 4 1 $(IMG_IN) $(FLT_PATH)random_filter_30.txt $(IMG_OUT)
	./main 4 1 $(IMG_IN) $(FLT_PATH)random_filter_40.txt $(IMG_OUT)
	./main 4 1 $(IMG_IN) $(FLT_PATH)random_filter_50.txt $(IMG_OUT)
	./main 4 1 $(IMG_IN) $(FLT_PATH)random_filter_65.txt $(IMG_OUT)

	./main 8 1 $(IMG_IN) $(FLT_PATH)sharpen.txt $(IMG_OUT)
	./main 8 1 $(IMG_IN) $(FLT_PATH)gaussian_blur.txt $(IMG_OUT)
	./main 8 1 $(IMG_IN) $(FLT_PATH)random_filter_10.txt $(IMG_OUT)
	./main 8 1 $(IMG_IN) $(FLT_PATH)random_filter_20.txt $(IMG_OUT)
	./main 8 1 $(IMG_IN) $(FLT_PATH)random_filter_30.txt $(IMG_OUT)
	./main 8 1 $(IMG_IN) $(FLT_PATH)random_filter_40.txt $(IMG_OUT)
	./main 8 1 $(IMG_IN) $(FLT_PATH)random_filter_50.txt $(IMG_OUT)
	./main 8 1 $(IMG_IN) $(FLT_PATH)random_filter_65.txt $(IMG_OUT)

clean:
	rm -f main
	rm -f filter_generator
	rm -f positive_filter_generator
	rm -rf filters/random_*
	rm -rf output/*
