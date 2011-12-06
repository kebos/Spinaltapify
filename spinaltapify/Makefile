all : SPTY
SPTY : spty

OBJBUILD := g++ -ggdb -O0 -c
MONGOOSEBUILD := gcc -ggdb -O0 -c -DNOSSL
LDFLAGS := g++ -ggb -O0 
BUILDDIR := build

spty : audioBuffer.o consoleUI.o corePlaylist.o portAudio.o spinaltapify.o mongoose.o mongooseServer.o
	$(LDFLAGS) audioBuffer.o consoleUI.o corePlaylist.o portAudio.o spinaltapify.o mongoose.o mongooseServer.o -o spty -lboost_thread -lportaudio -lspotify -pthread -DNOSSL

audioBuffer.o : audioBuffer.cpp
	$(OBJBUILD) $(CFLAGS) audioBuffer.cpp  
consoleUI.o : consoleUI.cpp
	$(OBJBUILD) $(CFLAGS) consoleUI.cpp 
corePlaylist.o : corePlaylist.cpp
	$(OBJBUILD) $(CFLAGS) corePlaylist.cpp 
portAudio.o : portAudio.cpp
	$(OBJBUILD) $(CFLAGS) portAudio.cpp 
spinaltapify.o : spinaltapify.cpp
	$(OBJBUILD) $(CFLAGS) spinaltapify.cpp 
mongoose.o : mongoose.c
	$(MONGOOSEBUILD) $(CFLAGS) mongoose.c
mongooseServer.o : mongooseServer.c
	$(OBJBUILD) $(CFLAGS) -DNOSLL  mongooseServer.c
	
clean :
	rm *.o
	rm spty

