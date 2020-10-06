# Build options
OBJNAME :=  smooch
CPPC :=     g++
CPPFLAGS := -g -Wall -Werror --std=c++17


# Auto stuff and helper rules
SRC :=      $(wildcard src/*.cpp)
OBJS :=     $(subst src,obj,$(subst cpp,o, $(SRC)))

.PHONY : all
all : $(OBJNAME)

.PHONY : clean
clean :
	rm -rf smooch
	rm -rf obj/

# Main build targets
obj/%.o : src/%.cpp
	mkdir -p obj
	$(CPPC) $(CPPFLAGS) -c -Iinclude/ -o $@ $<

$(OBJNAME) : $(OBJS)
	$(CPPC) -o $(OBJNAME) $(OBJS)
