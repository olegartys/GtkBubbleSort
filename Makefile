CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0`
FILES=bubble_sort.c

EasyExam:
	$(CC) $(FILES) $(CFLAGS) -o BubbleSort -export-dynamic
