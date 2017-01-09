DSTDIR=.
HOST=localhost
PORT=1981

all:
	javac -d $(DSTDIR) *.java

clean:
	rm *.class

run: all
	java ClickController $(HOST) $(PORT)
