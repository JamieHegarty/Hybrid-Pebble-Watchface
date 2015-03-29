// 2014 - Jamie Hegarty - @Hegrz
#include "hybrid.h"

//TODO: Battery icon update
// Invert colour setting - Fix colour when charging!
// Animate minutes  
// Update time handler


// incoming javascript messages


static char hour_buffer[BUFFER_SIZE];
static char minute_buffer[10];
static char date_buffer[10];
 
void jsmsg(DictionaryIterator *iter, void *context)
{
	Tuple *show_date = dict_find(iter, KEY_SHOWDATE);
	Tuple *date_format = dict_find(iter, KEY_DATEFORMAT);
	Tuple *invert = dict_find(iter, KEY_INVERT);


	if (show_date){
		settings.showdate = show_date->value->cstring[1] == 'n' ? 1 : 0;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Showdate = [%i]", settings.showdate);
	}

	if (invert){
		settings.invert = invert->value->cstring[1] == 'n' ? 1 : 0;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Invert = [%i]", settings.invert);
	}

	if (date_format){
		settings.dateformat = date_format->value->int16 - 48; //-48 turns char value to int value
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Format = [%i]", settings.dateformat);
	}
	
	//Save updated configuration
	persist_write_data(SAVEKEY_SETTINGS, &settings, sizeof(settings));

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Data Received");

	confUpdate();
}

void confUpdate(){

	if((settings.showdate != settings.old_showdate) || (settings.dateformat != settings.old_dateformat)){
		settings.old_showdate = settings.showdate;

		//Move battery bar to the top		
		CleanupBattBar();
		BBOptions options; 
  		options.direction = BATTBAR_DIRECTION_DOWN;
  		options.color = BATTBAR_COLOR_WHITE;
  		options.isWatchApp = true;

		if(settings.showdate == 1 ){
			struct tm *t;
  			time_t temp;
			temp = time(NULL);
  			t = localtime(&temp);
			
  			drawDate(t);

			options.position = BATTBAR_POSITION_TOP; 			
		}else{
			options.position = BATTBAR_POSITION_BOTTOM;
			layer_set_hidden(text_layer_get_layer(date_text_layer), true);
			text_layer_set_text(date_text_layer, "");
		}

		Layer *window_layer = window_get_root_layer(window);
			
 		SetupBattBar(options, window_layer);
  		DrawBattBar();
		drawInvert();
	}
	if(settings.invert != settings.old_invert){
 		drawInvert();
	}
}

void drawInvert(){
	if(settings.invert == 1){
		if(settings.old_invert == 1){
			inverter_layer_destroy(inverter_layer);
		}

		inverter_layer = inverter_layer_create(GRect(0, 0, 144, 168));
 		layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(inverter_layer));	
	}else{
		if(settings.old_invert == 1){
			inverter_layer_destroy(inverter_layer);
		}
	}
	
	settings.old_invert = settings.invert;

}

void drawDate(struct tm *t){
	switch (settings.dateformat) {
		case 1:
  			strftime(date_buffer, sizeof(date_buffer), "%d/%m", t);
		break;
		case 2:
  			strftime(date_buffer, sizeof(date_buffer), "%e %b", t);
  		break;
		case 3:
  			strftime(date_buffer, sizeof(date_buffer), "%m/%d", t);
  		break;
		case 4:
  			strftime(date_buffer, sizeof(date_buffer), "%b %e", t);
  		break;
		default:
  			strftime(date_buffer, sizeof(date_buffer), "%d/%m", t);
  		break;
	}
			
	text_layer_set_text(date_text_layer, date_buffer);
	layer_set_hidden(text_layer_get_layer(date_text_layer), false);
}

void handle_timechanges(struct tm *tick_time, TimeUnits units_changed){
  
  time_to_words(tick_time->tm_hour, hour_buffer, BUFFER_SIZE);
  text_layer_set_text(hour_text_layer, hour_buffer);
  
  strftime(minute_buffer, sizeof(minute_buffer),"%M", tick_time);

  //Hide minutes if they are equal to 0
  if(tick_time->tm_min == 0){
		text_layer_set_text_color(minute_text_layer, GColorBlack);
  }else{
		text_layer_set_text_color(minute_text_layer, GColorWhite);
  }
  
  text_layer_set_text(minute_text_layer, minute_buffer);
 
  if(settings.showdate == 1){
  	drawDate(tick_time);
  }
}

