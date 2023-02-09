CXX = g++

PROJ = snake
OBJS = main.o view.o controller.o model.o textview.o guiview.o
RM 	 = rm

$(PROJ): $(OBJS)
	$(CXX) $(OBJS) -o $(PROJ)

view.o:
	$(CXX) -o view.o -c views/view.cpp

textview.o:
	$(CXX) -o textview.o -c views/textview.cpp

guiview.o:
	$(CXX) -o guiview.o -c views/guiview.cpp

clean:
	$(RM) $(OBJS) $(PROJ)

-include.depence:

depend:
	$(CXX) -MM $(OBJS:.o=.cpp) > .depence