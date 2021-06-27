.PHONY : all build run clean unbuild
node_modules package.lock :
	npm install
all : node_modules package.lock
build : all
run : app.js node_modules
	node app.js
clean :
	rm -rf node_modules package.lock
unbuild : clean
