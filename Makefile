.PHONY : all build run clean unbuild
node_modules package-lock.json:
	npm install
all : node_modules package-lock.json
build : all
run : server.js node_modules
	node server.js
clean :
	rm -rf node_modules package-lock.json
unbuild : clean
