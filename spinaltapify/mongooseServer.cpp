
#include "common.hpp"

#include <stdio.h>

#include "string.h"
#include "mongoose.h"








static const char requestPage[] = "A";
extern int samplesPlayed;
extern sp_track * currentPlayingtrack;
static const char *options[] = {
  "document_root", "html",
  "listening_ports", "8081",
  "num_threads", "5",
  NULL
};
static char testLinkBuffer[256] = "None";

struct mg_context *ctx;

static void *event_handler(enum mg_event event,
                           struct mg_connection *conn,
                           const struct mg_request_info *request_info) {
	static bool clearedTracks = false;
  	void *processed = (void *) "yes";
	printf("Request url %s\n", request_info->uri);
	
	if (!strcmp(request_info->uri,"/spty_prevTrack")){
	spty_prevTrack(2);
		mg_printf(conn, "Success\n");
	}
	else if (!strcmp(request_info->uri,"/spty_chooseTrack")){
	spty_chooseTrack(1);
		mg_printf(conn, "Success\n");
	}
	else if (!strcmp(request_info->uri,"/spty_skipTrack")){
	spty_skipTrack();
		mg_printf(conn, "Success\n");
	}
	else if (!strcmp(request_info->uri,"/spty_stop")){
	spty_stop();
		mg_printf(conn, "Success\n");
	}
	else if (!strcmp(request_info->uri,"/spty_play")){
	spty_play();
		mg_printf(conn, "Success\n");
	}
	else if (!strcmp(request_info->uri,"/spty_pause")){
	spty_pause();
		mg_printf(conn, "Success\n");
	}
	else if (!strcmp(request_info->uri,"/spty_unPause")){
	spty_unPause();
		mg_printf(conn, "Success\n");
	}
	else if (!strncmp(request_info->uri,"/spty_seek", 10)){
		printf("Seeking to %u\n", atoi(request_info->uri+10));
		spty_seek(atoi(request_info->uri+10));
		mg_printf(conn, "Success\n");
	}
	else if (strstr(request_info->uri,"/spty_addTrack")){
		unsigned int timeout = 5;
		char spotifyURL[256] = {0};
		char * startOfUrl = 0;
		if (strlen(request_info->uri) > 256) {
			mg_printf(conn, "Invalid request string\n");
			goto finished;
		}
		startOfUrl = request_info->uri + 14;
		printf("request_info %s\n", request_info->uri);	
		unsigned int currTrackLength = 0;
		strcpy(spotifyURL, startOfUrl);
		printf("Request url is %s\n", spotifyURL);
        	sp_link * spl = sp_link_create_from_string(spotifyURL);
        	sp_track * spt = sp_link_as_track(spl);
		mg_printf(conn, "Success%u\n", spty_addTrack(spt));
		clearedTracks = false;
	}
	else if (!strcmp(request_info->uri,"/spty_removeTrack")){
	//spty_removeTrack(unsigned int position);
	}
	else if (!strcmp(request_info->uri,"/spty_clearTracks")){
		if (!clearedTracks) spty_clearTracks();
		clearedTracks = true;
		mg_printf(conn, "Success\n");
	}
	else if (!strcmp(request_info->uri,"/spty_setPlayList")){
	//spty_setPlayList(unsigned int size, sp_track * tracks);
	} else if (!strcmp(request_info->uri,"/spty_getCurrentAndPos")){
		if (currentPlayingtrack){
			sp_link * splink = 0;
			splink = sp_link_create_from_track(currentPlayingtrack, 0);
			printf("Made it %u", __LINE__);
			if (splink){
			printf("Made it %u", __LINE__);
				if (sp_link_as_string(splink, testLinkBuffer, sizeof(testLinkBuffer))){
			printf("Made it %u", __LINE__);
					mg_printf(conn, "%d%s", samplesPlayed, testLinkBuffer);
					currentPlayingtrack  = 0;
					goto finished;
				}
			}
		}
		mg_printf(conn, "%d%s", samplesPlayed/44100, testLinkBuffer);
	}else{
	//	mg_printf(conn, "Not implemented yet");
		return 0;
	}
	finished:
  	return processed;
}



int startMongoose()
{
	printf("Hello from the spinaltapify server!\n");
	ctx = mg_start(&event_handler, NULL, options);
	assert(ctx != NULL);
	printf("Run server till we get some input, on port %s\n",  mg_get_option(ctx, "listening_ports"));
	return 0;
}
int stopMongoose()
{
	printf("Shutting down Mongoose server\n");
	mg_stop(ctx);
	return 0;
}
