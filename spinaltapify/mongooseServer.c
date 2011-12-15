#include <stdio.h>
#include <assert.h>
#include "mongoose.h"
#include "spinaltapify.hpp"
#include "string.h"



static const char requestPage[] = "A";

static const char *options[] = {
  "document_root", "html",
  "listening_ports", "8081",
  "num_threads", "5",
  NULL
};

struct mg_context *ctx;

static void *event_handler(enum mg_event event,
                           struct mg_connection *conn,
                           const struct mg_request_info *request_info) {
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
	else if (!strcmp(request_info->uri,"/spty_seek")){
	spty_seek(1000);
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
        	spty_addTrack(spt);
		mg_printf(conn, "Success\n");
	}
	else if (!strcmp(request_info->uri,"/spty_removeTrack")){
	//spty_removeTrack(unsigned int position);
	}
	else if (!strcmp(request_info->uri,"/spty_clearTracks")){
		spty_clearTracks();
		mg_printf(conn, "Success\n");
	}
	else if (!strcmp(request_info->uri,"/spty_setPlayList")){
	//spty_setPlayList(unsigned int size, sp_track * tracks);
	} else {
		mg_printf(conn, "Not implemented yet");
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
}
