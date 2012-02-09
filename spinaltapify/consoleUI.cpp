#include "common.hpp"
#include <sstream>
#include <string>

boost::mutex mInputPoll;
unsigned int spty_addTrack(sp_track * track);

static void print_album(sp_album *album)
{
    printf("  YYYY Album \"%s\" (%d)\n",
           sp_album_name(album),
           sp_album_year(album));
}

static void print_artist(sp_artist *artist)
{
    printf("  Artist \"%s\"\n", sp_artist_name(artist));
}

static void print_track(sp_track * track){
	printf (" Track %s by %s \n", sp_track_name(track), sp_artist_name(sp_track_artist(track,0)));
}

static void print_search(sp_search *search)
{
    int i;
    std::string response;

    printf("Query          : %s\n", sp_search_query(search));
    printf("Did you mean   : %s\n", sp_search_did_you_mean(search));
    printf("Tracks in total: %d\n", sp_search_total_tracks(search));
    puts("");

    for (i = 0; i < sp_search_num_tracks(search); ++i){
		printf("Result %d ", i);
        print_track(sp_search_track(search, i));
	}
	badinput:
	printf( "Do you want to add this track to the playlist?\n");
	getline(std::cin, response);
	if (response == "n"){
		
	}else{
		int choice = 0;
		std::stringstream myStream(response);
		if (myStream >> choice && choice < sp_search_num_tracks(search)){
		spty_addTrack(sp_search_track(search,choice));
		}else{
			goto badinput;
		}
	}
}


void  __stdcall searchComplete(sp_search * search, void * userdata){
	printf("Search call back\n");
	if (sp_search_error(search) == SP_ERROR_OK){
		print_search(search);
	}else{
		fprintf(stderr, "Failed to search: %s\n",
		sp_error_message(sp_search_error(search)));
	}

    	sp_search_release(search);
	mInputPoll.unlock();
}

void searchForItem(std::string str){
	mInputPoll.lock();
	sp_search_create(sp, str.c_str(), 0, 20, 0, 0, 0, 0, &searchComplete, NULL);	
}

void inputThread(){
	std::string strSearchTerm = "";
	bool bBreakLoop = false;
	while (!bBreakLoop){
		printf("Enter a search term\n");
		getline(std::cin, strSearchTerm);
		if (strSearchTerm == "quit"){
				bBreakLoop = true;
		}else if (strSearchTerm == "next"){
			spty_skipTrack();
		}else if (strSearchTerm == "prev"){
			spty_prevTrack(2);
		}else if (strSearchTerm == "stop"){
			spty_stop();
		}else if (strSearchTerm == "pause"){
			spty_pause();
		}else if (strSearchTerm == "unpause"){
			spty_unPause();
		}else if (strSearchTerm == "play"){
			spty_play();
		}else if (strSearchTerm == "seek"){
			getline(std::cin, strSearchTerm);
			int choice = 0;
			std::stringstream myStream(strSearchTerm);
			if (myStream >> choice){
				spty_seek(choice);
			}
		}else if (strSearchTerm == "help"){
			std::cout << "Valid commands are: quit next help\n";
		}else if (strSearchTerm == "track"){
			unsigned int seekPos = 0;
			getline(std::cin, strSearchTerm);
			int choice = 0;
			std::stringstream myStream(strSearchTerm);
			if (myStream >> choice){
				spty_chooseTrack(choice);
			}
			std::cout << "Chosen track " << choice << std::endl;
		}else if (strSearchTerm == ""){
		}else{
			std::cout << "searching for " << strSearchTerm;
			searchForItem(strSearchTerm);
			mInputPoll.lock();
			mInputPoll.unlock();
		}
		
	}
	return;
}
