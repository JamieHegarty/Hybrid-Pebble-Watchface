// 2014 Thomas Hunsaker @thunsaker && Jamie Hegarty 
// BattBar.c
// BattBar v.1.4

#include <pebble.h>
#include "battbar.h"
#include "hybrid.h"
  
static BitmapLayer *image_layer_battery;
static BitmapLayer *image_layer_icon;
static GBitmap *image_line;
static GBitmap *image_battery;
static BBOptions options;
static Layer *current_window;

void battbar(void) {
	options.position = BATTBAR_POSITION_BOTTOM;
	options.direction = BATTBAR_DIRECTION_DOWN;
	options.color = BATTBAR_COLOR_BLACK;
	options.isWatchApp = true;
}

static void battbar_handle_battery(BatteryChargeState charge_state) {
	DrawBattBar(charge_state);
  drawInvert();
}

void CleanupBattBar(){
	bitmap_layer_destroy(image_layer_battery);
	bitmap_layer_destroy(image_layer_icon);
	gbitmap_destroy(image_line);
	gbitmap_destroy(image_battery);
}

void SetupBattBar(BBOptions myOptions, Layer *myWindow) {
	options = myOptions;
	current_window = myWindow;
	battery_state_service_subscribe(&battbar_handle_battery);
}

void DrawBattBar() {
	BatteryChargeState charge_state = battery_state_service_peek();
	RefreshBattBar(charge_state);
}

void RefreshBattBar(BatteryChargeState charge_state) {
	layer_remove_from_parent(bitmap_layer_get_layer(image_layer_battery));	
	layer_remove_from_parent(bitmap_layer_get_layer(image_layer_icon));
	
	int height = 168; // Without Title Bar
	int width = 144;
	int bar_size = 4;
	double segment = 1.28;
	if(options.isWatchApp) {
		height = 152; // With Title Bar
		segment = 1.52;
	}
	
	uint8_t raw_percent = charge_state.charge_percent;
	uint8_t percent_display = raw_percent * segment;
	bool isCharging = charge_state.is_charging;
	
	
	
	
	if(options.position == BATTBAR_POSITION_BOTTOM || options.position == BATTBAR_POSITION_TOP) {
		segment = 1.28;
		percent_display = raw_percent * segment;
		if(isCharging) {
			image_line = gbitmap_create_with_resource(RESOURCE_ID_CHARGING_LINE_LONG);
			image_battery = gbitmap_create_with_resource(RESOURCE_ID_BOLT_ICON);
		} else {
      
      if(raw_percent >= 60){
        image_battery = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_FULL);
      } else if(raw_percent < 60 && raw_percent >= 20){
        image_battery = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_HALF);
      }else {
        image_battery = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_EMPTY);
      }
      
			
			if(options.color == BATTBAR_COLOR_BLACK) {
				image_line = gbitmap_create_with_resource(RESOURCE_ID_BLACK_LINE_LONG);
			} else {
				image_line = gbitmap_create_with_resource(RESOURCE_ID_WHITE_LINE_LONG);
			}
		}
			
		if(options.position == BATTBAR_POSITION_TOP) {
			image_layer_battery = bitmap_layer_create(GRect(20,168-height,percent_display,bar_size));
			image_layer_icon = bitmap_layer_create(GRect(0,4,20,25));
		} else {
			image_layer_battery = bitmap_layer_create(GRect(20,height,percent_display,bar_size));
			image_layer_icon = bitmap_layer_create(GRect(0,140,20,25));
		}
	}
	
	bitmap_layer_set_bitmap(image_layer_battery, image_line);
	bitmap_layer_set_bitmap(image_layer_icon, image_battery);
	
	layer_add_child(current_window, bitmap_layer_get_layer(image_layer_icon));
	layer_add_child(current_window, bitmap_layer_get_layer(image_layer_battery));

	
}

