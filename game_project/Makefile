CXX = g++

PROJ = snake
OBJS = main.o view.o controller.o model.o
RM 	 = rm

$(PROJ): $(OBJS)
	$(CXX) $(OBJS) -o $(PROJ)

clean:
	$(RM) $(OBJS) $(PROJ)

-include.depence:

depend:
	$(CXX) -MM $(OBJS:.o=.cpp) > .depence