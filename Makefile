CC := clang
CFLAGS := -g -Wall -Werror

all: blackjack

clean:
	@echo "Removing build output"
	@rm -f blackjack blackjack.zip

blackjack: blackjack.c
	$(CC) $(CFLAGS) -o blackjack blackjack.c

zip:
	@echo "Generating blackjack.zip file to submit to Gradescope..."
	@zip -q -r blackjack.zip . -x .git/\* .vscode/\* .clang-format .gitignore blackjack
	@echo "Done. Please upload blackjack.zip to Gradescope."

format:
	@echo "Reformatting source code."
	@clang-format -i --style=file $(wildcard *.c) $(wildcard *.h)
	@echo "Done."

.PHONY: all clean zip format