void handle_init(void) {

  //Load font
  ResHandle font_handle = resource_get_handle(RESOURCE_ID_IMAGINE_32);
  ResHandle font_handle_small = resource_get_handle(RESOURCE_ID_IMAGINE_28);
  
  // Create a window
  window = window_create();
  window_set_background_color(window, GColorBlack);
  
  // Push the window
  window_stack_push(window, true);

  //Read saved configuration data
  persist_read_data(SAVEKEY_SETTINGS, &settings, sizeof(settings));

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Values: ");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Invert = [%i]", settings.invert);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Date Format = [%i]", settings.dateformat);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Showdate = [%i]", settings.showdate);

  settings.old_showdate = settings.showdate;
  settings.old_dateformat = settings.dateformat;
  settings.old_invert = settings.invert;


  // ----------------------------------------- Hour Layer ----------------------------------------------//
  hour_text_layer = text_layer_create(GRect(0, 70, 144, 80));
  
  // Set the text, font, and text alignment
  text_layer_set_font(hour_text_layer, fonts_load_custom_font(font_handle));
  text_layer_set_text_alignment(hour_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(hour_text_layer, GColorBlack);
  text_layer_set_text_color(hour_text_layer, GColorWhite);
  
  // Add the text layer to the window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(hour_text_layer));
  // ---------------------------------------------------------------------------------------------------//
  
  
  // ---------------------------------------- Minute Layer ---------------------------------------------//
  minute_text_layer = text_layer_create(GRect(0,47,144,33));
  text_layer_set_background_color(minute_text_layer, GColorBlack);
  text_layer_set_text_color(minute_text_layer, GColorWhite);
  
  // Set the text, font, and text alignment
  text_layer_set_font(minute_text_layer, fonts_load_custom_font(font_handle));
  text_layer_set_text_alignment(minute_text_layer, GTextAlignmentRight);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minute_text_layer));
  // ---------------------------------------------------------------------------------------------------//

  // ---------------------------------------- Date Layer ---------------------------------------------//
  date_text_layer = text_layer_create(GRect(20,130,112,44)); 
  text_layer_set_background_color(date_text_layer, GColorBlack);
  text_layer_set_text_color(date_text_layer, GColorWhite);
  
  // Set the text, font, and text alignment
  text_layer_set_font(date_text_layer, fonts_load_custom_font(font_handle_small));
  text_layer_set_text_alignment(date_text_layer, GTextAlignmentCenter);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_text_layer));
  // ---------------------------------------------------------------------------------------------------//
  
  //Subscribe to time update
  tick_timer_service_subscribe(SECOND_UNIT, handle_timechanges);
  
  // ------------------------------------- Set up Battery Bar ------------------------------------------//
  BBOptions options; 
  
  if(settings.showdate == 1){
	options.position = BATTBAR_POSITION_TOP;
  } else {
	options.position = BATTBAR_POSITION_BOTTOM;
  }

  options.direction = BATTBAR_DIRECTION_DOWN;
  options.color = BATTBAR_COLOR_WHITE;
  options.isWatchApp = true;

  Layer *window_layer = window_get_root_layer(window);
  SetupBattBar(options, window_layer);

  DrawBattBar();
	
  // ---------------------------------------------------------------------------------------------------//

	
  //Get a time structure so that the face doesn't start blank
  struct tm *t;
  time_t temp;
  temp = time(NULL);
  t = localtime(&temp);
 
  //Manually call the tick handler when the window is loading
  handle_timechanges(t, MINUTE_UNIT);

  // -----------------------------------------Invert Layer---------------------------------------------//
  drawInvert();

  app_message_register_inbox_received(jsmsg);
  app_message_open(256,50); // in/out
  
}

void handle_deinit(void) {
	tick_timer_service_unsubscribe();

	// Destroy the text layer
	text_layer_destroy(hour_text_layer);
	text_layer_destroy(minute_text_layer);
	text_layer_destroy(date_text_layer);
	if(settings.invert == 1){
		inverter_layer_destroy(inverter_layer);	
	}
	CleanupBattBar();
	
	// Destroy the window
	window_destroy(window);

}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}

