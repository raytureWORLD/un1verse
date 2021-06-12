SOURCE = ${shell (find code -name "*.cpp" -printf '%P\n')}
SOURCE_LIBRARIES = ${shell (find libraries -name "*.cpp" -printf '%P\n')}
OBJECTS = ${addprefix objects/, ${addsuffix .o, ${basename ${SOURCE}}}}
OBJECTS_LIBRARIES = ${addprefix objects/, ${addsuffix .o, ${basename ${SOURCE_LIBRARIES}}}}

FLAGS = -std=c++20 -O3 -ggdb3 -rdynamic -fsanitize=thread
WARNINGS = -Wall -Wextra -Wpedantic
LINK = -lpthread -ldl -lboost_system
INCLUDE = -I code -I libraries

main: ${OBJECTS} ${OBJECTS_LIBRARIES}
	g++ ${FLAGS} ${WARNINGS} -o main ${OBJECTS} ${OBJECTS_LIBRARIES} ${LINK}

${OBJECTS}: objects/%.o: code/%.cpp
	@mkdir -p ${dir $@}
	g++ ${FLAGS} ${WARNINGS} ${INCLUDE} -c -o $@ $<

${OBJECTS_LIBRARIES}: objects/%.o: libraries/%.cpp
	@mkdir -p ${dir $@}
	g++ ${FLAGS} -c -o $@ $<