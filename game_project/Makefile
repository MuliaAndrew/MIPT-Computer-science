CXX  = g++

PROJ = snake
OBJS = main.o view.o controller.o model.o textview.o guiview.o
RM 	 = rm

$(PROJ): $(OBJS)
	$(CXX) $(OBJS) -o $(PROJ) -lsfml-graphics -lsfml-window -lsfml-system

view.o: views/view.cpp views/view.h
	$(CXX) -o view.o -c views/view.cpp 

textview.o: views/textview.cpp views/textview.h view.o
	$(CXX) -o textview.o -c views/textview.cpp 

guiview.o: views/guiview.cpp views/guiview.h view.o
	$(CXX) -o guiview.o -c views/guiview.cpp -lsfml-graphics -lsfml-window -lsfml-system

controller.o: controllers/controller.cpp controllers/controller.h
	$(CXX) -o controller.o -c controllers/controller.cpp 

model.o: models/model.cpp models/model.h
	$(CXX) -o model.o -c models/model.cpp 

clean:
	$(RM) $(OBJS) $(PROJ)

-include.depence:

depend:
	$(CXX) -MM $(OBJS:.o=.cpp) > .depence