CC := clang
CFLAGS := -g -Wall 

all: blackjack client gameInterface

clean:
	@echo "Removing build output"
	@rm -f blackjack blackjack.zip

blackjack: blackjack.c socket.h message.h message.c blackjack.h
	$(CC) $(CFLAGS) -o blackjack blackjack.c message.c

server: server.c message.h message.c socket.h message.c
	$(CC) $(CFLAGS) -o server server.c message.c -lpthread

client: client.c message.h message.c socket.h message.c blackjack.h
	$(CC) $(CFLAGS) -o client client.c message.c

gameInterface: gameInterface.c 
	$(CC) $(CFLAGS) -o gameInterface gameInterface.c -lncurses

zip:
	@echo "Generating blackjack.zip file to submit to Gradescope..."
	@zip -q -r blackjack.zip . -x .git/\* .vscode/\* .clang-format .gitignore blackjack
	@echo "Done. Please upload blackjack.zip to Gradescope."

format:
	@echo "Reformatting source code."
	@clang-format -i --style=file $(wildcard *.c) $(wildcard *.h)
	@echo "Done."

.PHONY: all clean zip format
