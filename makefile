.PHONY: build run clean all
build:
	g++ main.cpp -o main -Wall -Wextra -Wpedantic
clean:
	rm -f main.exe
run:
	./main.exe
all:
	@for target in clean build run ; do\
			echo 'Running target: '$${target};\
	    $(MAKE) $$target;\
			echo -e '\n';\
	done