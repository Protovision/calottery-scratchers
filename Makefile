.PHONY : all build run clean unbuild
node_modules package-lock.json:
	npm install
all : node_modules package-lock.json
build : all
run : app.js node_modules
	node app.js
clean :
	rm -rf node_modules package-lock.json
unbuild : clean
